#include "mainwindow.h"

#include "QMidi.h"
#include "Settings.h"
#include "aboutdialog.h"
#include "qextserialenumerator.h"
#include "ui/MidiFrame.h"
#include "ui/SerialFrame.h"
#include "ui_mainwindow.h"

#include <QTextStream>
#include <utility>

const int LIST_REFRESH_RATE = 20; // Hz

MainWindow::MainWindow( QWidget* parent )
    : QMainWindow( parent )
    , ui( new Ui::MainWindow ) {
  ui->setupUi( this );
#ifdef Q_OS_LINUX
  this->setWindowIcon( QIcon( ":/images/images/icon48.png" ) );
#endif

  // Load initial state
  this->workerThread = new QThread();
  this->workerThread->start( QThread::HighestPriority );
  scrollbackSize = Settings::getScrollbackSize();

  // add the two bridgeheads
  leftBridgeHead   = new BridgeHead( "left", workerThread, ui->bridgeHeadLeft );
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
    new BridgeHead( "right", workerThread, ui->bridgeHeadRight );
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

  // Set up timer for the display list
  debugListTimer.setSingleShot( true );
  debugListTimer.setInterval( 1000 / LIST_REFRESH_RATE );

  // Plumb signals & slots
  connect( &debugListTimer, SIGNAL( timeout() ), SLOT( refreshDebugList() ) );

  connect( leftBridgeHead,
           &BridgeHead::messageReceived,
           rightBridgeHead,
           &BridgeHead::sendMessage );
  connect( rightBridgeHead,
           &BridgeHead::messageReceived,
           leftBridgeHead,
           &BridgeHead::sendMessage );
}

MainWindow::~MainWindow() {
  delete ui;
  delete leftBridgeHead;
  delete rightBridgeHead;
}

void MainWindow::showAboutBox() {
  AboutDialog().exec();
}

void MainWindow::onDisplayMessage( const QString& message ) {
  if( Settings::getConsoleOutput() ) {
    QTextStream out( stdout );
    out << message << Qt::endl;
  }
  if( debugListMessages.size() == scrollbackSize ) {
    debugListMessages.removeFirst();
  }
  debugListMessages.append( message );
  if( !debugListTimer.isActive() ) {
    debugListTimer.start();
  }
}

void MainWindow::onDebugMessage( const QString& message ) {
  onDisplayMessage( message );
}

/*
 * When the timer (started in onDisplayMessage) fires, we update lst_debug with
 * the contents of debugListMessages.
 *
 * This happens in the timer event in order to rate limit it to a number of
 * redraws per second (redrawing, and especially scrolling the list view, can be
 * quite resource intensive.)
 */
void MainWindow::refreshDebugList() {
  QListWidget* lst = ui->lst_debug;
  while( lst->count() + debugListMessages.size() - scrollbackSize > 0 &&
         lst->count() > 0 ) {
    delete lst->item( 0 );
  }
  lst->addItems( debugListMessages );
  debugListMessages.clear();
  lst->scrollToBottom();
}

void MainWindow::setConsoleOutputFromCommandLine( bool c ) {
  this->consoleOutput = c;
}

void MainWindow::on_pbAbout_clicked() {
  showAboutBox();
}
