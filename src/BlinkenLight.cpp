#include "BlinkenLight.h"

#include <QPixmapCache>

constexpr int DefaultBlinkTime = 75;

BlinkenLight::BlinkenLight( QWidget* parent )
    : QLabel( parent )
    , on( false ) {
  timer.setSingleShot( true );
  timer.setInterval( DefaultBlinkTime );
  connect( &timer, SIGNAL( timeout() ), this, SLOT( blinkOff() ) );
}

static QPixmap getCachedPixmap( const QString& name ) {
  QPixmap res;
  if( !QPixmapCache::find( name, &res ) ) {
    res = QPixmap::fromImage( QImage( name ) );
    QPixmapCache::insert( name, res );
  }
  return res;
}

void BlinkenLight::blinkOn() {
  if( on ) {
    return;
  }
  timer.start();
  this->setPixmap( getCachedPixmap( ":/images/images/led-on.png" ) );
  on = true;
}

void BlinkenLight::blinkOff() {
  if( !on ) {
    return;
  }
  this->setPixmap( getCachedPixmap( ":/images/images/led-off.png" ) );
  on = false;
}
