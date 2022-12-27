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

 public:
  void addNewBucket(Structures::Bucket const &bucket);
  void clear();
  QByteArray toCSV() const;

  Structures::WeightUnit currentUnit() const;
  void setCurrentUnit(const Structures::WeightUnit &currentUnit);

 private:
  Structures::WeightUnit _currentUnit;
  std::set<Structures::Bucket> _buckets;
};

#endif  // BUCKETS_MODEL_H
