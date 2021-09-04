#pragma once

/*
 * Very thin QT wrapper for the cross-platform RtMidiOut class.
 *
 * Exposes the "send MIDI message" method as a QT slot.
 */
#include "QMidi.h"
#include "RtMidi.h"

#include <QObject>

class QRtMidiOut
    : public QObject
    , public RtMidiOut {
  Q_OBJECT

public:
  QRtMidiOut(
    const std::string& clientName = std::string( "QRtMidi Output Client" ) );
  virtual ~QRtMidiOut( void ) throw() {}

signals:
  void midiSent();

public slots:
  void sendMessage( MidiMsg& message );
};
