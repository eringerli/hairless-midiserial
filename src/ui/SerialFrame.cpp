#include "SerialFrame.h"

#include "../Settings.h"
#include "portsettingsdialog.h"
#include "qextserialenumerator.h"
#include "ui_SerialFrame.h"

#include <QDebug>
#include <QEvent>
#include <QThread>
#include <utility>

SerialFrame::SerialFrame( QString name, QThread* workerThread, QWidget* parent )
    : QFrame( parent )
    , ui( new Ui::SerialFrame )
    , name( std::move( std::move( name ) ) )
    , workerThread( workerThread ) {
  ui->setupUi( this );

  ui->cmbSerial->installEventFilter( this );

  connect( ui->cmbSerial,
           QOverload< int >::of( &QComboBox::activated ),
           this,
           &SerialFrame::onSerialValuesChanged );

  refreshSerial();
}

SerialFrame::~SerialFrame() {
  delete ui;
}

bool SerialFrame::eventFilter( QObject* object, QEvent* event ) {
  // If user is triggering a dropdown, refresh it live
  if( event->type() == QEvent::MouseButtonPress ||
      event->type() == QEvent::KeyPress ) {
    if( object == ui->cmbSerial ) {
      refreshSerial();
    }
  }
  return false;
}

void SerialFrame::sendMessage( MidiMsg& message ) {
  if( midiInOut != nullptr ) {
    midiInOut->sendMessage( message );
  }
}

void SerialFrame::onSerialValuesChanged() {
  auto settings = Settings( name );

  if( midiInOut != nullptr &&
      settings.getLastSerialPort() == ui->cmbSerial->currentText() ) {
    qDebug() << "!!! delete midiInOut";
    return;
  }

  if( midiInOut != nullptr ) {
    midiInOut->deleteLater();
    QThread::yieldCurrentThread(); // Try and get any signals from the bridge
                                   // sent sooner not later
    QCoreApplication::processEvents();
    midiInOut = nullptr;
  }

  settings.setLastSerialPort( ui->cmbSerial->currentText() );

  midiInOut = new QSerialMidiInOut();

  connect( midiInOut,
           &QSerialMidiInOut::displayMessage,
           this,
           &SerialFrame::displayMessage );
  connect( midiInOut,
           &QSerialMidiInOut::debugMessage,
           this,
           &SerialFrame::debugMessage );

  connect( midiInOut,
           &QSerialMidiInOut::messageReceived,
           this,
           &SerialFrame::messageReceived );

  connect( midiInOut,
           &QSerialMidiInOut::midiSent,
           ui->led_serial,
           &BlinkenLight::blinkOn );
  connect( midiInOut,
           &QSerialMidiInOut::midiReceived,
           ui->led_serial,
           &BlinkenLight::blinkOn );

  midiInOut->attach( settings.getLastSerialPort(),
                     settings.getPortSettings(),
                     workerThread );
}

void SerialFrame::on_pbSettings_clicked() {
  auto* portSettingsDialog = new PortSettingsDialog( name );
  portSettingsDialog->show();
}

void SerialFrame::refreshSerial() {
  QString current = ui->cmbSerial->currentText();
  ui->cmbSerial->clear();
  ui->cmbSerial->addItem( TEXT_NOT_CONNECTED );
  QList< QextPortInfo > ports = QextSerialEnumerator::getPorts();
  for( auto& port: ports ) {
    QString label = port.friendName.isEmpty() ? port.portName : port.friendName;
#ifdef Q_OS_LINUX
    QString portName = port.physName; // Bug workaround, Linux needs the /dev/
                                      // in front of port name
#else
    QString portName = it->portName;
#endif
    ui->cmbSerial->addItem( label, QVariant( portName ) );
    if( current == label ) {
      ui->cmbSerial->setCurrentIndex( ui->cmbSerial->count() - 1 );
    }
  }
}
