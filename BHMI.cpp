#include "BHMI.h"

#include <BucketsModel.h>

#include <QDateTime>
#include <QHeaderView>
#include <QTableView>
#include <QtMultimedia/QAbstractVideoFilter>

#include "./ui_BHMI.h"

BHMI::BHMI(QWidget *parent) : QMainWindow(parent), ui(new Ui::BHMI) {
  ui->setupUi(this);
  initTimer();
  initBucketView();
}

BHMI::~BHMI() { delete ui; }

void BHMI::updateDateTime() {
  auto currentDateTime = QDateTime::currentDateTime();
  ui->timeLbl->setText(currentDateTime.time().toString());
  ui->dateLbl->setText(currentDateTime.date().toString("dd/MM/yyyy"));
  ui->dayLbl->setText(currentDateTime.toString("dddd"));
}

void BHMI::initBucketView() {
  _bucketsView = new QTableView(this);
  _bucketsModel.reset(new BucketsModel);

  ui->bucketParentLayout->insertWidget(1, _bucketsView.data(), 10);
  _bucketsView->setModel(_bucketsModel.get());
  //  _bucketsView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  //  _bucketsView->horizontalHeader()->setSectionResizeMode(
  //      1, QHeaderView::ResizeMode::ResizeToContents);
  _bucketsView->resizeColumnsToContents();
  _bucketsView->horizontalHeader()->setStretchLastSection(true);
  _bucketsView->horizontalHeader()->setStyleSheet(
      "background-color: rgb(196, 160, 0);");
}

void BHMI::initTimer() {
  updateDateTime();
  _timeUpdater.setInterval(1000);
  connect(&_timeUpdater, &QTimer::timeout, this, &BHMI::updateDateTime);
  _timeUpdater.start();
}
