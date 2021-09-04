#include "QRtMidiInOut.h"

QRtMidiInOut::QRtMidiInOut( QObject* parent )
    : QObject( parent ) {}

void QRtMidiInOut::attach( int      midiInPort,
                           int      midiOutPort,
                           QThread* workerThread ) {
  this->moveToThread( workerThread );

  // MIDI out
  try {
    if( midiOutPort > -1 ) {
      emit displayMessage(
        QStringLiteral( "Opening MIDI Out port #%1" ).arg( midiOutPort ) );
      this->midiOutPort = midiOutPort;
      this->midiOut     = new QRtMidiOut( NAME_MIDI_OUT );
      this->midiOut->moveToThread( workerThread );
      this->midiOut->openPort( midiOutPort );

      QObject::connect( this->midiOut,
                        &QRtMidiOut::midiSent,
                        this,
                        &QRtMidiInOut::midiSent );
    }
  } catch( RtMidiError& e ) {
    emit displayMessage( QStringLiteral( "Failed to open MIDI out port: %1" )
                           .arg( QString::fromStdString( e.getMessage() ) ) );
    this->midiOut->deleteLater();
    this->midiOut = nullptr;
  }

  // MIDI in
  try {
    if( midiInPort > -1 ) {
      emit displayMessage(
        QStringLiteral( "Opening MIDI In port #%1" ).arg( midiInPort ) );
      this->midiInPort = midiInPort;
      this->midiIn     = new QRtMidiIn( NAME_MIDI_IN );
      this->midiIn->moveToThread( workerThread );
      this->midiIn->openPort( midiInPort );

      QObject::connect( this->midiIn,
                        &QRtMidiIn::midiReceived,
                        this,
                        &QRtMidiInOut::midiReceived );
      QObject::connect( this->midiIn,
                        &QRtMidiIn::messageReceived,
                        this,
                        &QRtMidiInOut::messageReceived );
    }
  } catch( RtMidiError& e ) {
    emit displayMessage( QStringLiteral( "Failed to open MIDI in port: %1" )
                           .arg( QString::fromStdString( e.getMessage() ) ) );
    this->midiIn->deleteLater();
    this->midiIn = nullptr;
  }
}

void QRtMidiInOut::sendMessage( MidiMsg& message ) {
  if( midiOut != nullptr ) {
    emit midiOut->sendMessage( message );
  }
}
