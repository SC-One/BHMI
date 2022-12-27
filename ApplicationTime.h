#ifndef APPLICATIONTIME_H
#define APPLICATIONTIME_H

#include <QTimer>
#include <QWidget>

namespace Ui {
class ApplicationTime;
}

class ApplicationTime : public QWidget {
  Q_OBJECT

 public:
  explicit ApplicationTime(QWidget *parent = nullptr);
  ~ApplicationTime();

 private:
  Ui::ApplicationTime *ui;
  QTimer _timer;
};

#endif  // APPLICATIONTIME_H
