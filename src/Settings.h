#ifndef SETTINGS_H
#define SETTINGS_H

#include "qextserialport.h"

#include <QSettings>

// Note that main.cpp sets QCoreApplication::applicationName and such, allowing
// us to use a plain settings constructor

class Settings {
public:
  explicit Settings( QString name ) {
    settings.beginGroup( name );
  }

  ~Settings() {
    settings.endGroup();
  }

  bool getDebug() {
    return settings.value( "debug", false ).toBool();
  }
  void setDebug( bool debug ) {
    settings.setValue( "debug", debug );
  }

  QString getLastType() {
    return settings.value( "lastType", "MIDI" ).toString();
  }
  void setLastType( QString type ) {
    settings.setValue( "lastType", type );
  }

  QString getLastMidiIn() {
    return settings.value( "lastMidiIn", "" ).toString();
  }
  void setLastMidiIn( QString port ) {
    settings.setValue( "lastMidiIn", port );
  }

  QString getLastMidiOut() {
    return settings.value( "lastMidiOut", "" ).toString();
  }
  void setLastMidiOut( QString port ) {
    settings.setValue( "lastMidiOut", port );
  }

  QString getLastSerialPort() {
    return settings.value( "lastSerialPort", "" ).toString();
  }
  void setLastSerialPort( QString port ) {
    settings.setValue( "lastSerialPort", port );
  }

  PortSettings getPortSettings() {
    PortSettings result;
    result.BaudRate =
      ( BaudRateType )settings.value( "baudRate", ( int )BAUD115200 ).toInt();
    result.DataBits =
      ( DataBitsType )settings.value( "dataBits", ( int )DATA_8 ).toInt();
    result.FlowControl =
      ( FlowType )settings.value( "flowControl", ( int )FLOW_OFF ).toInt();
    result.Parity =
      ( ParityType )settings.value( "parity", ( int )PAR_NONE ).toInt();
    result.StopBits =
      ( StopBitsType )settings.value( "stopBits", ( int )STOP_1 ).toInt();
    result.Timeout_Millisec =
      1; // not used when qextserialport is event-driven, anyhow
    return result;
  }

  void setPortSettings( PortSettings newSettings ) {
    settings.setValue( "baudRate", ( int )newSettings.BaudRate );
    settings.setValue( "dataBits", ( int )newSettings.DataBits );
    settings.setValue( "flowControl", ( int )newSettings.FlowControl );
    settings.setValue( "parity", ( int )newSettings.Parity );
    settings.setValue( "stopBits", ( int )newSettings.StopBits );
  }

  static bool getConsoleOutput() {
    return QSettings().value( "consoleOutput", false ).toBool();
  }
  static void setConsoleOutput( bool consoleOutput ) {
    QSettings().setValue( "consoleOutput", consoleOutput );
  }

  static int getScrollbackSize() {
    return QSettings().value( "scrollbackSize", 75 ).toInt();
  }
  static void setScrollbackSize( int newSize ) {
    QSettings().setValue( "scrollbackSize", newSize );
  }

private:
  QSettings settings;
};

#endif // SETTINGS_H
