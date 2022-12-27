
#include "BHMI.h"

#include <BucketsModel.h>
#include <DriverSettings.h>
#include <NewBucket.h>
#include <SerialHandler.h>

#include <QCameraViewfinder>
#include <QComboBox>
#include <QDataStream>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QHeaderView>
#include <QMessageBox>
#include <QTableView>
#include <QtMultimedia/QAbstractVideoFilter>
#include <cstdint>

#include "./ui_BHMI.h"

BHMI::BHMI(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::BHMI),
      _sensorSH(new SerialHandler),
      _cameraView(new QCameraViewfinder) {
  ui->setupUi(this);
  initTimer();
  initBucketView();
  initSettings();
  initLoadManagement();
  {
    Structures::DataOverSerial data;
    data.cameraOn = 0;
    data.newBucket = 0;
    data.rawPump = 0;
    data.s2 = 0;
    data.s1 = 0;
    setLastData(data);
  }
  initCamera();
}

BHMI::~BHMI() { delete ui; }

void BHMI::updateDateTime() {
  auto currentDateTime = QDateTime::currentDateTime();
  ui->timeLbl->setText(currentDateTime.time().toString());  // "hh:mm" origin
  ui->dateLbl->setText(currentDateTime.date().toString("dd/MM/yyyy"));
  ui->dayLbl->setText(currentDateTime.toString("dddd"));
}
static float calculateBucket(float s1, float s2) {
  return 2435 * (s1 - s2) - 2800;
}
void BHMI::onAddBUcket() {
  auto lastData = getLastData();
  if (lastData.newBucket) {
    NewBucket bucketAdder;
    bucketAdder.setBucket(calculateBucket(lastData.s1, lastData.s2));
    auto res = bucketAdder.exec();
    if (res) {
      _bucketsModel->addNewBucket(bucketAdder.bucket());
    }
  }
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

void BHMI::onRemoveBucket() {
  auto selectedModel = _bucketsView->selectionModel();
  if (selectedModel->hasSelection()) {
    auto selectedRows = selectedModel->selectedRows();
    for (auto const &item : selectedRows) {
      _bucketsModel->removeBucket(item.row());
    }
  }
}

void BHMI::initBucketView() {
  _bucketsView = new QTableView(this);
  _bucketsModel.reset(new BucketsModel);
  {
    _bucketsView->setModel(_bucketsModel.get());
    ui->bucketParentLayout->insertWidget(1, _bucketsView.data(), 10);
    _bucketsView->horizontalHeader()->setSectionResizeMode(
        QHeaderView::Stretch);
    //  _bucketsView->horizontalHeader()->setSectionResizeMode(
    //      1, QHeaderView::ResizeMode::ResizeToContents);
    //    _bucketsView->horizontalHeader()->setStretchLastSection(true);
    //    _bucketsView->resizeColumnsToContents();
    _bucketsView->setSelectionBehavior(QAbstractItemView::SelectRows);

    //    connect(_bucketsView.data(), &QTableView::doubleClicked, this,
    //            [this](QModelIndex const &index) {
    //              _bucketsModel->removeBucket(index.row());
    //            });
    _bucketsView->horizontalHeader()->setStyleSheet(
        "background-color: rgb(196, 160, 0);");
  }
  {
    connect(ui->addBtn, &QPushButton::clicked, this, &BHMI::onAddBUcket);
    connect(ui->saveBtn, &QPushButton::clicked, this, &BHMI::onSaveBuckets);
    connect(ui->rmvBtn, &QPushButton::clicked, this, &BHMI::onRemoveBucket);
    connect(_bucketsModel.get(), &BucketsModel::dataUpdated, this, [this]() {
      auto totalBuckets = _bucketsModel->buckets();
      std::uint_fast32_t sum = 0;
      for (auto const &item : totalBuckets) {
        sum += item.weight;
      }
      Structures::Bucket tempTotalBuckets;
      tempTotalBuckets.weight = sum;
      ui->sumBucketsWeight->setText(
          QString::number(
              tempTotalBuckets.convert(_bucketsModel->currentUnit())) +
          " " + Structures::wUnittoString(_bucketsModel->currentUnit()));
      ui->totalBucketsCount->setText(QString::number(totalBuckets.size()));
    });
    connect(ui->weightUnitCmb,
            QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            [this](int index) {
              switch (index) {
                case 0:
                  _bucketsModel->setCurrentUnit(Structures::Gram);
                  break;
                case 1:
                  _bucketsModel->setCurrentUnit(Structures::KiloGram);
                  break;
                case 2:
                  _bucketsModel->setCurrentUnit(Structures::Tonne);
                  break;
                default:
                  break;
              }
            });
  }
  {
    ui->cameraTurner->setVisible(false);
    connect(this, &BHMI::cameraModeChanged, this, &BHMI::onCameraModeChanged);
  }
}

void BHMI::initTimer() {
  updateDateTime();
  _timeUpdater.setInterval(1000);
  connect(&_timeUpdater, &QTimer::timeout, this, &BHMI::updateDateTime);
  _timeUpdater.start();
}

void BHMI::updateDataOverSerial(Structures::DataOverSerial const &data) {
  ui->sensor1->setValue(25 * (data.s1 - 4));
  ui->sensor2->setValue(25 * (data.s2 - 4));
  setCamera(data.cameraOn);
  ui->addBtn->setEnabled(data.newBucket);
}

void BHMI::initSettings() {
  connect(this, &BHMI::lastDataChanged, this,
          [this]() { updateDataOverSerial(getLastData()); });
  connect(ui->settingsBtn, &QPushButton::clicked, this,
          [this]() { _sensorSH->show(); });
  connect(_sensorSH.get(), &SerialHandler::startPortClicked, this, [this]() {
    _sensorSH->close();
    _sensorSH->openSerialPort([this](QByteArray const &ba) {
      QDataStream streammer(ba);
      Structures::DataOverSerial data;
      float newBucketTemp;
      float cameraOnTemp;

      streammer >> data.s1 >> data.s2 >> data.rawPump >> newBucketTemp >>
          cameraOnTemp;
      data.newBucket = newBucketTemp;
      data.cameraOn = cameraOnTemp;
      setLastData(data);
      qDebug() << ba;
    });
  });
}

void BHMI::initLoadManagement() {
  connect(ui->loadManagementBtn, &QPushButton::clicked, this, []() {
    static auto driverDialog = new DriverSettings();
    driverDialog->setWindowModality(Qt::WindowModality::ApplicationModal);
    driverDialog->setWindowFlag(Qt::Dialog);
    driverDialog->show();
  });
}

// void BHMI::initSensorsNetwork() {}

void BHMI::turnCameraOn() { _cameraDriver.start(); }

void BHMI::turnCameraOff() { _cameraDriver.stop(); }

void BHMI::initCamera() {
  new QCameraViewfinder(this);
  _cameraDriver.setCameraRenderer(_cameraView.get());
}

Structures::DataOverSerial BHMI::getLastData() const { return _lastData; }

void BHMI::setLastData(const Structures::DataOverSerial &lastData) {
  _lastData = lastData;
  emit lastDataChanged();
}

bool BHMI::getCamera() const { return _cameraMode; }

void BHMI::setCamera(bool value) {
  _cameraMode = value;
  emit cameraModeChanged(value);
}

void BHMI::onCameraModeChanged(bool turnOn) {
  if (turnOn) {
    turnCameraOn();  // TODO: you should take data from serial camera and
                     // show it
    ui->totalSumFrame->hide();
    _bucketsView->hide();
    ui->backCameraPic->show();
  } else {
    turnCameraOff();
    ui->totalSumFrame->show();
    _bucketsView->show();
    ui->backCameraPic->hide();
  }
}
