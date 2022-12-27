#include "NewBucket.h"

#include "ui_NewBucket.h"

NewBucket::NewBucket(QWidget *parent) : QDialog(parent), ui(new Ui::NewBucket) {
  ui->setupUi(this);
}

NewBucket::~NewBucket() { delete ui; }
