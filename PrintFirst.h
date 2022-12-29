#ifndef PRINTFIRST_H
#define PRINTFIRST_H

#include <QObject>

class PrintFirst : public QObject {
  Q_OBJECT
 public:
  explicit PrintFirst(QObject *parent = nullptr);

 signals:

 public slots:
};

#endif  // PRINTFIRST_H
