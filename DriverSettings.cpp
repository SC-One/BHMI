#include "DriverSettings.h"

#include <QDebug>
#include <QSettings>

#include "ui_DriverSettings.h"

static constexpr auto DRIVER_FULL_NAME = "DRIVER_FULL_NAME";
static constexpr auto DRIVER_CODE = "DRIVER_CODE";
static constexpr auto DRIVER_PHONE = "DRIVER_PHONE";
static constexpr auto EMPLOYER_FULL_NAME = "EMPLOYER_FULL_NAME";
static constexpr auto VEHICLE_CODE = "VEHICLE_CODE";

DriverSettings::DriverSettings(QWidget *parent)
    : QWidget(parent), ui(new Ui::DriverSettings), _settings(new QSettings) {
  ui->setupUi(this);

  load();
}

DriverSettings::~DriverSettings() {
  save();
  _settings->sync();  //
  delete ui;
}

void DriverSettings::load() {
  auto driverName = _settings->value(DRIVER_FULL_NAME, "");
  auto driverCode = _settings->value(DRIVER_CODE, "");
  auto driverPhone = _settings->value(DRIVER_PHONE, "");
  auto employerName = _settings->value(EMPLOYER_FULL_NAME, "");
  auto vehicleCode = _settings->value(VEHICLE_CODE, "");

  {
    ui->driverFullName->setText(driverName.toString());
    ui->driverCode->setText(driverCode.toString());
    ui->driverPhone->setText(driverPhone.toString());
    ui->employerFullName->setText(employerName.toString());
    ui->vehicleCode->setText(vehicleCode.toString());
  }
  qDebug() << _settings->status();
}

void DriverSettings::save() {
  _settings->setValue(DRIVER_FULL_NAME, ui->driverFullName->text());
  _settings->setValue(DRIVER_CODE, ui->driverCode->text());
  _settings->setValue(DRIVER_PHONE, ui->driverPhone->text());
  _settings->setValue(EMPLOYER_FULL_NAME, ui->employerFullName->text());
  _settings->setValue(VEHICLE_CODE, ui->vehicleCode->text());
  qDebug() << _settings->status();
}
