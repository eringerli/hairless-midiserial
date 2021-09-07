#include "BridgeHead.h"

#include "../Settings.h"
#include "ui_BridgeHead.h"

BridgeHead::BridgeHead( const QString& name,
                        QThread*       workerThread,
                        QWidget*       parent )
    : QFrame( parent )
    , ui( new Ui::BridgeHead )
    , name( name )
    , workerThread( workerThread ) {
  ui->setupUi( this );

  auto* settings = new Settings( name );
  ui->cbDebug->setChecked( settings->getDebug() );

  layout = new QHBoxLayout( ui->frame );

  if( settings->getLastType() == ui->rbMidi->text() ) {
    ui->rbMidi->setChecked( true );
  } else {
    ui->rbSerial->setChecked( true );
  }

  onRadioButtonsClicked();
  on_cbDebug_stateChanged( 0 );
}

BridgeHead::~BridgeHead() {
  delete ui;
}

void BridgeHead::sendMessage( MidiMsg message ) {
  if( midiFrame != nullptr ) {
    if( debugOut != nullptr && frameEnabled ) {
      debugOut->showMessage( message );
    }
    midiFrame->sendMessage( message );
  }
  if( serialFrame != nullptr ) {
    if( debugOut != nullptr && frameEnabled ) {
      debugOut->showMessage( message );
    }
    serialFrame->sendMessage( message );
  }
}

void BridgeHead::setFrameEnabled( bool enabled ) {
  frameEnabled = enabled;
}

void BridgeHead::onDisplayMessage( QString message ) {
  emit displayMessage( QString( name ).append( ": " ).append( message ) );
}

void BridgeHead::onDebugMessage( QString message ) {
  emit debugMessage( QString( name ).append( ": " ).append( message ) );
}

void BridgeHead::on_cbDebug_stateChanged( int ) {
  auto settings = Settings( name );
  settings.setDebug( ui->cbDebug->isChecked() );

  if( ui->cbDebug->isChecked() ) {
    if( debugIn == nullptr ) {
      debugIn = new QMidiDebug( "In" );
      connect( debugIn,
               &QMidiDebug::debugMessage,
               this,
               &BridgeHead::onDebugMessage );
    }
    if( debugOut == nullptr ) {
      debugOut = new QMidiDebug( "Out" );
      connect( debugOut,
               &QMidiDebug::debugMessage,
               this,
               &BridgeHead::onDebugMessage );
    }

    if( midiFrame != nullptr ) {
      connect( midiFrame,
               &MidiFrame::messageReceived,
               debugIn,
               &QMidiDebug::showMessage,
               Qt::UniqueConnection );
    }
    if( serialFrame != nullptr ) {
      connect( serialFrame,
               &SerialFrame::messageReceived,
               debugIn,
               &QMidiDebug::showMessage,
               Qt::UniqueConnection );
    }
  } else {
    if( debugIn != nullptr ) {
      debugIn->deleteLater();
      debugIn = nullptr;
    }
    if( debugOut != nullptr ) {
      debugOut->deleteLater();
      debugOut = nullptr;
    }
  }
}

void BridgeHead::on_rbSerial_clicked() {
  onRadioButtonsClicked();
}

void BridgeHead::on_rbMidi_clicked() {
  onRadioButtonsClicked();
}

void BridgeHead::onRadioButtonsClicked() {
  auto* settings = new Settings( name );
  if( ui->rbMidi->isChecked() ) {
    settings->setLastType( ui->rbMidi->text() );
    if( midiFrame == nullptr ) {
      if( serialFrame != nullptr ) {
        serialFrame->deleteLater();
        serialFrame = nullptr;
      }

      midiFrame = new MidiFrame( name, workerThread );
      layout->addWidget( midiFrame );

      connect( midiFrame,
               &MidiFrame::messageReceived,
               this,
               &BridgeHead::messageReceived );
      connect( midiFrame,
               &MidiFrame::displayMessage,
               this,
               &BridgeHead::onDisplayMessage );
      connect( midiFrame,
               &MidiFrame::debugMessage,
               this,
               &BridgeHead::onDebugMessage );
      connect( midiFrame,
               &MidiFrame::onEnabledChanged,
               this,
               &BridgeHead::setFrameEnabled );
    }
  } else {
    settings->setLastType( ui->rbSerial->text() );

    if( serialFrame == nullptr ) {
      if( midiFrame != nullptr ) {
        midiFrame->deleteLater();
        midiFrame = nullptr;
      }
      serialFrame = new SerialFrame( name, workerThread );
      layout->addWidget( serialFrame );

      connect( serialFrame,
               &SerialFrame::messageReceived,
               this,
               &BridgeHead::messageReceived );
      connect( serialFrame,
               &SerialFrame::displayMessage,
               this,
               &BridgeHead::onDisplayMessage );
      connect( serialFrame,
               &SerialFrame::debugMessage,
               this,
               &BridgeHead::onDebugMessage );
      connect( serialFrame,
               &SerialFrame::onEnabledChanged,
               this,
               &BridgeHead::setFrameEnabled );
    }
  }
  on_cbDebug_stateChanged( 0 );
}
