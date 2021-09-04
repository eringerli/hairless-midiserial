#pragma once

#include "PortLatency.h"
#include "QMidi.h"
#include "qextserialport.h"

#include <QObject>
#include <QThread>
#include <QTime>

class QSerialMidiInOut : public QObject {
  Q_OBJECT
public:
  explicit QSerialMidiInOut( QObject* parent = nullptr );
  ~QSerialMidiInOut();

  void attach( const QString& serialName,
               PortSettings   serialSettings,
               QThread*       workerThread );

signals:
  void serialTraffic();

  void midiSent();

  void midiReceived();
  void messageReceived( MidiMsg message );

  void displayMessage( QString message );
  void debugMessage( QString message );

public slots:
  void sendMessage( MidiMsg message );

private slots:
  void onSerialAvailable();

private:
  void sendMidiMessage();

  void    onDataByte( uint8_t byte );
  void    onStatusByte( uint8_t byte );
  QString applyTimeStamp( const QString& message );

  bool bufferStartsWith( uint8_t byte );

  int running_status =
    0; // if we have a running or current status byte, what is it?
  int data_expected = 0; // how many bytes of data are we currently waiting on?
  QByteArray      msg_data; // accumulated message data from the serial port
  QextSerialPort* serial  = nullptr;
  PortLatency*    latency = nullptr;
  QTime           attachTime;
};
