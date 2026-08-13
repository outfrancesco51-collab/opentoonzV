#pragma once

#include <QDialog>
#include <QProcess>

class QLineEdit;
class QSpinBox;
class QPushButton;
class QLabel;

// Local, explicit FFmpeg workflow. No media is uploaded and input/output paths
// remain on the artist's workstation.
class VideoExtractPopup final : public QDialog {
  Q_OBJECT
public:
  explicit VideoExtractPopup(QWidget *parent = nullptr);

private slots:
  void chooseVideo();
  void chooseOutputFolder();
  void beginExtraction();
  void onProcessFinished(int exitCode, QProcess::ExitStatus status);

private:
  void startFrameExtraction();
  void startAudioExtraction();
  QString ffmpegExecutable() const;

  QLineEdit *m_videoPath;
  QLineEdit *m_outputFolder;
  QSpinBox *m_fps;
  QLabel *m_status;
  QPushButton *m_extractButton;
  QProcess m_process;
  bool m_extractingAudio = false;
};
