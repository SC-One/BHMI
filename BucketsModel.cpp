#include "BucketsModel.h"

#include <cassert>

static constexpr int COLUMNS = 3;  // 2 is better, now just I'm filling

BucketsModel::BucketsModel(QObject *parent)
    : QAbstractTableModel(parent),
      _currentUnit(Structures::WeightUnit::KiloGram) {}

int BucketsModel::rowCount(const QModelIndex & /*parent*/) const {
  return static_cast<int>(_buckets.size());
}

int BucketsModel::columnCount(const QModelIndex & /*parent*/) const {
  return COLUMNS;
}

QVariant BucketsModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid()) return QVariant();
  if (role == Qt::DisplayRole) {
    switch (index.column()) {
      case 0:  // weight
      {
        if (index.row() < 0) return "QError(1) , invalid index";
        if (index.row() >= static_cast<int>(_buckets.size()))
          return "QError(1) , out of range";
        return QString::number(_buckets[index.row()].convert(currentUnit()));
      }
      case 1:  // weight unit
        return Structures::wUnittoString(currentUnit());
      case 2:  // weight unit
      {
        if (index.row() < 0) return "QError(2) , invalid index";
        if (index.row() >= static_cast<int>(_buckets.size()))
          return "QError(2) , out of range";
        return QString::fromStdString(_buckets[index.row()].description);
      }
      default:
        assert("Please report to the developer.");
        return "Assert!";
    }
  }
  return QString("QError , Make valid return data!");
}

QVariant BucketsModel::headerData(int section,
                                  [[maybe_unused]] Qt::Orientation orientation,
                                  [[maybe_unused]] int role) const {
  if (role != Qt::DisplayRole) return QVariant();

  if (orientation == Qt::Horizontal) {
    switch (section) {
      case 0:
        return "Weight";
      case 1:
        return "(Unit)";
      case 2:
        return "description";
      default:
        assert(
            "Please make sure that never come in this section!, report to the "
            "developer");
        return "Assert!";
    }
  } else
    return QStringLiteral("# %1").arg(section);
}
void BucketsModel::addNewBucket(Structures::Bucket const &bucket) {
  beginResetModel();
  _buckets << bucket;
  endResetModel();
  emit dataUpdated();
}

bool BucketsModel::removeBucket(int index) {
  if (index >= static_cast<int>(_buckets.size()) || index < 0) return false;
  beginResetModel();
  _buckets.remove(index);
  endResetModel();
  emit dataUpdated();
  return true;
}

void BucketsModel::clear() {
  beginResetModel();
  _buckets.clear();
  endResetModel();
  emit dataUpdated();
}

QByteArray BucketsModel::toCSV() const {
  QByteArray result;
  result.append("Weight,Description\n");
  for (auto const &item : _buckets) {
    result.append(QString::fromStdString(item.toString())).append("\n");
  }
  return result;
}

Structures::WeightUnit BucketsModel::currentUnit() const {
  return _currentUnit;
}

void BucketsModel::setCurrentUnit(const Structures::WeightUnit &currentUnit) {
  beginResetModel();
  _currentUnit = currentUnit;
  endResetModel();
  emit dataUpdated();
}

QVector<Structures::Bucket> BucketsModel::buckets() const { return _buckets; }

void BucketsModel::setBuckets(const QVector<Structures::Bucket> &buckets) {
  beginResetModel();
  _buckets = buckets;
  endResetModel();
  emit dataUpdated();
}
