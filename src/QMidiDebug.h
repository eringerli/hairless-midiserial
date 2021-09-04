#ifndef QMIDIDEBUG_H
#define QMIDIDEBUG_H

#include "QMidi.h"

#include <QObject>

class QMidiDebug : public QObject {
  Q_OBJECT
public:
  explicit QMidiDebug( QString prefix, QObject* parent = nullptr );

signals:
  void debugMessage( QString message );

public slots:
  void showMessage( MidiMsg message );

private:
  QString prefix;
};

#endif // QMIDIDEBUG_H
