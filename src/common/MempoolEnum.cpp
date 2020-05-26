/*
 * Copyright (C) 2020 Zilliqa
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

#include "MempoolEnum.h"
#include "libUtils/Logger.h"

bool DroppedTxnContainer::insert(const TxnHash& txhash,
                                 const PoolTxnStatus status,
                                 const uint64_t& epochNum) {
  m_txnHashExpiration[epochNum].emplace_back(txhash);
  LOG_GENERAL(INFO, "[DTXN]"
                        << "Inserted " << txhash << " at " << epochNum);
  return m_txnCode.emplace(txhash, status).second;
}

void DroppedTxnContainer::clear(const uint64_t& epochNum, const uint& TTL) {
  LOG_MARKER();
  if (TTL > epochNum) {
    return;
  }
  const uint64_t& oldEpoch = epochNum - TTL;

  LOG_GENERAL(INFO, "[DTXN]"
                        << "Removing epochNum: " << oldEpoch << " at "
                        << epochNum);

  if (m_txnHashExpiration.find(oldEpoch) != m_txnHashExpiration.end()) {
    for (const auto& txhash : m_txnHashExpiration[oldEpoch]) {
      m_txnCode.erase(txhash);
      LOG_GENERAL(INFO, "[DTXN]"
                            << "Remove " << txhash);
    }

    m_txnHashExpiration.erase(oldEpoch);
  }
}

const HashCodeMap& DroppedTxnContainer::GetHashCodeMap() const {
  return m_txnCode;
}
