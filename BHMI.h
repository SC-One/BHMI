#ifndef BHMI_H
#define BHMI_H

#include <QMainWindow>
#include <QPointer>
#include <QTimer>

class BucketsModel;
QT_BEGIN_NAMESPACE
class QTableView;

namespace Ui {
class BHMI;
}
QT_END_NAMESPACE

class BHMI : public QMainWindow {
  Q_OBJECT

 public:
  BHMI(QWidget *parent = nullptr);
  ~BHMI();

 signals:
  void cameraModeChanged(bool turnOn);

 private slots:
  void updateDateTime();
  void onAddBUcket();
  void onSaveBuckets();
  void onRemoveBucket();

  bool getCamera() const;
  void setCamera(bool value);

 private:
  // helper functions
  void initBucketView();
  void initTimer();

  void turnCameraOn();

 private:
  Ui::BHMI *ui;
  QTimer _timeUpdater;

  QPointer<QTableView> _bucketsView;
  QScopedPointer<BucketsModel> _bucketsModel;

  bool _cameraMode;
};
#endif  // BHMI_H
