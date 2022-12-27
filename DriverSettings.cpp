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

  connect(ui->driverFullName, &QLineEdit::textEdited, this,
          &DriverSettings::save);
  connect(ui->driverCode, &QLineEdit::textEdited, this, &DriverSettings::save);
  connect(ui->driverPhone, &QLineEdit::textEdited, this, &DriverSettings::save);
  connect(ui->employerFullName, &QLineEdit::textEdited, this,
          &DriverSettings::save);
  connect(ui->vehicleCode, &QLineEdit::textEdited, this, &DriverSettings::save);
  load();
}

DriverSettings::~DriverSettings() {
  save();
  _settings->sync();  //
  delete ui;
}

DriverSettings::Information DriverSettings::info() const {
  DriverSettings::Information result;
  result.driverName = ui->driverFullName->text();
  result.driverCode = ui->driverCode->text();
  result.driverPhone = ui->driverPhone->text();
  result.employerName = ui->employerFullName->text();
  result.vehicleCode = ui->vehicleCode->text();
  return result;
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
}

void DriverSettings::save() {
  auto infoResult = info();
  _settings->setValue(DRIVER_FULL_NAME, infoResult.driverName);
  _settings->setValue(DRIVER_CODE, infoResult.driverCode);
  _settings->setValue(DRIVER_PHONE, infoResult.driverPhone);
  _settings->setValue(EMPLOYER_FULL_NAME, infoResult.employerName);
  _settings->setValue(VEHICLE_CODE, infoResult.vehicleCode);
}
