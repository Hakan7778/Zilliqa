/*
 * Copyright (C) 2019 Zilliqa
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef ZILLIQA_SRC_COMMON_MEMPOOLENUM_H_
#define ZILLIQA_SRC_COMMON_MEMPOOLENUM_H_

#include "depends/common/FixedHash.h"

using TxnHash = dev::h256;

enum PoolTxnStatus : uint8_t {
  NOT_PRESENT = 0,
  PRESENT_NONCE_HIGH = 1,
  PRESENT_GAS_EXCEEDED = 2,
  PRESENT_VALID_CONSENSUS_NOT_REACHED = 3,
  ERROR = 4,
  MATH_ERROR = 10,
  INVALID_FROM_ACCOUNT = 11,
  INSUFFICIENT_BALANCE = 12,
  VERIF_ERROR = 13,
  INSUFFICIENT_GAS = 14,
  INSUFFICIENT_GAS_LIMIT = 15,
  INVALID_TO_ACCOUNT = 16,
  FAIL_CONTRACT_ACCOUNT_CREATION = 17,
  FAIL_SCILLA_LIB = 18,
  FAIL_CONTRACT_INIT = 19,
  INCORRECT_TXN_TYPE = 20
};

inline bool IsPoolTxnDropped(PoolTxnStatus code) {
  return (static_cast<uint8_t>(code) >= 10);
}

using HashCodeMap = std::unordered_map<TxnHash, PoolTxnStatus>;

class DroppedTxnContainer {
 private:
  std::unordered_map<uint64_t, std::vector<TxnHash>> m_txnHashExpiration;
  HashCodeMap m_txnCode;

 public:
  bool insert(const TxnHash& txhash, const PoolTxnStatus status,
              const uint64_t& epochNum) {
    m_txnHashExpiration[epochNum].emplace_back(txhash);
    return m_txnCode.emplace(txhash, status).second;
  }

  void clear(const uint64_t& epochNum, const uint& TTL) {
    if (TTL > epochNum) {
      return;
    }
    const auto& oldEpoch = epochNum - TTL;

    if (m_txnHashExpiration.find(oldEpoch) != m_txnHashExpiration.end()) {
      for (const auto& txhash : m_txnHashExpiration[oldEpoch]) {
        m_txnCode.erase(txhash);
      }

      m_txnHashExpiration.erase(oldEpoch);
    }
  }

  const HashCodeMap& GetHashCodeMap() const { return m_txnCode; }
};

enum PendingData { HASH_CODE_MAP, PUBKEY, SHARD_ID };

#endif  // ZILLIQA_SRC_COMMON_MEMPOOLENUM_H_
