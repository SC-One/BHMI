#include "NewBucket.h"

#include "ui_NewBucket.h"

NewBucket::NewBucket(QWidget *parent) : QDialog(parent), ui(new Ui::NewBucket) {
  ui->setupUi(this);
}

NewBucket::~NewBucket() { delete ui; }

NewBucket::Info NewBucket::data() const {
  NewBucket::Info result;
  result.weight = ui->weight->value();
  result.description = ui->descText->toPlainText();
  return result;
}
