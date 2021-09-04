#ifndef BRIDGEHEAD_H
#define BRIDGEHEAD_H

#include "MidiFrame.h"
#include "QMidiDebug.h"
#include "SerialFrame.h"

#include <QFrame>

namespace Ui {
  class BridgeHead;
}

class QThread;
class QHBoxLayout;

class BridgeHead : public QFrame {
  Q_OBJECT

public:
  explicit BridgeHead( const QString& name,
                       QThread*       workerThread,
                       QWidget*       parent = nullptr );
  ~BridgeHead();

signals:
  void messageReceived( MidiMsg message );

  void displayMessage( QString message );
  void debugMessage( QString message );

public slots:
  void sendMessage( MidiMsg message );

  void setFrameEnabled(bool);

  void onDisplayMessage( QString message );
  void onDebugMessage( QString message );

private slots:
  void on_cbDebug_stateChanged( int );

  void on_rbSerial_clicked();

  void on_rbMidi_clicked();

private:
  Ui::BridgeHead* ui = nullptr;
  QString         name;
  QThread*        workerThread = nullptr;
  bool frameEnabled = false;
  MidiFrame*      midiFrame    = nullptr;
  SerialFrame*    serialFrame  = nullptr;
  QHBoxLayout*    layout       = nullptr;

  QMidiDebug* debugIn  = nullptr;
  QMidiDebug* debugOut = nullptr;

  void onRadioButtonsClicked();
};

#endif // BRIDGEHEAD_H
