#ifndef NEWBUCKET_H
#define NEWBUCKET_H

#include <Structures.h>

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

  Structures::Bucket bucket() const;

 private:
  void init();
 private slots:
  void updateView(int weightValue);

 private:
  Ui::NewBucket *ui;
};

#endif  // NEWBUCKET_H
