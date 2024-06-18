#include "orderbook.hpp"
#include <stdexcept>
using namespace fast_ob;

template <bool is_bid> bool first_is_worse(int64_t first, int64_t second) {
  if constexpr (is_bid) {
    return first < second;
  } else {
    return first > second;
  }
}

Orderbook::Orderbook(uint64_t ob_size) : ob_capacity{ob_size} {
  if (ob_size == 0) {
    throw std::invalid_argument("ob_size cannot be 0");
  }
}

template <bool is_bid>
__always_inline void
update_internals(phmap::btree_map<int64_t, uint64_t> &tiny_book,
                 UpdateDump<is_bid> &dump, uint64_t tiny_book_capacity,
                 int64_t price, uint64_t quantity) {
  if (tiny_book.size() < tiny_book_capacity) {
    tiny_book[price] = quantity;
    return;
  }

  const int64_t &worst_price{is_bid ? tiny_book.begin()->first
                                    : tiny_book.rbegin()->first};

  if (first_is_worse<is_bid>(price, worst_price)) {
    dump.update(price, quantity);
  } else {
    tiny_book[price] = quantity;
    if (tiny_book.size() > tiny_book_capacity) {
      auto pair{tiny_book.extract(tiny_book.rbegin()->first)};
      dump.update(pair.key(), pair.mapped());
    }
  }
}

void Orderbook::set_bid(int64_t price, uint64_t quantity) {
  update_internals<true>(tiny_bids, bid_dump, ob_capacity, price, quantity);
}

void Orderbook::set_ask(int64_t price, uint64_t quantity) {
  update_internals<false>(tiny_bids, ask_dump, ob_capacity, price, quantity);
}

template <bool is_bid>
__always_inline void
remove_from_internals(phmap::btree_map<int64_t, uint64_t> &tiny_book,
                      UpdateDump<is_bid> &dump, uint64_t tiny_book_capacity,
                      int64_t price) {
  const int64_t &worst_price{is_bid ? tiny_book.begin()->first
                                    : tiny_book.rbegin()->first};

  if (first_is_worse<is_bid>(price, worst_price)) {
    dump.update(price, 0);
  } else if (auto iter{tiny_book.find(price)}; iter != tiny_book.end()) {
    tiny_book.erase(iter);
  }

  if (tiny_book.empty()) {
    while (tiny_book.size() < tiny_book_capacity and dump.size() > 0) {
      int64_t prc{};
      uint64_t qty{};
      if (not dump.pop(prc, qty)) {
        break;
      }
      tiny_book[prc] = qty;
    }
  }
}

void Orderbook::remove_bid(int64_t price) {
  if (tiny_bids.empty()) [[unlikely]] {
    return;
  }
  remove_from_internals(tiny_bids, bid_dump, ob_capacity, price);
}

void Orderbook::remove_ask(int64_t price) {
  if (tiny_asks.empty()) [[unlikely]] {
    return;
  }
  remove_from_internals(tiny_asks, ask_dump, ob_capacity, price);
}
