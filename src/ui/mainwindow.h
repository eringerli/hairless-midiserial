#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Bridge.h"
#include "RtMidi.h"

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

  bool eventFilter( QObject*, QEvent* );

  void setDebugFromCommandLine( bool showDebug );
  void setConsoleOutputFromCommandLine( bool consoleOutput );

private:
  Ui::MainWindow* ui;
  Bridge*         bridge;
  QPixmap         pxLedOn;
  QPixmap         pxLedOff;
  QThread*        workerThread;
  int             scrollbackSize;
  QTimer          debugListTimer;
  QStringList     debugListMessages;
  bool            consoleOutput = false;

  void refresh();
  void refreshSerial();
  void refreshMidiIn();
  void refreshMidiOut();
  void refreshMidi( QComboBox* combo, RtMidi* midi );

private slots:
  void onValueChanged();
  void onDisplayMessage( const QString& message );
  void onDebugMessage( QString message );
  void onDebugClicked( bool value );
  void refreshDebugList();
  void showPreferences();
  void showAboutBox();
  void on_pbSettings_clicked();
  void on_pbAbout_clicked();
};

#endif // MAINWINDOW_H
