//
// Created by Nicolai Bergulff on 12/06/2025.
//
#ifndef STOCHASTIC_SYMBOL_TABLE_H
#define STOCHASTIC_SYMBOL_TABLE_H

#include <unordered_map>
#include <vector>
#include "exceptions.h"

namespace Stochastic {
    // R3
    template<typename Key, typename Value>
    class SymbolTable {
        std::unordered_map<Key, Value> table_;
    public:
        void add(const Key& key, const Value& value) {
            if (table_.find(key) != table_.end())
                throw SymbolTableException("Symbol already exists: " + key);
            table_[key] = value;
        }

        const Value& get(const Key& key) const {
            auto it = table_.find(key);
            if (it == table_.end())
                throw SymbolTableException("Symbol not found: " + key);
            return it->second;
        }

        bool contains(const Key& key) const {
            return table_.find(key) != table_.end();
        }

        void update(const Key& key, const Value& value) {
            if (!contains(key))
                throw SymbolTableException("Cannot update non-existent symbol: " + key);
            table_[key] = value;
        }

        std::vector<Key> keys() const {
            std::vector<Key> result;
            for (const auto& kv : table_)
                result.push_back(kv.first);
            return result;
        }
    };
}
#endif // STOCHASTIC_SYMBOL_TABLE_H
