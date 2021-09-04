#pragma once

#include "QMidiDebug.h"
#include "QRtMidiIn.h"
#include "QRtMidiOut.h"

#include <QObject>
#include <sol/sol.hpp>

class LuaMidiInOut : public QObject {
  Q_OBJECT
public:
  explicit LuaMidiInOut( const QString& name,
                         QThread*       workerThread,
                         QObject*       parent = nullptr );
  ~LuaMidiInOut();

  void openLuaFile( const QString& fileName );

signals:
  void messageReceivedLeft( MidiMsg message );
  void messageReceivedRight( MidiMsg message );

  void displayMessage( QString message );
  void debugMessage( QString message );

public slots:
  void sendMessageLeft( MidiMsg message );
  void sendMessageRight( MidiMsg message );

  void onMessageReceivedLeft( MidiMsg message );
  void onMessageReceivedRight( MidiMsg message );

  void changeDebug( bool );

private:
  sol::state lua;
  QString    name;
  QString    fileName;

  bool        debugEnabled  = false;
  QMidiDebug* debugLeftIn   = nullptr;
  QMidiDebug* debugLeftOut  = nullptr;
  QMidiDebug* debugRightIn  = nullptr;
  QMidiDebug* debugRightOut = nullptr;

  void enableDebug();
};
