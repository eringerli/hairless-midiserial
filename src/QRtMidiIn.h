#pragma once

/*
 * Very thin QT wrapper for the cross-platform RtMidiIn class.
 *
 * Exposes the "new MIDI message" callback as a QT signal, which can be sent to
 * a QT slot.
 */
#include "QMidi.h"
#include "RtMidi.h"

#include <QObject>

class QRtMidiIn
    : public QObject
    , public RtMidiIn {
  Q_OBJECT

public:
  QRtMidiIn(
    const std::string& clientName = std::string( "QRtMidi Input Client" ) );
  void midiCallback( double timeStamp, MidiMsg* message );
  virtual ~QRtMidiIn( void ) throw() {}
signals:
  void midiReceived();
  void messageReceived( MidiMsg message );
};
