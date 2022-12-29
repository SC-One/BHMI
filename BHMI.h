#ifndef BHMI_H
#define BHMI_H

#include <CameraDriver.h>
#include <Structures.h>

#include <QDateTime>
#include <QMainWindow>
#include <QPointer>
#include <QTimer>

class BucketsModel;
class SerialHandler;

QT_BEGIN_NAMESPACE
class QTableView;
class QCameraViewfinder;
class QDateTimeEdit;
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

  void updateDataOverSerial(const Structures::DataOverSerial &data);

 private:
  Ui::BHMI *ui;
  QTimer _timeUpdater;

  QPointer<QTableView> _bucketsView;
  QScopedPointer<BucketsModel> _bucketsModel;

  bool _cameraMode;

  QScopedPointer<SerialHandler> _sensorSH;  // sensor serial handler

  Structures::DataOverSerial _lastData;

  QScopedPointer<QCameraViewfinder> _cameraView;
  CameraDriver _cameraDriver;
  QDateTime _dateTimeShow;
};
#endif  // BHMI_H
