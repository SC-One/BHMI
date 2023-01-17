#ifndef SerialHandler_H
#define SerialHandler_H

#include <QByteArray>
#include <QGroupBox>
#include <QSerialPort>
#include <QWidget>
#include <functional>
#include <memory>

class Holder;
class QIntValidator;
class QSerialPort;
namespace Ui {
class SerialHandler;
}

class SerialHandler : public QWidget {
  Q_OBJECT

 public:
  explicit SerialHandler(QWidget *parent = nullptr);
  ~SerialHandler();

 public:
  void openSerialPort(std::shared_ptr<Holder> holder);

  void closeSerialPort();

 signals:
  void startPortClicked();

 private:
  struct Settings {
    QString name;
    qint32 baudRate;
    QString stringBaudRate;
    QSerialPort::DataBits dataBits;
    QString stringDataBits;
    QSerialPort::Parity parity;
    QString stringParity;
    QSerialPort::StopBits stopBits;
    QString stringStopBits;
    QSerialPort::FlowControl flowControl;
    QString stringFlowControl;
    bool localEchoEnabled;
  };

  Settings settings() const;

 private slots:
  void showPortInfo(int idx);
  void apply();
  void checkCustomBaudRatePolicy(int idx);
  void checkCustomDevicePathPolicy(int idx);

 private:
  void fillPortsParameters();
  void fillPortsInfo();
  void updateSettings();

 private:
  Ui::SerialHandler *ui;
  Settings m_currentSettings;
  QIntValidator *m_intValidator = nullptr;

  QScopedPointer<QSerialPort> _serialPort;
};

#endif  // SerialHandler_H
