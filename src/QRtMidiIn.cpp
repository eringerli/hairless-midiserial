#include "QRtMidiIn.h"

static void
midiCallbackOuter( double timeStamp, MidiMsg* message, void* userData ) {
  auto* midi = ( QRtMidiIn* )userData;
  midi->midiCallback( timeStamp, message );
}

QRtMidiIn::QRtMidiIn( const std::string& clientName )
    : QObject( nullptr )
    , RtMidiIn( RtMidi::UNSPECIFIED, clientName ) {
  this->ignoreTypes( false,
                     false,
                     false ); // we want MIDI SysEx, time & sense messages
  this->setCallback( midiCallbackOuter, this );
}

void QRtMidiIn::midiCallback( double /*timeStamp*/, MidiMsg* message ) {
  emit messageReceived( MidiMsg( *message ) );
  emit midiReceived();
}
