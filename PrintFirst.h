#ifndef PRINTFIRST_H
#define PRINTFIRST_H

#include <QObject>
#include <QScopedPointer>

QT_BEGIN_NAMESPACE
class QPrinter;
QT_END_NAMESPACE

class PrintFirst : public QObject {
  Q_OBJECT
 public:
  explicit PrintFirst(QObject *parent = nullptr);

  bool print(QString const &text);
 signals:

 public slots:

 private:
  QPrinter *_printer;
};

#endif  // PRINTFIRST_H
