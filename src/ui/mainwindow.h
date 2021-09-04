#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "LuaMidiInOut.h"
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
  QThread*        workerThread  = nullptr;
  bool            consoleOutput = false;

  BridgeHead*   leftBridgeHead  = nullptr;
  BridgeHead*   rightBridgeHead = nullptr;
  LuaMidiInOut* luaMidiInOut    = nullptr;

  QMetaObject::Connection luaShortcutConnection1;
  QMetaObject::Connection luaShortcutConnection2;

  void plumbSignalsForLua();

private slots:
  void onDisplayMessage( const QString& message );
  void onDebugMessage( const QString& message );
  void showAboutBox();
  void on_pbAbout_clicked();
  void on_cbEnableLua_stateChanged( int arg1 );
  void on_pbOpenLuaFile_clicked();
  void on_teDisplay_textChanged();
  void on_pbClear_clicked();
  void on_cbAutoScroll_stateChanged( int arg1 );
  void on_cbLuaDebug_stateChanged( int );
  void on_pbLuaReload_clicked();
};

#endif // MAINWINDOW_H
