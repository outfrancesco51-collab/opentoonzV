#include "videoextractpopup.h"

#include <QDialogButtonBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QStandardPaths>
#include <QVBoxLayout>

VideoExtractPopup::VideoExtractPopup(QWidget *parent) : QDialog(parent) {
  setWindowTitle(tr("Video Extract Frame"));
  setMinimumWidth(560);
  auto *layout = new QVBoxLayout(this);
  auto *form = new QFormLayout;
  m_videoPath = new QLineEdit(this);
  auto *chooseVideoButton = new QPushButton(tr("Browse..."), this);
  auto *videoRow = new QHBoxLayout;
  videoRow->addWidget(m_videoPath); videoRow->addWidget(chooseVideoButton);
  form->addRow(tr("Video input:"), videoRow);
  m_outputFolder = new QLineEdit(this);
  auto *chooseOutputButton = new QPushButton(tr("Folder..."), this);
  auto *outputRow = new QHBoxLayout;
  outputRow->addWidget(m_outputFolder); outputRow->addWidget(chooseOutputButton);
  form->addRow(tr("Output folder:"), outputRow);
  m_fps = new QSpinBox(this); m_fps->setRange(1, 120); m_fps->setValue(24);
  form->addRow(tr("Frames per second:"), m_fps);
  layout->addLayout(form);
  layout->addWidget(new QLabel(tr("Creates numbered PNG frames and preserves the original audio stream when available."), this));
  m_status = new QLabel(this); layout->addWidget(m_status);
  auto *buttons = new QDialogButtonBox(QDialogButtonBox::Cancel, this);
  m_extractButton = buttons->addButton(tr("Extract"), QDialogButtonBox::AcceptRole);
  layout->addWidget(buttons);
  connect(chooseVideoButton, SIGNAL(clicked()), this, SLOT(chooseVideo()));
  connect(chooseOutputButton, SIGNAL(clicked()), this, SLOT(chooseOutputFolder()));
  connect(m_extractButton, SIGNAL(clicked()), this, SLOT(beginExtraction()));
  connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
  connect(&m_process, SIGNAL(finished(int,QProcess::ExitStatus)), this,
          SLOT(onProcessFinished(int,QProcess::ExitStatus)));
}

void VideoExtractPopup::chooseVideo() {
  const QString path = QFileDialog::getOpenFileName(this, tr("Choose video"), "", tr("Video Files (*.mp4 *.mov *.avi *.mkv *.webm);;All Files (*)"));
  if (!path.isEmpty()) m_videoPath->setText(path);
}
void VideoExtractPopup::chooseOutputFolder() {
  const QString path = QFileDialog::getExistingDirectory(this, tr("Choose output folder"));
  if (!path.isEmpty()) m_outputFolder->setText(path);
}
QString VideoExtractPopup::ffmpegExecutable() const {
  return QStandardPaths::findExecutable("ffmpeg");
}
void VideoExtractPopup::beginExtraction() {
  if (!QFileInfo::exists(m_videoPath->text()) || m_outputFolder->text().isEmpty()) {
    QMessageBox::warning(this, tr("Video Extract Frame"), tr("Select a valid video and output folder.")); return;
  }
  if (ffmpegExecutable().isEmpty()) {
    QMessageBox::warning(this, tr("FFmpeg unavailable"), tr("Install FFmpeg or set it in Preferences before extracting media.")); return;
  }
  m_extractButton->setEnabled(false); m_extractingAudio = false; startFrameExtraction();
}
void VideoExtractPopup::startFrameExtraction() {
  m_status->setText(tr("Extracting PNG frames..."));
  const QString output = m_outputFolder->text() + "/frame_%06d.png";
  m_process.start(ffmpegExecutable(), {"-hide_banner", "-y", "-i", m_videoPath->text(),
                    "-vf", QString("fps=%1").arg(m_fps->value()), "-vsync", "0", output});
}
void VideoExtractPopup::startAudioExtraction() {
  m_extractingAudio = true; m_status->setText(tr("Extracting audio..."));
  const QString output = m_outputFolder->text() + "/audio.mka";
  m_process.start(ffmpegExecutable(), {"-hide_banner", "-y", "-i", m_videoPath->text(),
                    "-map", "0:a?", "-c:a", "copy", output});
}
void VideoExtractPopup::onProcessFinished(int exitCode, QProcess::ExitStatus status) {
  if (status != QProcess::NormalExit || exitCode != 0) {
    QMessageBox::critical(this, tr("Video Extract Frame"), tr("FFmpeg failed. Check the video codec and FFmpeg path."));
    m_extractButton->setEnabled(true); return;
  }
  if (!m_extractingAudio) { startAudioExtraction(); return; }
  m_status->setText(tr("Done: PNG frames and audio.mka created.")); m_extractButton->setEnabled(true);
}
