#include <QApplication>

#include "BHMI.h"

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  QApplication::setOrganizationName("Telegram: @adreup");
  QApplication::setOrganizationDomain("HCoding.ir");
  QApplication::setApplicationName("BHMI by Heydar and Ashkan");

  BHMI w;
  w.show();
  return a.exec();
}
