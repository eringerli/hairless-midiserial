#include "Settings.h"
#include "ui/mainwindow.h"

#include <QCommandLineParser>
#include <QDebug>
#include <QtWidgets/QApplication>

void myMessageOutput( QtMsgType,
                      const QMessageLogContext&,
                      const QString& msg ) {
  QTextStream cout( stdout, QIODevice::WriteOnly );
  cout << msg << Qt::endl;
}

int main( int argc, char* argv[] ) {
  QApplication a( argc, argv );
  QCoreApplication::setOrganizationName( "Projectgus2" );
  QCoreApplication::setOrganizationDomain( "projectgus.com2" );
  QCoreApplication::setApplicationName( "Hairless MIDI Serial Bridge2" );
  QCoreApplication::setApplicationVersion( QString::fromUtf8( VERSION ) );

  qInstallMessageHandler( myMessageOutput );

  QCommandLineParser parser;
  parser.setApplicationDescription(
    "Sends and receives MIDI events over a serial device." );
  parser.addHelpOption();
  parser.addVersionOption();
  QCommandLineOption showDebugOption( QStringList() << "d"
                                                    << "debug",
                                      "Show debug MIDI messages" );
  parser.addOption( showDebugOption );
  QCommandLineOption consoleOutputOption( QStringList() << "c"
                                                        << "console-output",
                                          "Output messages to the console" );
  parser.addOption( consoleOutputOption );
  QCommandLineOption hideWindowOption( QStringList() << "w"
                                                     << "hide-window",
                                       "Hides the GUI" );
  parser.addOption( hideWindowOption );

  QCommandLineOption serialPortOption(
    QStringList() << "s"
                  << "serial",
    "Specify the name of the serial device to use",
    "serial" );
  parser.addOption( serialPortOption );
  QCommandLineOption midiInOption( QStringList() << "i"
                                                 << "midi-in",
                                   "Specify the name of MIDI in device to use",
                                   "midi-in" );
  parser.addOption( midiInOption );
  QCommandLineOption midiOutOption(
    QStringList() << "o"
                  << "midi-out",
    "Specify the name of MIDI out device to use",
    "midi-out" );
  parser.addOption( midiOutOption );

  parser.process( a );
  Settings::setConsoleOutput( parser.isSet( consoleOutputOption ) );
  MainWindow w;
  if( !parser.isSet( hideWindowOption ) ) {
    w.show();
  }
  return QApplication::exec();
}
