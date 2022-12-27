#include "BHMI.h"

#include <BucketsModel.h>
#include <NewBucket.h>

#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QHeaderView>
#include <QMessageBox>
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

void BHMI::onAddBUcket() {
  NewBucket bucketAdder;
  auto res = bucketAdder.exec();
  qDebug() << res;
}

void BHMI::onSaveBuckets() {
  QString fileName = QFileDialog::getSaveFileName(
      this, tr("Save Report"), QDir::homePath() + "/report.csv",
      tr("CSV (*.csv)"));
  if (!fileName.isEmpty() && '/' != fileName.at(fileName.size() - 5)) {
    QFile file(fileName);
    if (file.open(QFile::OpenModeFlag::WriteOnly)) {
      QTextStream out(&file);
      out << _bucketsModel->toCSV();
      file.close();
      {
        QMessageBox msgBox;
        static QString const FILE_SAVED_MSG("Report saved: %1");
        msgBox.setText(FILE_SAVED_MSG.arg(fileName));
        msgBox.exec();
      }
      return;
    } else {
      qDebug() << file.errorString();
      return;
    }
  } else {
    // cancel or empty name
    qDebug() << "File name was empty!";
  }
}

void BHMI::initBucketView() {
  _bucketsView = new QTableView(this);
  _bucketsModel.reset(new BucketsModel);
  {
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
  {
    connect(ui->addBtn, &QPushButton::clicked, this, &BHMI::onAddBUcket);
    connect(ui->saveBtn, &QPushButton::clicked, this, &BHMI::onSaveBuckets);
  }
}

void BHMI::initTimer() {
  updateDateTime();
  _timeUpdater.setInterval(1000);
  connect(&_timeUpdater, &QTimer::timeout, this, &BHMI::updateDateTime);
  _timeUpdater.start();
}
