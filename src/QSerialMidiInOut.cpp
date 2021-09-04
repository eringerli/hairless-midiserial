#include "QSerialMidiInOut.h"

#include "qextserialenumerator.h"

#include <QIODevice>
#include <QList>
#include <QVector>
#include <cstdint>

// MIDI message masks
constexpr uint8_t STATUS_MASK  = 0x80;
constexpr uint8_t CHANNEL_MASK = 0x0F;
constexpr uint8_t TAG_MASK     = 0xF0;

constexpr uint8_t TAG_NOTE_OFF         = 0x80;
constexpr uint8_t TAG_NOTE_ON          = 0x90;
constexpr uint8_t TAG_KEY_PRESSURE     = 0xA0;
constexpr uint8_t TAG_CONTROLLER       = 0xB0;
constexpr uint8_t TAG_PROGRAM          = 0xC0;
constexpr uint8_t TAG_CHANNEL_PRESSURE = 0xD0;
constexpr uint8_t TAG_PITCH_BEND       = 0xE0;
constexpr uint8_t TAG_SPECIAL          = 0xF0;

constexpr uint8_t MSG_SYSEX_START = 0xF0;
constexpr uint8_t MSG_SYSEX_END   = 0xF7;

constexpr uint8_t MSG_DEBUG =
  0xFF; // special ttymidi "debug output" MIDI message tag

constexpr uint8_t UNKNOWN_MIDI = -2;

inline bool is_voice_msg( uint8_t tag ) {
  return tag >= 0x80 && tag <= 0xEF;
};
inline bool is_syscommon_msg( uint8_t tag ) {
  return tag >= 0xF0 && tag <= 0xF7;
};
inline bool is_realtime_msg( uint8_t tag ) {
  return tag >= 0xF8 && tag < 0xFF;
};

// return the number of data bytes for a given message status byte
inline static int get_data_length( uint8_t status ) {
  switch( status & TAG_MASK ) {
    case TAG_PROGRAM:
    case TAG_CHANNEL_PRESSURE:
      return 1;
    case TAG_NOTE_ON:
    case TAG_NOTE_OFF:
    case TAG_KEY_PRESSURE:
    case TAG_CONTROLLER:
    case TAG_PITCH_BEND:
      return 2;
    case TAG_SPECIAL: {
      if( status == MSG_DEBUG ) {
        return 3; // Debug messages go { 0xFF, 0, 0, <len>, Msg } so first we
                  // read 3 data bytes to get full length
      }
      if( status == MSG_SYSEX_START ) {
        return 65535; // SysEx messages go until we see a SysExEnd
      }

      uint8_t channel = status & CHANNEL_MASK;
      if( channel < 3 ) {
        return 2;
      }
      if( channel < 6 ) {
        return 1;
      }
      return 0;
    }
    default:
      // Unknown midi message...?
      return UNKNOWN_MIDI;
  }
}

QSerialMidiInOut::QSerialMidiInOut( QObject* parent )
    : QObject( parent )
    , attachTime( QTime::currentTime() ) {}

QSerialMidiInOut::~QSerialMidiInOut() {
  emit displayMessage( applyTimeStamp( "Closing Serial port..." ) );
  if( this->latency != nullptr ) {
    this->latency->resetLatency();
  }
  delete this->serial;
}

void QSerialMidiInOut::attach( const QString& serialName,
                               PortSettings   serialSettings,
                               QThread*       workerThread ) {
  this->moveToThread( workerThread );

  if( !serialName.isEmpty() && serialName != TEXT_NOT_CONNECTED ) {
    // Latency fixups
    latency = new PortLatency( serialName );
    connect( latency,
             &PortLatency::debugMessage,
             this,
             &QSerialMidiInOut::debugMessage );
    connect( latency,
             &PortLatency::errorMessage,
             this,
             &QSerialMidiInOut::displayMessage );
    latency->fixLatency();

    emit displayMessage(
      QStringLiteral( "Opening serial port '%1'..." ).arg( serialName ) );
    this->serial = new QextSerialPort( serialName, serialSettings );
    connect( this->serial,
             &QextSerialPort::readyRead,
             this,
             &QSerialMidiInOut::onSerialAvailable );
    this->serial->open( QIODevice::ReadWrite | QIODevice::Unbuffered );
    attachTime = QTime::currentTime();
    this->serial->moveToThread( workerThread );
  }
}

