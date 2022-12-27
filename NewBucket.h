#ifndef NEWBUCKET_H
#define NEWBUCKET_H

#include <QDialog>

namespace Ui {
class NewBucket;
}

class NewBucket : public QDialog {
  Q_OBJECT

 public:
  explicit NewBucket(QWidget *parent = nullptr);
  ~NewBucket();

 private:
  Ui::NewBucket *ui;
};

#endif  // NEWBUCKET_H
