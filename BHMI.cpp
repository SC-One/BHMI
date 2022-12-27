#include "BHMI.h"

#include <QDateTime>

#include "./ui_BHMI.h"

BHMI::BHMI(QWidget *parent) : QMainWindow(parent), ui(new Ui::BHMI) {
  ui->setupUi(this);
  updateDateTime();
  _timeUpdater.setInterval(1000);
  connect(&_timeUpdater, &QTimer::timeout, this, &BHMI::updateDateTime);
  _timeUpdater.start();
}

BHMI::~BHMI() { delete ui; }

void BHMI::updateDateTime() {
  auto currentDateTime = QDateTime::currentDateTime();
  ui->timeLbl->setText(currentDateTime.time().toString());
  ui->dateLbl->setText(currentDateTime.date().toString());
  ui->dayLbl->setText(currentDateTime.toString("dddd"));
}
