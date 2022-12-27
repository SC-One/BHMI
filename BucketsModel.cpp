#include "BucketsModel.h"

#include <cassert>

static constexpr int COLUMNS = 2;  // 1 is better, now just I'm filling

BucketsModel::BucketsModel(QObject *parent)
    : QAbstractTableModel(parent),
      _currentUnit(Structures::WeightUnit::KiloGram) {}

int BucketsModel::rowCount(const QModelIndex & /*parent*/) const {
  return _buckets.size();
}

int BucketsModel::columnCount(const QModelIndex & /*parent*/) const {
  return COLUMNS;
}
static QString toString(Structures::WeightUnit const unit) {
  switch (unit) {
    case Structures::WeightUnit::Gram:
      return "Gram";
    case Structures::WeightUnit::KiloGram:
      return "KiloGram";
    case Structures::WeightUnit::Tonne:
      return "Tonne";
      //    default:
      //      return "Error!";
  }
}
QVariant BucketsModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid()) return QVariant();
  if (role == Qt::DisplayRole) switch (index.column()) {
      case 0:  // weight
        break;
      case 1:  // weight unit
        return toString(currentUnit());
      default:
        assert("Please report to the developer.");
        return "Assert!";
    }
  return QString("Row%1, Column%2")
      .arg(index.row() + 1)
      .arg(index.column() + 1);

  return QVariant();
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
      default:
        assert(
            "Please make sure that never come in this section!, report to the "
            "developer");
        return "Assert!";
    }
  } else
    return QStringLiteral("%1").arg(section);
}
void BucketsModel::addNewBucket(Structures::Bucket const &bucket) {
  beginResetModel();
  _buckets.insert(bucket);
  endResetModel();
}

QByteArray BucketsModel::toCSV() const {
  QByteArray result;
  for (auto const &item : _buckets) {
    result.append(QString::number(item.weight));
    result.append("\n");
  }
  return result;
}

Structures::WeightUnit BucketsModel::currentUnit() const {
  return _currentUnit;
}

void BucketsModel::setCurrentUnit(const Structures::WeightUnit &currentUnit) {
  _currentUnit = currentUnit;
}