void QSerialMidiInOut::sendMessage( MidiMsg message ) {
  if( ( serial != nullptr ) && serial->isOpen() ) {
    serial->write(
      QByteArray( ( const char* )message.data(), message.size() ) );
    emit midiSent();
  }
}

void QSerialMidiInOut::onSerialAvailable() {
  emit       serialTraffic();
  QByteArray data = this->serial->readAll();
  foreach( uint8_t next, data ) {
    if( ( next & STATUS_MASK ) != 0 ) {
      this->onStatusByte( next );
    } else {
      this->onDataByte( next );
    }
    if( this->data_expected == 0 ) {
      sendMidiMessage();
    }
  }
}

void QSerialMidiInOut::onStatusByte( uint8_t byte ) {
  if( byte == MSG_SYSEX_END && bufferStartsWith( MSG_SYSEX_START ) ) {
    this->msg_data.append( byte ); // bookends of a complete SysEx message
    sendMidiMessage();
    return;
  }

  if( this->data_expected > 0 ) {
    emit displayMessage( applyTimeStamp(
      QString(
        "Warning: got a status byte when we were expecting %1 more data bytes, sending possibly incomplete MIDI message 0x%2" )
        .arg( this->data_expected )
        .arg( ( uint8_t )this->msg_data[ 0 ], 0, 16 ) ) );
    sendMidiMessage();
  }

  if( is_voice_msg( byte ) ) {
    this->running_status = byte;
  }
  if( is_syscommon_msg( byte ) ) {
    this->running_status = 0;
  }

  this->data_expected = get_data_length( byte );
  if( this->data_expected == UNKNOWN_MIDI ) {
    emit displayMessage(
      applyTimeStamp( QStringLiteral( "Warning: got unexpected status byte %1" )
                        .arg( byte, 0, 16 ) ) );
    this->data_expected = 0;
  }
  this->msg_data.clear();
  this->msg_data.append( byte );
}

void QSerialMidiInOut::onDataByte( uint8_t byte ) {
  if( this->data_expected == 0 && this->running_status != 0 ) {
    onStatusByte( this->running_status );
  }
  if( this->data_expected == 0 ) { // checking again just in case running
                                   // status failed to update us to expect data
    emit displayMessage(
      applyTimeStamp( QStringLiteral( "Error: got unexpected data byte 0x%1." )
                        .arg( byte, 0, 16 ) ) );
    return;
  }

  this->msg_data.append( byte );
  this->data_expected--;

  if( bufferStartsWith( MSG_DEBUG ) && this->data_expected == 0 &&
      this->msg_data.length() ==
        4 ) { // we've read the length of the debug message
    this->data_expected += this->msg_data[ 3 ]; // add the message length
  }
}

void QSerialMidiInOut::sendMidiMessage() {
  if( msg_data.length() == 0 ) {
    return;
  }
  if( bufferStartsWith( MSG_DEBUG ) ) {
    QString debug_msg =
      QString::fromUtf8( msg_data.mid( 4, msg_data[ 3 ] ).data() );
    emit displayMessage(
      applyTimeStamp( QStringLiteral( "Serial Says: %1" ).arg( debug_msg ) ) );
  } else {
    auto message = std::vector< uint8_t >( msg_data.begin(), msg_data.end() );
    emit messageReceived( message );
    emit midiReceived();
  }
  msg_data.clear();
  data_expected = 0;
}

QString QSerialMidiInOut::applyTimeStamp( const QString& message ) {
  auto msecs = this->attachTime.msecsTo( QTime::currentTime() ) / 1000.0;
  return QStringLiteral( "+%1 - %2" ).arg( msecs, 4 ).arg( message );
}

bool QSerialMidiInOut::bufferStartsWith( uint8_t byte ) {
  return !this->msg_data.isEmpty() && ( uint8_t )msg_data[ 0 ] == byte;
}
