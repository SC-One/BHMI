#include "ApplicationTime.h"

#include "ui_ApplicationTime.h"

ApplicationTime::ApplicationTime(QWidget *parent)
    : QWidget(parent), ui(new Ui::ApplicationTime) {
  ui->setupUi(this);
  _timer.setInterval(1000);
}

ApplicationTime::~ApplicationTime() { delete ui; }
