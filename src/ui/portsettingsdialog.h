#pragma once

#include "../Settings.h"

#include <QtWidgets/QDialog>

namespace Ui {
  class SettingsDialog;
}

class QComboBox;

class PortSettingsDialog : public QDialog {
  Q_OBJECT

public:
  explicit PortSettingsDialog( QString name, QWidget* parent = 0 );
  ~PortSettingsDialog();

private:
  Ui::SettingsDialog* ui;
  Settings            settings;

public slots:
  virtual void accept();
private slots:
  void resetSerialPort();

private:
  void selectByData( QComboBox* box, int dataValue );

  void populateBaudRate( QComboBox* box, BaudRateType initialValue );
  void populateDataBits( QComboBox* box, DataBitsType initialValue );
  void populateParity( QComboBox* box, ParityType initialValue );
  void populateStopBits( QComboBox* box, StopBitsType initialValue );
  void populateFlowControl( QComboBox* box, FlowType initialValue );
};
