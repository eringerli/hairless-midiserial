#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "RtMidi.h"
#include "ui/BridgeHead.h"

#include <QTime>
#include <QTimer>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>

namespace Ui {
  class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow( QWidget* parent = 0 );
  ~MainWindow();

  void setConsoleOutputFromCommandLine( bool consoleOutput );

private:
  Ui::MainWindow* ui;
  QPixmap         pxLedOn;
  QPixmap         pxLedOff;
  QThread*        workerThread = nullptr;
  int             scrollbackSize;
  QTimer          debugListTimer;
  QStringList     debugListMessages;
  bool            consoleOutput = false;

  BridgeHead* leftBridgeHead  = nullptr;
  BridgeHead* rightBridgeHead = nullptr;

private slots:
  void onDisplayMessage( const QString& message );
  void onDebugMessage( const QString& message );
  void refreshDebugList();
  void showAboutBox();
  void on_pbAbout_clicked();
};

#endif // MAINWINDOW_H
