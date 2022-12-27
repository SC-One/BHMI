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

 private slots:
  void updateDateTime();
  void onAddBUcket();
  void onSaveBuckets();
  void onRemoveBucket();

 private:
  // helper functions
  void initBucketView();
  void initTimer();

 private:
  Ui::BHMI *ui;
  QTimer _timeUpdater;

  QPointer<QTableView> _bucketsView;
  QScopedPointer<BucketsModel> _bucketsModel;
};
#endif  // BHMI_H
