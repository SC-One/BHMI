#ifndef STRUCTURES_H
#define STRUCTURES_H
#include <QObject>
#include <cstdint>
#include <string>

namespace Structures {

enum WeightUnit { Gram, KiloGram, Tonne };

struct Bucket {
  std::uint_fast32_t weight;  // default in KiloGram(TODO: be sure)
  std::string description;

 public:
  bool operator<(Bucket const& other) const { return weight < other.weight; }
  bool operator==(Bucket const& other) const { return weight == other.weight; }

  std::string toString() const {
    return std::to_string(weight) + ",\"" + description + "\"";
  }

  inline double convert(WeightUnit const unit) {
    switch (unit) {
      case Structures::WeightUnit::Gram:
        return weight * 1e3;
      case Structures::WeightUnit::Tonne:
        return weight / 1e3;
      default:
        return weight;
    }
  }
};
}  // namespace Structures

#endif  // STRUCTURES_H
