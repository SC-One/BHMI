#ifndef BHMI_H
#define BHMI_H

#include <CameraDriver.h>
#include <Structures.h>

#include <QDateTime>
#include <QMainWindow>
#include <QPointer>
#include <QTimer>
#include <memory>

class BucketsModel;
class SerialHandler;
class DriverSettings;
class PrintFirst;
class Holder;
QT_BEGIN_NAMESPACE
class QTableView;
class QListView;
class QCameraViewfinder;
class QDateTimeEdit;
class QStringListModel;
class QPushButton;
namespace Ui {
class BHMI;
}
QT_END_NAMESPACE

class BHMI : public QMainWindow {
  Q_OBJECT

 public:
  BHMI(QWidget *parent = nullptr);
  ~BHMI();

  Structures::DataOverSerial getLastData() const;
  void setLastData(const Structures::DataOverSerial &lastData);

 signals:
  void cameraModeChanged(bool turnOn);
  void lastDataChanged();

 private slots:
  void updateDateTime();
  void onAddBUcket();
  void onSaveBuckets();
  void onRemoveBucket();

  bool getCamera() const;
  void setCamera(bool value);

  void onCameraModeChanged(bool turnOn);

 private:
  // helper functions
  void initBucketView();
  void initTimer();
  void initSettings();
  void initLoadManagement();
  //  void initSensorsNetwork();
  void turnCameraOn();
  void turnCameraOff();
  void initCamera();
  void initPrinter();

  void updateDataOverSerial(const Structures::DataOverSerial &data);

 private:
  Ui::BHMI *ui;
  QTimer _timeUpdater;

  QPointer<QListView> _bucketsView;
  QScopedPointer<BucketsModel> _bucketsModel;
  QScopedPointer<QStringListModel> _mainListModel;

  QPushButton *_lastSelected = nullptr;
  bool _cameraMode;

  QScopedPointer<SerialHandler> _sensorSH;  // sensor serial handler

  Structures::DataOverSerial _lastData;

  QScopedPointer<QCameraViewfinder> _cameraView;
  CameraDriver _cameraDriver;
  QDateTime _dateTimeShow;
  QScopedPointer<DriverSettings> _driverDialog;
  QScopedPointer<PrintFirst> _printer;

  std::shared_ptr<Holder> _holder;
};
#endif  // BHMI_H
