#include "a4layoutpopup.h"
#include "tapp.h"
#include "toonz/preferences.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QSpinBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QFileInfo>
#include <QDir>

A4LayoutPopup::A4LayoutPopup(QWidget *parent)
    : QDialog(parent) {
  setWindowTitle(tr("Export A4 Frame Layout (Scanning-in-reverse)"));

  QVBoxLayout *mainLayout = new QVBoxLayout(this);

  // Video Path
  QHBoxLayout *videoLayout = new QHBoxLayout();
  m_videoPath = new QLineEdit(this);
  m_videoPath->setPlaceholderText(tr("Select Input Video/Sequence..."));
  QPushButton *browseVideoBtn = new QPushButton(tr("Browse..."), this);
  connect(browseVideoBtn, &QPushButton::clicked, this, &A4LayoutPopup::chooseVideo);
  videoLayout->addWidget(new QLabel(tr("Input Video:")));
  videoLayout->addWidget(m_videoPath);
  videoLayout->addWidget(browseVideoBtn);
  mainLayout->addLayout(videoLayout);

  // Output Folder
  QHBoxLayout *outLayout = new QHBoxLayout();
  m_outputFolder = new QLineEdit(this);
  m_outputFolder->setPlaceholderText(tr("Select Output Folder..."));
  QPushButton *browseOutBtn = new QPushButton(tr("Browse..."), this);
  connect(browseOutBtn, &QPushButton::clicked, this, &A4LayoutPopup::chooseOutputFolder);
  outLayout->addWidget(new QLabel(tr("Output Folder:")));
  outLayout->addWidget(m_outputFolder);
  outLayout->addWidget(browseOutBtn);
  mainLayout->addLayout(outLayout);

  // Columns and Rows
  QHBoxLayout *gridSettingsLayout = new QHBoxLayout();
  m_columns = new QSpinBox(this);
  m_columns->setRange(1, 20);
  m_columns->setValue(4);
  m_rows = new QSpinBox(this);
  m_rows->setRange(1, 20);
  m_rows->setValue(6);
  gridSettingsLayout->addWidget(new QLabel(tr("Columns:")));
  gridSettingsLayout->addWidget(m_columns);
  gridSettingsLayout->addWidget(new QLabel(tr("Rows:")));
  gridSettingsLayout->addWidget(m_rows);
  mainLayout->addLayout(gridSettingsLayout);

  // Status
  m_status = new QLabel(tr("Ready"), this);
  mainLayout->addWidget(m_status);

  // Export Button
  m_exportButton = new QPushButton(tr("Generate A4 Layouts"), this);
  connect(m_exportButton, &QPushButton::clicked, this, &A4LayoutPopup::beginExport);
  mainLayout->addWidget(m_exportButton);

  connect(&m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
          this, &A4LayoutPopup::onProcessFinished);
}

void A4LayoutPopup::chooseVideo() {
  QString path = QFileDialog::getOpenFileName(this, tr("Select Video"), "", tr("Video Files (*.mp4 *.mov *.avi *.mkv *.webm)"));
  if (!path.isEmpty()) {
    m_videoPath->setText(path);
    if (m_outputFolder->text().isEmpty()) {
      m_outputFolder->setText(QFileInfo(path).absolutePath());
    }
  }
}

void A4LayoutPopup::chooseOutputFolder() {
  QString path = QFileDialog::getExistingDirectory(this, tr("Select Output Folder"));
  if (!path.isEmpty()) {
    m_outputFolder->setText(path);
  }
}

void A4LayoutPopup::beginExport() {
  startExporting();
}

void A4LayoutPopup::startExporting() {
  if (m_videoPath->text().isEmpty() || m_outputFolder->text().isEmpty()) {
    QMessageBox::warning(this, tr("Warning"), tr("Please select an input video and an output folder."));
    return;
  }

  QString ffmpeg = ffmpegExecutable();
  if (ffmpeg.isEmpty()) {
    QMessageBox::warning(this, tr("Error"), tr("FFmpeg not found in preferences."));
    return;
  }

  m_status->setText(tr("Generating layouts..."));
  m_exportButton->setEnabled(false);

  QString input = m_videoPath->text();
  QString output = QDir(m_outputFolder->text()).filePath("A4_Layout_%03d.png");

  // e.g. tile=4x6
  QString tileFilter = QString("scale=iw*0.5:ih*0.5,tile=%1x%2").arg(m_columns->value()).arg(m_rows->value());

  QStringList args;
  args << "-i" << input << "-vf" << tileFilter << output;

  m_process.start(ffmpeg, args);
}

void A4LayoutPopup::onProcessFinished(int exitCode, QProcess::ExitStatus status) {
  m_exportButton->setEnabled(true);
  if (exitCode == 0 && status == QProcess::NormalExit) {
    m_status->setText(tr("Layout generation complete!"));
    QMessageBox::information(this, tr("Success"), tr("A4 layouts exported successfully."));
  } else {
    m_status->setText(tr("Extraction failed!"));
    QMessageBox::warning(this, tr("Error"), tr("FFmpeg encountered an error."));
  }
}

QString A4LayoutPopup::ffmpegExecutable() const {
  QString path = Preferences::instance()->getStringValue(ffmpegPath);
  if (path.isEmpty()) return "";
  QFileInfo fi(path, "ffmpeg.exe");
  if (fi.exists()) return fi.absoluteFilePath();
  fi = QFileInfo(path, "ffmpeg");
  if (fi.exists()) return fi.absoluteFilePath();
  return "";
}
