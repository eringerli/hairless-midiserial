#include "mainwindow.h"

#include "QMidi.h"
#include "Settings.h"
#include "aboutdialog.h"
#include "qextserialenumerator.h"
#include "ui/MidiFrame.h"
#include "ui/SerialFrame.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QTextStream>
#include <utility>

MainWindow::MainWindow( QWidget* parent )
    : QMainWindow( parent )
    , ui( new Ui::MainWindow ) {
  ui->setupUi( this );
#ifdef Q_OS_LINUX
  this->setWindowIcon( QIcon( ":/images/images/icon48.png" ) );
#endif

  qRegisterMetaType< MidiMsg >();

  // Load initial state
  this->workerThread = new QThread();
  this->workerThread->start( QThread::HighestPriority );

  ui->cbEnableLua->setChecked( false /*Settings::getLastLuaState()*/ );
  on_cbEnableLua_stateChanged( 0 );

  ui->lbLuaFilename->setText( Settings::getLastLuaFile() );

  ui->cbLuaDebug->setChecked( Settings::getLastLuaDebugState() );
  on_cbLuaDebug_stateChanged( 0 );

  ui->cbAutoScroll->setChecked( Settings::getAutoScroll() );

  // add the two bridgeheads
  leftBridgeHead =
    new BridgeHead( QString( "left" ), workerThread, ui->bridgeHeadLeft );
  auto* leftLayout = new QHBoxLayout( ui->bridgeHeadLeft );
  leftLayout->addWidget( leftBridgeHead );
  connect( leftBridgeHead,
           &BridgeHead::displayMessage,
           this,
           &MainWindow::onDisplayMessage );
  connect( leftBridgeHead,
           &BridgeHead::debugMessage,
           this,
           &MainWindow::onDebugMessage );

  rightBridgeHead =
    new BridgeHead( QString( "right" ), workerThread, ui->bridgeHeadRight );
  auto* rightLayout = new QHBoxLayout( ui->bridgeHeadRight );
  rightLayout->addWidget( rightBridgeHead );
  connect( rightBridgeHead,
           &BridgeHead::displayMessage,
           this,
           &MainWindow::onDisplayMessage );
  connect( rightBridgeHead,
           &BridgeHead::debugMessage,
           this,
           &MainWindow::onDebugMessage );

  plumbSignalsForLua();
}

MainWindow::~MainWindow() {

  delete ui;
  delete leftBridgeHead;
  delete rightBridgeHead;
  delete luaMidiInOut;
}

void MainWindow::showAboutBox() {
  AboutDialog().exec();
}

void MainWindow::onDisplayMessage( const QString& message ) {
  if( Settings::getConsoleOutput() ) {
    QTextStream out( stdout );
    out << message << Qt::endl;
  }

  auto cursor = ui->teDisplay->textCursor();
  cursor.movePosition( QTextCursor::MoveOperation::End );
  auto format = QTextCharFormat();
  format.setForeground( Qt::darkRed );
  cursor.insertText( "\n", format );
  cursor.insertText( message, format );
  on_teDisplay_textChanged();
}

void MainWindow::onDebugMessage( const QString& message ) {
  auto cursor = ui->teDisplay->textCursor();
  cursor.movePosition( QTextCursor::MoveOperation::End );
  auto format = QTextCharFormat();
  format.setForeground( Qt::darkBlue );
  cursor.insertText( "\n", format );
  cursor.insertText( message, format );
  on_teDisplay_textChanged();
}

void MainWindow::setConsoleOutputFromCommandLine( bool c ) {
  this->consoleOutput = c;
}

void MainWindow::on_pbAbout_clicked() {
  showAboutBox();
}

void MainWindow::on_cbEnableLua_stateChanged( int ) {
  Settings::setLastLuaState( ui->cbEnableLua->isChecked() );
  if( ui->cbEnableLua->isChecked() ) {
    if( luaMidiInOut == nullptr ) {
      luaMidiInOut = new LuaMidiInOut( "middle", workerThread );
      connect( luaMidiInOut,
               &LuaMidiInOut::debugMessage,
               this,
               &MainWindow::onDebugMessage );
      connect( luaMidiInOut,
               &LuaMidiInOut::displayMessage,
               this,
               &MainWindow::onDisplayMessage );
      auto fileName = ui->lbLuaFilename->text();
      if( !fileName.isEmpty() ) {
        luaMidiInOut->openLuaFile( fileName );
      }
    }
  } else {
    if( luaMidiInOut != nullptr ) {
      luaMidiInOut->deleteLater();
      luaMidiInOut = nullptr;
    }
  }

  plumbSignalsForLua();
}

void MainWindow::on_pbOpenLuaFile_clicked() {
  auto fileName = QFileDialog::getOpenFileName( this,
                                                "Open Lua Script for Filtering",
                                                QString(),
                                                "Lua Scripts (*.lua)" );
  if( !fileName.isEmpty() ) {
    Settings::setLastLuaFile( fileName );
    if( luaMidiInOut != nullptr ) {
      luaMidiInOut->openLuaFile( fileName );
    }
  }
}

void MainWindow::on_teDisplay_textChanged() {
  if( ui->cbAutoScroll->isChecked() ) {
    ui->teDisplay->moveCursor( QTextCursor::MoveOperation::End,
                               QTextCursor::MoveAnchor );
  }
}

void MainWindow::on_pbClear_clicked() {
  ui->teDisplay->clear();
}

void MainWindow::on_cbAutoScroll_stateChanged( int ) {
  Settings::setAutoScroll( ui->cbAutoScroll->isChecked() );
}

void MainWindow::on_cbLuaDebug_stateChanged( int ) {
  if( luaMidiInOut != nullptr ) {
    luaMidiInOut->changeDebug( ui->cbLuaDebug->isChecked() );
  }
}

void MainWindow::plumbSignalsForLua() {
  if( luaMidiInOut == nullptr ) {
    luaShortcutConnection1 = connect( leftBridgeHead,
                                      &BridgeHead::messageReceived,
                                      rightBridgeHead,
                                      &BridgeHead::sendMessage );
    luaShortcutConnection2 = connect( rightBridgeHead,
                                      &BridgeHead::messageReceived,
                                      leftBridgeHead,
                                      &BridgeHead::sendMessage );
  } else {
    disconnect( luaShortcutConnection1 );
    disconnect( luaShortcutConnection2 );
    if( leftBridgeHead != nullptr ) {
      connect( luaMidiInOut,
               &LuaMidiInOut::messageReceivedLeft,
               leftBridgeHead,
               &BridgeHead::sendMessage );
      connect( leftBridgeHead,
               &BridgeHead::messageReceived,
               luaMidiInOut,
               &LuaMidiInOut::sendMessageLeft );
    }

    if( rightBridgeHead != nullptr ) {
      connect( luaMidiInOut,
               &LuaMidiInOut::messageReceivedRight,
               rightBridgeHead,
               &BridgeHead::sendMessage );
      connect( rightBridgeHead,
               &BridgeHead::messageReceived,
               luaMidiInOut,
               &LuaMidiInOut::sendMessageRight );
    }
  }
}

void MainWindow::on_pbLuaReload_clicked() {
  if( ui->cbEnableLua->isChecked() ) {
    ui->cbEnableLua->setChecked( false );
    ui->cbEnableLua->setChecked( true );
  }
}
