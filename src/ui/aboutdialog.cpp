#include "aboutdialog.h"

#include "ui_aboutdialog.h"

AboutDialog::AboutDialog( QWidget* parent )
    : QDialog( parent )
    , ui( new Ui::About ) {
  ui->setupUi( this );

  QString text = ui->label_info->text()
                   .arg( QString::fromUtf8( VERSION ) )
                   .arg( QString::fromUtf8( __DATE__ ) );
  ui->label_info->setText( text );
}

AboutDialog::~AboutDialog() {
  delete ui;
}
