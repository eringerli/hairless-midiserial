#include "LuaMidiInOut.h"

#include "QMidi.h"
#include "Settings.h"

#include <QFileInfo>
#include <sol/sol.hpp>
#include <utility>

inline void my_panic( sol::optional< std::string > maybe_msg ) {
  std::cerr << "Lua is in a panic state and will now abort() the application"
            << std::endl;
  if( maybe_msg ) {
    const std::string& msg = maybe_msg.value();
    std::cerr << "\terror message: " << msg << std::endl;
  }
  // When this function exits, Lua will exhibit default behavior and abort()
}

LuaMidiInOut::LuaMidiInOut( const QString& name,
                            QThread*       workerThread,
                            QObject*       parent )
    : QObject( parent )
    , name( name ) {

  // Lua
  // open some common libraries
  lua.open_libraries( sol::lib::base,
                      sol::lib::package,
                      sol::lib::coroutine,
                      sol::lib::string,
                      sol::lib::os,
                      sol::lib::math,
                      sol::lib::table,
                      sol::lib::bit32,
                      sol::lib::io );

  lua.set_function( "sendMessageLeft",
                    &LuaMidiInOut::onMessageReceivedLeft,
                    this );
  lua.set_function( "sendMessageRight",
                    &LuaMidiInOut::onMessageReceivedRight,
                    this );
  lua.set_function( "setTimeout", &LuaMidiInOut::setTimeout, this );

  lua.set_panic( sol::c_call< decltype( &my_panic ), &my_panic > );

  timer.start( timeout );
  connect( &timer, &QTimer::timeout, this, &LuaMidiInOut::onTimer );
  connect( this,
           &LuaMidiInOut::setInterval,
           &timer,
           QOverload< int >::of( &QTimer::setInterval ) );

  this->moveToThread( workerThread );
  timer.moveToThread( workerThread );
}

LuaMidiInOut::~LuaMidiInOut() = default;

void LuaMidiInOut::openLuaFile( const QString& fileNameToOpen ) {
  auto fileInfo = QFileInfo( fileNameToOpen );
  if( fileInfo.exists() ) {
    fileName = fileNameToOpen;
    Settings::setLastLuaFile( fileName );

    displayMessage( QString( "Loaded Lua script: " ).append( fileName ) );
    lua.safe_script_file(
      fileName.toStdString(),
      [ this ]( lua_State*, sol::protected_function_result pfr ) {
        sol::error err = pfr;
        emit       displayMessage( QString( err.what() ) );
        return pfr;
      } );
  }
}

void LuaMidiInOut::sendMessageLeft( MidiMsg message ) {
  if( debugLeftOut != nullptr ) {
    debugLeftOut->showMessage( message );
  }
  sol::function f = lua[ "onMessageLeft" ];
  if( f.valid() ) {
    lua[ "dataLeft" ] = message;
    f( lua[ "dataLeft" ] );
  } else {
    onMessageReceivedRight( message );
  }
}

void LuaMidiInOut::sendMessageRight( MidiMsg message ) {
  if( debugRightOut != nullptr ) {
    debugRightOut->showMessage( message );
  }
  sol::function f = lua[ "onMessageRight" ];
  if( f.valid() ) {
    lua[ "dataRight" ] = message;
    f( lua[ "dataRight" ] );
  } else {
    onMessageReceivedLeft( message );
  }
}

void LuaMidiInOut::onMessageReceivedLeft( MidiMsg message ) {
  emit messageReceivedLeft( message );
}

void LuaMidiInOut::onMessageReceivedRight( MidiMsg message ) {
  emit messageReceivedRight( message );
}

void LuaMidiInOut::onTimer() {
  sol::function f = lua[ "onTimer" ];
  if( f.valid() ) {
    f();
  }
  emit setInterval( timeout );
}

void LuaMidiInOut::setTimeout( int msec ) {
  if( msec < 1 ) {
    msec = 1;
  }
  timeout = msec;
}

void LuaMidiInOut::enableDebug() {
  if( debugEnabled ) {
    if( debugLeftIn == nullptr ) {
      debugLeftIn = new QMidiDebug( "Lua: Left In " );
      connect( debugLeftIn,
               &QMidiDebug::debugMessage,
               this,
               &LuaMidiInOut::debugMessage );
      connect( this,
               &LuaMidiInOut::messageReceivedLeft,
               debugLeftIn,
               &QMidiDebug::showMessage );
    }
    if( debugLeftOut == nullptr ) {
      debugLeftOut = new QMidiDebug( "Lua: Left Out" );
      connect( debugLeftOut,
               &QMidiDebug::debugMessage,
               this,
               &LuaMidiInOut::debugMessage );
    }
    if( debugRightIn == nullptr ) {
      debugRightIn = new QMidiDebug( "Lua: Right In " );
      connect( debugRightIn,
               &QMidiDebug::debugMessage,
               this,
               &LuaMidiInOut::debugMessage );
      connect( this,
               &LuaMidiInOut::messageReceivedRight,
               debugLeftIn,
               &QMidiDebug::showMessage );
    }
    if( debugRightOut == nullptr ) {
      debugRightOut = new QMidiDebug( "Lua: Right Out" );
      connect( debugRightOut,
               &QMidiDebug::debugMessage,
               this,
               &LuaMidiInOut::debugMessage );
    }
  } else {
    if( debugLeftIn != nullptr ) {
      debugLeftIn->deleteLater();
      debugLeftIn = nullptr;
    }
    if( debugLeftOut != nullptr ) {
      debugLeftOut->deleteLater();
      debugLeftOut = nullptr;
    }
    if( debugRightIn != nullptr ) {
      debugRightIn->deleteLater();
      debugRightIn = nullptr;
    }
    if( debugRightOut != nullptr ) {
      debugRightOut->deleteLater();
      debugRightOut = nullptr;
    }
  }
}

void LuaMidiInOut::changeDebug( bool debug ) {
  auto settings = Settings( name );
  settings.setDebug( debug );
  debugEnabled = debug;
  enableDebug();
}
