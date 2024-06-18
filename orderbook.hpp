#pragma once
#include "update_dump.hpp"
#include <cstdint>
#include <parallel_hashmap/btree.h>

namespace fast_ob {
struct Orderbook {
  Orderbook(uint64_t ob_size);
  void set_bid(int64_t price, uint64_t quantity);
  void set_ask(int64_t price, uint64_t quantity);
  void remove_bid(int64_t price);
  void remove_ask(int64_t price);

private:
  phmap::btree_map<int64_t, uint64_t> tiny_bids;
  phmap::btree_map<int64_t, uint64_t> tiny_asks;
  UpdateDump<true> bid_dump;
  UpdateDump<false> ask_dump;
  uint64_t ob_capacity;
};
} // namespace fast_ob