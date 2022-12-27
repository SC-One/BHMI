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

  void load();
  void save();

 private:
  Ui::DriverSettings *ui;
  QScopedPointer<QSettings> _settings;
};

#endif  // DRIVERSETTINGS_H
