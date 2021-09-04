#pragma once

#include <QComboBox>
#include <QString>

inline void selectIfAvailable( QComboBox* box, const QString& itemText ) {
  for( int i = 0; i < box->count(); i++ ) {
    if( box->itemText( i ) == itemText ) {
      box->setCurrentIndex( i );
      return;
    }
  }
}
