#include "SerialHandler.h"

#include "ui_SerialHandler.h"

SerialHandler::SerialHandler(QWidget *parent)
    : QWidget(parent), ui(new Ui::SerialHandler) {
  ui->setupUi(this);
}

SerialHandler::~SerialHandler() { delete ui; }
