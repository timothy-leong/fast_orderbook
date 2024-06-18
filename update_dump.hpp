#pragma once

#include <cstdint>
#include <parallel_hashmap/phmap.h>
#include <queue>
#include <type_traits>
#include <vector>

namespace fast_ob {
template <bool is_bid> struct UpdateDump {
  void update(int64_t price, uint64_t quantity);
  bool pop(int64_t &price, uint64_t &quantity);
  int size();

private:
  phmap::flat_hash_map<int64_t, uint64_t> latest_quantities;
  std::priority_queue<
      int64_t, std::vector<int64_t>,
      std::conditional_t<is_bid, std::less<void>, std::greater<void>>>
      ordered_prices;
};

} // namespace fast_ob