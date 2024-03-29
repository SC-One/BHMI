#ifndef BUCKETS_MODEL_H
#define BUCKETS_MODEL_H

#include <Structures.h>

#include <QAbstractTableModel>
#include <QSet>
#include <QVector>
#include <set>

class BucketsModel : public QAbstractTableModel {
  Q_OBJECT
 public:
  explicit BucketsModel(QObject *parent = nullptr);

  /// override abstract
  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;
  QVariant data(const QModelIndex &index,
                int role = Qt::DisplayRole) const override;
  QVariant headerData(int section, Qt::Orientation orientation,
                      int role = Qt::DisplayRole) const override;

 signals:
  void dataUpdated();

 public:
  void addNewBucket(Structures::Bucket const &bucket);
  bool removeBucket(int index = 0);
  void clear();
  QByteArray toCSV() const;

  Structures::WeightUnit currentUnit() const;
  void setCurrentUnit(const Structures::WeightUnit &currentUnit);

  QVector<Structures::Bucket> buckets() const;
  void setBuckets(const QVector<Structures::Bucket> &buckets);

 private:
  Structures::WeightUnit _currentUnit;
  QVector<Structures::Bucket> _buckets;
};

#endif  // BUCKETS_MODEL_H
