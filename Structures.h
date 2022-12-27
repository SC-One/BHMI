#ifndef STRUCTURES_H
#define STRUCTURES_H
#include <QObject>
#include <QString>
#include <cstdint>

namespace Structures {

enum WeightUnit { Gram, KiloGram, Tonne };

struct Bucket {
  std::uint_fast32_t weight;  // default in KiloGram(TODO: be sure)
  bool operator<(Bucket const& other) const { return weight < other.weight; }
  bool operator==(Bucket const& other) const { return weight == other.weight; }
};
}  // namespace Structures

#endif  // STRUCTURES_H
