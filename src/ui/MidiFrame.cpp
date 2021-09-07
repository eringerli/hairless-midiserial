#include "MidiFrame.h"

#include "../QRtMidiInOut.h"
#include "../Settings.h"
#include "ui_MidiFrame.h"

#include <QComboBox>
#include <QDebug>
#include <QEvent>
#include <QThread>
#include <utility>

MidiFrame::MidiFrame( const QString name,
                      QThread*      workerThread,
                      QWidget*      parent )
    : QFrame( parent )
    , ui( new Ui::MidiFrame )
    , name( name )
    , workerThread( workerThread ) {
  auto settings = Settings( name );

  ui->setupUi( this );

  ui->cmbMidiIn->installEventFilter( this );
  ui->cmbMidiOut->installEventFilter( this );

  connect( ui->cmbMidiIn,
           QOverload< int >::of( &QComboBox::activated ),
           this,
           &MidiFrame::onMidiValuesChanged );
  connect( ui->cmbMidiOut,
           QOverload< int >::of( &QComboBox::activated ),
           this,
           &MidiFrame::onMidiValuesChanged );

  RtMidiIn in = RtMidiIn();
  refreshMidi( ui->cmbMidiIn, &in, settings.getLastMidiIn() );

  RtMidiOut out = RtMidiOut();
  refreshMidi( ui->cmbMidiOut, &out, settings.getLastMidiOut() );
}

MidiFrame::~MidiFrame() {
  delete ui;
  delete midiInOut;
}

bool MidiFrame::eventFilter( QObject* object, QEvent* event ) {
  // If user is triggering a dropdown, refresh it live
  if( event->type() == QEvent::MouseButtonPress ||
      event->type() == QEvent::KeyPress ) {
    if( object == ui->cmbMidiIn ) {
      refreshMidiIn();
    } else if( object == ui->cmbMidiOut ) {
      refreshMidiOut();
    }
  }
  return false;
}

void MidiFrame::onMidiValuesChanged() {
  auto settings = Settings( name );
  if( ui->cbEnable->isChecked() ) {
    if( midiInOut != nullptr &&
        settings.getLastMidiIn() == ui->cmbMidiIn->currentText() &&
        settings.getLastMidiOut() == ui->cmbMidiOut->currentText() ) {
      return;
    }

    if( midiInOut != nullptr ) {
      midiInOut->deleteLater();
      QThread::yieldCurrentThread(); // Try and get any signals from the bridge
                                     // sent sooner not later
      QCoreApplication::processEvents();
      midiInOut = nullptr;
    }

    settings.setLastMidiIn( ui->cmbMidiIn->currentText() );
    settings.setLastMidiOut( ui->cmbMidiOut->currentText() );

    int midiIn  = ui->cmbMidiIn->currentIndex() - 1;
    int midiOut = ui->cmbMidiOut->currentIndex() - 1;

    midiInOut = new QRtMidiInOut( name );

    connect( midiInOut,
             &QRtMidiInOut::displayMessage,
             this,
             &MidiFrame::displayMessage );
    connect( midiInOut,
             &QRtMidiInOut::debugMessage,
             this,
             &MidiFrame::debugMessage );

    connect( midiInOut,
             &QRtMidiInOut::messageReceived,
             this,
             &MidiFrame::messageReceived );

    connect( midiInOut,
             &QRtMidiInOut::midiSent,
             ui->led_midiout,
             &BlinkenLight::blinkOn );
    connect( midiInOut,
             &QRtMidiInOut::midiReceived,
             ui->led_midiin,
             &BlinkenLight::blinkOn );

    midiInOut->attach( midiIn, midiOut, workerThread );
  } else {
    midiInOut->deleteLater();
    midiInOut = nullptr;
  }

  emit onEnabledChanged( ui->cbEnable->isChecked() );
}

void MidiFrame::sendMessage( MidiMsg& message ) {
  if( midiInOut != nullptr ) {
    midiInOut->sendMessage( message );
  }
}
void MidiFrame::refreshMidiIn() {
  RtMidiIn in = RtMidiIn();
  refreshMidi( ui->cmbMidiIn, &in, ui->cmbMidiIn->currentText() );
}

void MidiFrame::refreshMidiOut() {
  RtMidiOut out = RtMidiOut();
  refreshMidi( ui->cmbMidiOut, &out, ui->cmbMidiOut->currentText() );
}

void MidiFrame::refreshMidi( QComboBox*     combo,
                             RtMidi*        midi,
                             const QString& setToName ) {
  combo->clear();
  try {
    int ports = midi->getPortCount();
    combo->addItem( TEXT_NOT_CONNECTED );
    for( int i = 0; i < ports; i++ ) {
      QString name = QString::fromStdString( midi->getPortName( i ) );
      combo->addItem( name );
      if( name == setToName ) {
        combo->setCurrentIndex( combo->count() - 1 );
      }
    }
  } catch( RtMidiError& err ) {
    emit displayMessage( "Failed to scan for MIDI ports:" );
    emit displayMessage( QString::fromStdString( err.getMessage() ) );
  }
}

void MidiFrame::on_cbEnable_stateChanged( int ) {
  onMidiValuesChanged();
}
