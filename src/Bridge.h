#ifndef BRIDGE_H
#define BRIDGE_H

/*
 * Bridge is the actual controller that bridges Serial <-> MIDI. Create it by passing the
 * the serial & MIDI ports to use (or NULL if that part is not used.)
 *
 * The Bridge manages its own internal state, and can be deleted when you stop bridging and/or
 * recreated with new settings.
 */

#include <QObject>
#include "RtMidi.h"
#include "QRtMidiIn.h"
#include "qextserialport/qextserialport.h"

class Bridge : public QObject
{
    Q_OBJECT
public:
    explicit Bridge(QObject *parent, QString *serialName, PortSettings *serialSettings, int *midiInPort, int *midiOutPort);

    // Destroying an existing Bridge will cleanup state & release all ports
    ~Bridge();

private:
    QString bridgeName();

signals:
    // Signals to push user status messages
    void displayMessage(QString message);
    void debugMessage(QString message);

    // Signals to trigger UI "blinkenlight" updates
    void midiReceived();
    void midiSent();
    void serialTraffic();
private slots:
    void onMidiIn(double timeStamp, QByteArray message);
    void onSerialAvailable();
private:
    QRtMidiIn *midiIn;
    int *midiInPort;
    RtMidiOut *midiOut;
    int *midiOutPort;
    QextSerialPort *serial;
};

#endif // BRIDGE_H