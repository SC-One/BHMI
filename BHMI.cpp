#include "BHMI.h"
#include "./ui_BHMI.h"

BHMI::BHMI(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::BHMI)
{
    ui->setupUi(this);
}

BHMI::~BHMI()
{
    delete ui;
}

