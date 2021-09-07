#include "QMidiDebug.h"

#include <utility>

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

QMidiDebug::QMidiDebug( const QString& prefix, QObject* parent )
    : QObject( parent )
    , prefix( prefix ) {}

void QMidiDebug::showMessage( MidiMsg buf ) {

  uint8_t     msg     = buf[ 0 ];
  uint8_t     tag     = msg & TAG_MASK;
  uint8_t     channel = msg & CHANNEL_MASK;
  const char* desc    = nullptr;

  auto debugString = QString( prefix );
  debugString.append( ": " );

  for( const uint8_t& b: buf ) {
    if( b <= 0xF ) {
      debugString.append( '0' );
    }
    debugString.append( QString::number( b, 16 ) );
  }
  debugString.append( " " );

  // Work out what we have
  switch( tag ) {
    case TAG_PROGRAM:
      desc = "Ch %1: Program change %2";
      break;
    case TAG_CHANNEL_PRESSURE:
      desc = "Ch %1: Pressure change %2";
      break;
    case TAG_NOTE_ON:
      desc = "Ch %1: Note %2 on  velocity %3";
      break;
    case TAG_NOTE_OFF:
      desc = "Ch %1: Note %2 off velocity %3";
      break;
    case TAG_KEY_PRESSURE:
      desc = "Ch %1: Note %2 pressure %3";
      break;
    case TAG_CONTROLLER:
      desc = "Ch %1: Controller %2 value %3";
      break;
    case TAG_PITCH_BEND:
      desc = "Ch %1: Pitch bend %2";
      break;
    case TAG_SPECIAL:
      if( msg == MSG_SYSEX_START ) {
        debugString.append( "SysEx Message: " );
        for( const auto& val: buf ) {
          if( val == MSG_SYSEX_END ) {
            break;
          }
          debugString.append( QStringLiteral( "0x%1 " ).arg( val, 0, 16 ) );
        }
        emit debugMessage( debugString );
        return;
      }
      if( channel < 3 ) {
        desc = "System Message #%1: %2 %3";
      } else if( channel < 6 ) {
        desc = "System Message #%1: %2";
      } else {
        desc = "System Message #%1";
      }
      break;
    default:
      debugString.append(
        QStringLiteral( "Unknown MIDI message %1" )
          .arg( QString(
            QByteArray( ( const char* )buf.data(), buf.size() ).toHex() ) ) );
      emit debugMessage( debugString );
      return;
  }

  // Format the output

  debugString.append( desc );
  debugString =
    debugString.arg( ( int )channel + ( tag == TAG_SPECIAL ? 0 : 1 ) );

  if( tag == TAG_PITCH_BEND &&
      buf.size() == 3 ) { // recombine LSB/MSB for pitch bend
    debugString = debugString.arg( ( uint16_t )( ( uint8_t )buf[ 1 ] ) |
                                   ( ( uint8_t )buf[ 2 ] ) << 7 );
    emit debugMessage( debugString );
    return;
  }

  int i = 1, end = buf.size();
  while( debugString.contains( "%" ) && i < end ) {
    debugString = debugString.arg( ( int )buf[ i++ ] );
  }

  emit debugMessage( debugString );
}
