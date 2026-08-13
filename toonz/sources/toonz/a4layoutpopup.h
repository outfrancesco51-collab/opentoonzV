#pragma once

#include <QDialog>
#include <QProcess>

class QLineEdit;
class QSpinBox;
class QPushButton;
class QLabel;
class QCheckBox;

// Export animation frames into an A4 Layout Sheet (Scanning-in-reverse)
class A4LayoutPopup final : public QDialog {
  Q_OBJECT
public:
  explicit A4LayoutPopup(QWidget *parent = nullptr);

private slots:
  void chooseVideo();
  void chooseOutputFolder();
  void beginExport();
  void onProcessFinished(int exitCode, QProcess::ExitStatus status);

private:
  void startExporting();
  QString ffmpegExecutable() const;

  QLineEdit *m_videoPath;
  QLineEdit *m_outputFolder;
  QSpinBox *m_columns;
  QSpinBox *m_rows;
  QLabel *m_status;
  QPushButton *m_exportButton;
  QProcess m_process;
};
