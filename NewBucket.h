#ifndef NEWBUCKET_H
#define NEWBUCKET_H

#include <QDialog>
#include <QString>

namespace Ui {
class NewBucket;
}

class NewBucket : public QDialog {
  Q_OBJECT

 public:
  explicit NewBucket(QWidget *parent = nullptr);
  ~NewBucket();

  struct Info {
    QString description;
    int weight;
  };

  Info data() const;

 private:
  Ui::NewBucket *ui;
};

#endif  // NEWBUCKET_H
