#include "SerialHandler.h"

#include <QDebug>
#include <QIntValidator>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QSettings>

#include "ui_SerialHandler.h"

static constexpr char blankString[] =
    QT_TRANSLATE_NOOP("SettingsDialog_SerialHandler", "N/A");

SerialHandler::SerialHandler(QWidget *parent)
    : QWidget(parent), ui(new Ui::SerialHandler), _serialPort(new QSerialPort) {
  ui->setupUi(this);

  //  {
  //    ui->parametersBox->hide();
  //    ui->selectBox->hide();
  //  }
  connect(ui->startReadBtn, &QPushButton::clicked, this,
          &SerialHandler::startPortClicked);

  ui->baudRateBox->setInsertPolicy(QComboBox::NoInsert);

  connect(ui->serialPortInfoListBox,
          QOverload<int>::of(&QComboBox::currentIndexChanged), this,
          &SerialHandler::showPortInfo);
  connect(ui->baudRateBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
          this, &SerialHandler::checkCustomBaudRatePolicy);
  connect(ui->serialPortInfoListBox,
          QOverload<int>::of(&QComboBox::currentIndexChanged), this,
          &SerialHandler::checkCustomDevicePathPolicy);

  fillPortsParameters();
  fillPortsInfo();

  updateSettings();
}

SerialHandler::~SerialHandler() { delete ui; }

void SerialHandler::openSerialPort(
    std::function<void(QByteArray const &ba)> const &onReadCB) {
  closeSerialPort();
  const SerialHandler::Settings p = settings();
  _serialPort->setPortName(p.name);
  _serialPort->setBaudRate(p.baudRate);
  _serialPort->setDataBits(p.dataBits);
  _serialPort->setParity(p.parity);
  _serialPort->setStopBits(p.stopBits);
  _serialPort->setFlowControl(p.flowControl);
  if (_serialPort->open(QIODevice::ReadWrite)) {
    //      m_console->setEnabled(true);
    //      m_console->setLocalEchoEnabled(p.localEchoEnabled);
    //      m_ui->actionConnect->setEnabled(false);
    //      m_ui->actionDisconnect->setEnabled(true);
    //      m_ui->actionConfigure->setEnabled(false);
    QString status = QString::fromLatin1("Connected to %1 : %2, %3, %4, %5, %6")
                         .arg(p.name)
                         .arg(p.stringBaudRate)
                         .arg(p.stringDataBits)
                         .arg(p.stringParity)
                         .arg(p.stringStopBits)
                         .arg(p.stringFlowControl);
    qDebug() << status;
    connect(_serialPort.get(), &QSerialPort::readyRead, this,
            [&]() { onReadCB(_serialPort->readAll()); });
  } else {
    QMessageBox::critical(this, tr("Error"), _serialPort->errorString());
    qDebug() << "Open error! " << _serialPort->errorString();
  }
}

void SerialHandler::closeSerialPort() {
  if (_serialPort->isOpen()) {
    _serialPort->close();
    qDebug() << "Disconnected";
  }
  disconnect(_serialPort.get(), &QSerialPort::readyRead, nullptr, nullptr);
  //    m_console->setEnabled(false);
  //    m_ui->actionConnect->setEnabled(true);
  //    m_ui->actionDisconnect->setEnabled(false);
  //    m_ui->actionConfigure->setEnabled(true);
}

SerialHandler::Settings SerialHandler::settings() const {
  return m_currentSettings;
}

void SerialHandler::showPortInfo(int idx) {
  if (idx == -1) return;

  const QStringList list =
      ui->serialPortInfoListBox->itemData(idx).toStringList();
  ui->descriptionLabel->setText(
      tr("Description: %1")
          .arg(list.count() > 1 ? list.at(1) : tr(blankString)));
  ui->manufacturerLabel->setText(
      tr("Manufacturer: %1")
          .arg(list.count() > 2 ? list.at(2) : tr(blankString)));
  ui->serialNumberLabel->setText(
      tr("Serial number: %1")
          .arg(list.count() > 3 ? list.at(3) : tr(blankString)));
  ui->locationLabel->setText(
      tr("Location: %1").arg(list.count() > 4 ? list.at(4) : tr(blankString)));
  ui->vidLabel->setText(
      tr("Vendor Identifier: %1")
          .arg(list.count() > 5 ? list.at(5) : tr(blankString)));
  ui->pidLabel->setText(
      tr("Product Identifier: %1")
          .arg(list.count() > 6 ? list.at(6) : tr(blankString)));
}

void SerialHandler::apply() { updateSettings(); }

void SerialHandler::checkCustomBaudRatePolicy(int idx) {
  const bool isCustomBaudRate = !ui->baudRateBox->itemData(idx).isValid();
  ui->baudRateBox->setEditable(isCustomBaudRate);
  if (isCustomBaudRate) {
    ui->baudRateBox->clearEditText();
    QLineEdit *edit = ui->baudRateBox->lineEdit();
    edit->setValidator(m_intValidator);
  }
}

