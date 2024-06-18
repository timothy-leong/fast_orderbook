#include "update_dump.hpp"
using namespace fast_ob;

template <bool is_bid>
void UpdateDump<is_bid>::update(int64_t price, uint64_t quantity) {
  if (quantity > 0) {
    if (not latest_quantities.contains(price)) {
      ordered_prices.push(price);
    }
    latest_quantities[price] = quantity;
  } else if (auto iter{latest_quantities.find(price)};
             iter != latest_quantities.end()) {
    iter->second = 0;
  }
}

template <bool is_bid>
__always_inline bool UpdateDump<is_bid>::pop(int64_t &price,
                                             uint64_t &quantity) {
  while (not ordered_prices.empty()) {
    const uint64_t top_price{ordered_prices.top()};
    if (latest_quantities[top_price] == 0) {
      ordered_prices.pop();
      latest_quantities.erase(top_price);
    } else {
      break;
    }
  }

  if (ordered_prices.empty()) [[unlikely]] {
    return false;
  }

  price = ordered_prices.top();
  quantity = latest_quantities[price];
  return true;
}

template <bool is_bid> int UpdateDump<is_bid>::size() {
  return latest_quantities.size();
}
