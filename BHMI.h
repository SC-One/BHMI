#ifndef BHMI_H
#define BHMI_H

#include <QMainWindow>
#include <QTimer>

QT_BEGIN_NAMESPACE
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

 private:
  Ui::BHMI *ui;
  QTimer _timeUpdater;
};
#endif  // BHMI_H
