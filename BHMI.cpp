
#include "BHMI.h"

#include <BucketsModel.h>
#include <DriverSettings.h>
#include <NewBucket.h>
#include <PrintFirst.h>
#include <SerialHandler.h>
#include <tmpLambda.h>

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
#include <QListView>
#include <QMessageBox>
#include <QModelIndex>
#include <QScopedPointer>
#include <QString>
#include <QStringListModel>
#include <QTabWidget>
#include <QTableView>
#include <QVBoxLayout>
#include <QtMultimedia/QAbstractVideoFilter>
#include <cstdint>

#include "./ui_BHMI.h"

static constexpr auto TIMER_UPDATE_SEC = 1;

static QString toWriteOnSerial(QString const &rawData) {
  static QString const ba = "p(%1,%2)";
  return ba.arg(rawData.size()).arg(rawData);
}

BHMI::BHMI(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::BHMI),
      _mainListModel(new QStringListModel),
      _sensorSH(new SerialHandler),
      _cameraView(new QCameraViewfinder),
      _driverDialog(new DriverSettings),
      _printer(new PrintFirst) {
  ui->setupUi(this);
  _lastSelected = ui->Weighbridge;

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
  connect(ui->test_lic, &QPushButton::clicked, this, [this]() {
    //    this->centralWidget()->setStyleSheet("");
    QMessageBox::about(
        this, "Heydar&Ashkan application",
        "This application is demo and is not safe to use in environments!\n"
        "Contact us:\n"
        "- Heydar.Mahmoodi75@gmail.com\n"
        "- Ashkan.younesi216@gmail.com\n");
  });
  connect(ui->Weighbridge, &QPushButton::clicked, this,
          [this]() { setCamera(false); });

  connect(ui->realPrintBuckets, &QPushButton::clicked, this, [this]() {
    QString str;
    auto stringList = _mainListModel->stringList();
    for (auto const &item : stringList) {
      str.append(item).append("\n");
    }
    static QString const TOTALITY = "Bucket counts: %1 \nTotal Weights: %2\n";
    str.append("--------------------------\n")
        .append(TOTALITY.arg(ui->totalBucketsCount->text())
                    .arg(ui->sumBucketsWeight->text()));
    qDebug() << "write Byte size:" << _sensorSH->write(toWriteOnSerial(str));
  });

  connect(ui->goTop, &QPushButton::clicked, this, [this]() {
    auto tmp = ui->listView->currentIndex();
    if (tmp.row() > 0)
      ui->listView->setCurrentIndex(tmp.siblingAtRow(tmp.row() - 1));
  });
  connect(ui->goBottom, &QPushButton::clicked, this, [this]() {
    auto tmp = ui->listView->currentIndex();
    if (tmp.row() < _bucketsModel->rowCount() - 1)
      ui->listView->setCurrentIndex(tmp.siblingAtRow(tmp.row() + 1));
  });
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
    //    auto res = bucketAdder.exec();
    //    if (res)
    if (ui->addBtn->isChecked()) {
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
  _bucketsView = ui->listView;
  _bucketsModel.reset(new BucketsModel);
  {
    connect(_bucketsModel.get(), &BucketsModel::dataUpdated, this, [this]() {
      auto totalBuckets = _bucketsModel->buckets();
      QStringList result;
      for (int i = 0; i < totalBuckets.size(); ++i) {
        auto const &bucket = totalBuckets[i];
        auto weight = bucket.convert(Structures::WeightUnit::KiloGram);
        static QString const CURRENT_BUCKET_PF = "Bucket %1: \t %2 \tKiloGram ";
        result.append(CURRENT_BUCKET_PF.arg(i).arg(weight));
      }

      _mainListModel->setStringList(result);
    });
  }
  {
    ui->listView->setModel(_mainListModel.get());
    _bucketsView->setSelectionBehavior(QAbstractItemView::SelectRows);
  }
  {
    //    connect(ui->addBtn, &QPushButton::clicked, this,
    //    &BHMI::onAddBUcket);
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
    _bucketsModel->setCurrentUnit(Structures::KiloGram);
    //    connect(ui->weightUnitCmb,
    //            QOverload<int>::of(&QComboBox::currentIndexChanged), this,
    //            [this](int index) {
    //              switch (index) {
    //                case 0:
    //                  _bucketsModel->setCurrentUnit(Structures::Gram);
    //                  break;
    //                case 1:
    //                  _bucketsModel->setCurrentUnit(Structures::KiloGram);
    //                  break;
    //                case 2:
    //                  _bucketsModel->setCurrentUnit(Structures::Tonne);
    //                  break;
    //                default:
    //                  break;
    //              }
    //            });
  }
  {
    ui->cameraTurner->setVisible(true);
    connect(ui->cameraTurner, &QPushButton::clicked, this, [this]() {
      Structures::DataOverSerial data;
      data.cameraOn = 1;
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
  {
    if (data.newBucket && ui->addBtn->isChecked()) {
      NewBucket bucketAdder;
      bucketAdder.setBucket(calculateBucket(data.s1, data.s2));
      //    auto res = bucketAdder.exec();
      //    if (res)
      { _bucketsModel->addNewBucket(bucketAdder.bucket()); }
    }
  }
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
  if (nullptr == _holder.get()) _holder.reset(new Holder(this));
  connect(_sensorSH.data(), &SerialHandler::startPortClicked, this, [this]() {
    _sensorSH->close();
    _sensorSH->openSerialPort(_holder);
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
  ui->bucketsItemsGroup->addWidget(_cameraView.data());
}

void BHMI::initPrinter() {
  connect(ui->printBtn, &QPushButton::clicked, this, [this]() {
    auto driverInfoText = _driverDialog->info().stringify();
    _bucketsModel->addNewBucket({213213, "test1"});
    _bucketsModel->addNewBucket({65812, "test2"});
    _bucketsModel->addNewBucket({258238, "test3"});
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

static void hideChildren(QLayout *lay) {
  auto const count = lay->count();
  for (int i = 0; i < count; ++i) {
    auto l = lay->itemAt(i)->layout();
    auto w = lay->itemAt(i)->widget();
    if (nullptr != w) {
      w->hide();
    } else if (nullptr != l)
      hideChildren(l);
  }
}

static void showChildren(QLayout *lay) {
  auto const count = lay->count();
  for (int i = 0; i < count; ++i) {
    auto l = lay->itemAt(i)->layout();
    auto w = lay->itemAt(i)->widget();
    if (nullptr != w) {
      w->show();
    } else if (nullptr != l)
      showChildren(l);
  }
}

void BHMI::onCameraModeChanged(bool turnOn) {
  static QString const mastmaliWithBorder =
      "QPushButton{ "
      "border:5px solid #ff0000;"
      "border-color: rgb(0, 0, 0);"
      "%1"
      "}";
  auto const static normal =
      mastmaliWithBorder.arg("background-color: rgb(238, 238, 236);");
  auto const static selected =
      mastmaliWithBorder.arg("background-color:rgb(78, 154, 6);");

  if (turnOn) {
    _lastSelected->setStyleSheet(normal);
    _lastSelected = ui->cameraTurner;
    _lastSelected->setStyleSheet(selected);

    turnCameraOn();
    hideChildren(ui->bucketsItemsGroup);
    _cameraView->show();
  } else {
    _lastSelected->setStyleSheet(normal);
    _lastSelected = ui->Weighbridge;
    _lastSelected->setStyleSheet(selected);

    turnCameraOff();
    showChildren(ui->bucketsItemsGroup);
    _cameraView->hide();
  }
}