void SerialHandler::checkCustomDevicePathPolicy(int idx) {
  const bool isCustomPath = !ui->serialPortInfoListBox->itemData(idx).isValid();
  ui->serialPortInfoListBox->setEditable(isCustomPath);
  if (isCustomPath) ui->serialPortInfoListBox->clearEditText();
}

void SerialHandler::fillPortsParameters() {
  ui->baudRateBox->addItem(QStringLiteral("9600"), QSerialPort::Baud9600);
  ui->baudRateBox->addItem(QStringLiteral("19200"), QSerialPort::Baud19200);
  ui->baudRateBox->addItem(QStringLiteral("38400"), QSerialPort::Baud38400);
  ui->baudRateBox->addItem(QStringLiteral("115200"), QSerialPort::Baud115200);
  ui->baudRateBox->addItem(tr("Custom"));

  ui->dataBitsBox->addItem(QStringLiteral("5"), QSerialPort::Data5);
  ui->dataBitsBox->addItem(QStringLiteral("6"), QSerialPort::Data6);
  ui->dataBitsBox->addItem(QStringLiteral("7"), QSerialPort::Data7);
  ui->dataBitsBox->addItem(QStringLiteral("8"), QSerialPort::Data8);
  ui->dataBitsBox->setCurrentIndex(3);

  ui->parityBox->addItem(tr("None"), QSerialPort::NoParity);
  ui->parityBox->addItem(tr("Even"), QSerialPort::EvenParity);
  ui->parityBox->addItem(tr("Odd"), QSerialPort::OddParity);
  ui->parityBox->addItem(tr("Mark"), QSerialPort::MarkParity);
  ui->parityBox->addItem(tr("Space"), QSerialPort::SpaceParity);

  ui->stopBitsBox->addItem(QStringLiteral("1"), QSerialPort::OneStop);
#ifdef Q_OS_WIN
  ui->stopBitsBox->addItem(tr("1.5"), QSerialPort::OneAndHalfStop);
#endif
  ui->stopBitsBox->addItem(QStringLiteral("2"), QSerialPort::TwoStop);

  ui->flowControlBox->addItem(tr("None"), QSerialPort::NoFlowControl);
  ui->flowControlBox->addItem(tr("RTS/CTS"), QSerialPort::HardwareControl);
  ui->flowControlBox->addItem(tr("XON/XOFF"), QSerialPort::SoftwareControl);
}

void SerialHandler::fillPortsInfo() {
  ui->serialPortInfoListBox->clear();
  QString description;
  QString manufacturer;
  QString serialNumber;
  const auto infos = QSerialPortInfo::availablePorts();
  for (const QSerialPortInfo &info : infos) {
    QStringList list;
    description = info.description();
    manufacturer = info.manufacturer();
    serialNumber = info.serialNumber();
    list << info.portName()
         << (!description.isEmpty() ? description : blankString)
         << (!manufacturer.isEmpty() ? manufacturer : blankString)
         << (!serialNumber.isEmpty() ? serialNumber : blankString)
         << info.systemLocation()
         << (info.vendorIdentifier()
                 ? QString::number(info.vendorIdentifier(), 16)
                 : blankString)
         << (info.productIdentifier()
                 ? QString::number(info.productIdentifier(), 16)
                 : blankString);

    ui->serialPortInfoListBox->addItem(list.first(), list);
  }

  ui->serialPortInfoListBox->addItem(tr("Custom"));
}

void SerialHandler::updateSettings() {
  m_currentSettings.name = ui->serialPortInfoListBox->currentText();

  if (ui->baudRateBox->currentIndex() == 4) {
    m_currentSettings.baudRate = ui->baudRateBox->currentText().toInt();
  } else {
    m_currentSettings.baudRate = static_cast<QSerialPort::BaudRate>(
        ui->baudRateBox->itemData(ui->baudRateBox->currentIndex()).toInt());
  }
  m_currentSettings.stringBaudRate =
      QString::number(m_currentSettings.baudRate);

  m_currentSettings.dataBits = static_cast<QSerialPort::DataBits>(
      ui->dataBitsBox->itemData(ui->dataBitsBox->currentIndex()).toInt());
  m_currentSettings.stringDataBits = ui->dataBitsBox->currentText();

  m_currentSettings.parity = static_cast<QSerialPort::Parity>(
      ui->parityBox->itemData(ui->parityBox->currentIndex()).toInt());
  m_currentSettings.stringParity = ui->parityBox->currentText();

  m_currentSettings.stopBits = static_cast<QSerialPort::StopBits>(
      ui->stopBitsBox->itemData(ui->stopBitsBox->currentIndex()).toInt());
  m_currentSettings.stringStopBits = ui->stopBitsBox->currentText();

  m_currentSettings.flowControl = static_cast<QSerialPort::FlowControl>(
      ui->flowControlBox->itemData(ui->flowControlBox->currentIndex()).toInt());
  m_currentSettings.stringFlowControl = ui->flowControlBox->currentText();
}
