#include "QRtMidiOut.h"

QRtMidiOut::QRtMidiOut( const std::string& clientName )
    : QObject( nullptr )
    , RtMidiOut( RtMidi::UNSPECIFIED, clientName ) {}

void QRtMidiOut::sendMessage( MidiMsg& message ) {
  RtMidiOut::sendMessage( &message );
  emit midiSent();
}
