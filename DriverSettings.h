#ifndef DRIVERSETTINGS_H
#define DRIVERSETTINGS_H

#include <QScopedPointer>
#include <QWidget>

class QSettings;
namespace Ui {
class DriverSettings;
}

class DriverSettings : public QWidget {
  Q_OBJECT

 public:
  explicit DriverSettings(QWidget *parent = nullptr);
  ~DriverSettings();

  struct Information {
    QString driverName;
    QString driverCode;
    QString driverPhone;
    QString employerName;
    QString vehicleCode;
    QString stringify() const {
      static const QString result(
          "Driver Name: %1 , \tDriverCode: %2\n"
          "Driver Phone: %3,\n"
          "Employer Name: %4,\n"
          "Vehicle Code: %5.\n");
      return result.arg(driverName)
          .arg(driverCode)
          .arg(driverPhone)
          .arg(employerName)
          .arg(vehicleCode);
    }
  };

  Information info() const;

 private slots:
  void load();
  void save();

 private:
  Ui::DriverSettings *ui;
  QScopedPointer<QSettings> _settings;
};

#endif  // DRIVERSETTINGS_H
