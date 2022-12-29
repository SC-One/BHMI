#include "PrintFirst.h"

#include <QDebug>
#include <QPainter>
#include <QPrintDialog>
#include <QPrinter>

PrintFirst::PrintFirst(QObject *parent)
    : QObject(parent), _printer(new QPrinter) {
  if (_printer->isValid()) {
    qDebug() << "printer name:" << _printer->printerName()
             << "\t,state:" << _printer->printerState();
  } else {
    qDebug() << "Printer is not valid!";
  }
}
static bool showPrinterDialog(QPrinter *printer) {
  QScopedPointer<QPrintDialog> dialog(new QPrintDialog(printer));
  dialog->setWindowTitle("Print Document");
  return dialog->exec() == QDialog::Accepted;
}

bool PrintFirst::print(const QString &text) {
  if (!_printer->isValid()) return false;

  if (!showPrinterDialog(_printer)) return false;

  QPainter painter;
  if (painter.begin(_printer)) {
    painter.drawText(100, 100, 500, 500, Qt::AlignLeft | Qt::AlignTop, text);
    return painter.end();
  } else
    return false;
}
