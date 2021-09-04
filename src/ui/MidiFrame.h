#pragma once

#include "../QRtMidiInOut.h"
#include "commons.h"

#include <QFrame>

namespace Ui {
  class MidiFrame;
}

class MidiFrame : public QFrame {
  Q_OBJECT

public:
  explicit MidiFrame( QString  name,
                      QThread* workerThread,
                      QWidget* parent = nullptr );
  ~MidiFrame();

  bool eventFilter( QObject*, QEvent* );

signals:
  void midiSent();

  void midiReceived();
  void messageReceived( MidiMsg message );

  void displayMessage( QString message );
  void debugMessage( QString message );

public slots:
  void sendMessage( MidiMsg& message );

private slots:
  void onMidiValuesChanged();

private:
  void refreshMidiIn();
  void refreshMidiOut();
  void refreshMidi( QComboBox* combo, RtMidi* midi );

private:
  Ui::MidiFrame* ui        = nullptr;
  QRtMidiInOut*  midiInOut = nullptr;

  QString  name;
  QThread* workerThread = nullptr;
};
