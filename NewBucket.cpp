#include "NewBucket.h"

#include <QSpinBox>

#include "ui_NewBucket.h"
NewBucket::NewBucket(QWidget *parent) : QDialog(parent), ui(new Ui::NewBucket) {
  ui->setupUi(this);
  init();
  updateView(ui->weight->value());
}

NewBucket::~NewBucket() { delete ui; }

Structures::Bucket NewBucket::bucket() const {
  Structures::Bucket result;
  result.weight = static_cast<decltype(result.weight)>(ui->weight->value());
  result.description = ui->descText->toPlainText().toStdString();
  return result;
}

void NewBucket::init() {
  connect(ui->weight, QOverload<int>::of(&QSpinBox::valueChanged), this,
          &NewBucket::updateView);
}

void NewBucket::updateView(int weightValue) {
  if (weightValue <= 0)
    ui->buttonBox->setEnabled(false);
  else
    ui->buttonBox->setEnabled(true);
}
