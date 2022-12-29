
#include "BHMI.h"

#include <BucketsModel.h>
#include <DriverSettings.h>
#include <NewBucket.h>
#include <PrintFirst.h>
#include <SerialHandler.h>

#include <QCameraViewfinder>
#include <QComboBox>
#include <QDataStream>
#include <QDateTime>
#include <QDateTimeEdit>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QHeaderView>
#include <QMessageBox>
#include <QScopedPointer>
#include <QTabWidget>
#include <QTableView>
#include <QVBoxLayout>
#include <QtMultimedia/QAbstractVideoFilter>
#include <cstdint>

#include "./ui_BHMI.h"

static constexpr auto TIMER_UPDATE_SEC = 1;
BHMI::BHMI(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::BHMI),
      _sensorSH(new SerialHandler),
      _cameraView(new QCameraViewfinder),
      _driverDialog(new DriverSettings),
      _printer(new PrintFirst) {
  ui->setupUi(this);

  _dateTimeShow = QDateTime::currentDateTime();
  initTimer();
  initBucketView();
  initSettings();
  initLoadManagement();
  initCamera();
  {
    Structures::DataOverSerial data;
    data.cameraOn = 0;
    data.newBucket = 0;
    data.rawPump = 0;
    data.s2 = 0;
    data.s1 = 0;
    setLastData(data);
  }
  initPrinter();
}

BHMI::~BHMI() { delete ui; }

void BHMI::updateDateTime() {
  { _dateTimeShow = _dateTimeShow.addSecs(TIMER_UPDATE_SEC); }
  auto const &currentDateTime = _dateTimeShow;
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
    ui->cameraTurner->setVisible(true);
    connect(ui->cameraTurner, &QPushButton::clicked, this, [this]() {
      Structures::DataOverSerial data;
      data.cameraOn = !getLastData().cameraOn;
      data.newBucket = 0;
      data.rawPump = 0;
      data.s2 = 0;
      data.s1 = 0;
      setLastData(data);
    });
    connect(this, &BHMI::cameraModeChanged, this, &BHMI::onCameraModeChanged);
  }
}

void BHMI::initTimer() {
  updateDateTime();
  _timeUpdater.setInterval(TIMER_UPDATE_SEC * 1e3);
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
  connect(ui->settingsBtn, &QPushButton::clicked, this, [this]() {
    static auto tempSettings = new QDialog();
    static auto *dateEdit = new QDateTimeEdit(_dateTimeShow);
    static auto lay = new QVBoxLayout();
    static bool inited = false;

    static auto xTimer = new QTimer;
    xTimer->setInterval(TIMER_UPDATE_SEC * 1e3);
    xTimer->start();
    connect(xTimer, &QTimer::timeout, this,
            [this]() { dateEdit->setDateTime(_dateTimeShow); });
    connect(
        dateEdit, &QDateTimeEdit::dateTimeChanged, this,
        [this](QDateTime const &dateTime) { this->_dateTimeShow = dateTime; });

    connect(this, &BHMI::destroyed, [&]() {
      tempSettings->deleteLater();
      dateEdit->deleteLater();
      lay->deleteLater();
    });

    if (!inited) {
      dateEdit->setMaximumHeight(37);
      auto static tabWidget = new QTabWidget;
      tempSettings->setLayout(lay);
      lay->addWidget(tabWidget);
      tabWidget->addTab(dateEdit, "DateTime");
      tabWidget->addTab(_sensorSH.get(), "SerialSettings");
      //      lay->addWidget(dateEdit);
      //      lay->addWidget(_sensorSH.get());
      inited = true;
    }
    tempSettings->exec();
  });
  connect(_sensorSH.data(), &SerialHandler::startPortClicked, this, [this]() {
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
  connect(ui->loadManagementBtn, &QPushButton::clicked, this, [this]() {
    _driverDialog->setWindowModality(Qt::WindowModality::ApplicationModal);
    _driverDialog->setWindowFlag(Qt::Dialog);
    _driverDialog->setGeometry(this->x() + 10, this->y() + 10, 400, 250);
    _driverDialog->show();
  });
}

// void BHMI::initSensorsNetwork() {}

void BHMI::turnCameraOn() { _cameraDriver.start(); }

void BHMI::turnCameraOff() { _cameraDriver.stop(); }

void BHMI::initCamera() {
  _cameraView.reset(new QCameraViewfinder());
  _cameraDriver.setCameraRenderer(_cameraView.get());
  ui->bucketParentLayout->insertWidget(1, _cameraView.data());
}

void BHMI::initPrinter() {
  connect(ui->printBtn, &QPushButton::clicked, this, [this]() {
    auto driverInfoText = _driverDialog->info().stringify();
    //    _bucketsModel->addNewBucket({213213, "test1"});
    //    _bucketsModel->addNewBucket({0, "test2"});
    //    _bucketsModel->addNewBucket({213213213213213213, "test3"});
    auto buckets = _bucketsModel->toCSV();
    _printer->print("@@@@@@Info@@@@@@\n" + driverInfoText +
                    "\n@@@@@@Bucket@@@@@@\n" + buckets);
  });
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
    _cameraView->show();
  } else {
    turnCameraOff();
    ui->totalSumFrame->show();
    _bucketsView->show();
    _cameraView->hide();
  }
}
