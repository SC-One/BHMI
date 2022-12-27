#ifndef SerialHandler_H
#define SerialHandler_H

#include <QWidget>

namespace Ui {
class SerialHandler;
}

class SerialHandler : public QWidget {
  Q_OBJECT

 public:
  explicit SerialHandler(QWidget *parent = nullptr);
  ~SerialHandler();

 private:
  Ui::SerialHandler *ui;
};

#endif  // SerialHandler_H
