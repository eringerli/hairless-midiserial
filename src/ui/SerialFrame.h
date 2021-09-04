#pragma once

#include "../QSerialMidiInOut.h"
#include "commons.h"

#include <QFrame>

namespace Ui {
  class SerialFrame;
}

class SerialFrame : public QFrame {
  Q_OBJECT

public:
  explicit SerialFrame( QString  name,
                        QThread* workerThread,
                        QWidget* parent = nullptr );
  ~SerialFrame();

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
  void onSerialValuesChanged();
  void on_pbSettings_clicked();

private:
  Ui::SerialFrame*  ui;
  QSerialMidiInOut* midiInOut = nullptr;

  QString  name;
  QThread* workerThread;

  void refreshSerial();
};
