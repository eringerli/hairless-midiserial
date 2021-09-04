#pragma once

#include "QRtMidiIn.h"
#include "QRtMidiOut.h"

#include <QObject>

class QRtMidiInOut : public QObject {
  Q_OBJECT
public:
  explicit QRtMidiInOut( const QString& name, QObject* parent = nullptr );
  ~QRtMidiInOut();

  void attach( int midiInPort, int midiOutPort, QThread* workerThread );
signals:

  void midiSent();

  void midiReceived();
  void messageReceived( MidiMsg message );

  void displayMessage( QString message );
  void debugMessage( QString message );

public slots:
  void sendMessage( MidiMsg& message );

private:
  QRtMidiIn*  midiIn      = nullptr;
  QRtMidiOut* midiOut     = nullptr;
  int         midiInPort  = 0;
  int         midiOutPort = 0;

  QString name;
};
