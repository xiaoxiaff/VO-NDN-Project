/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2017, Regents of the University of California.
 *
 * This file is part of ndnabacdaemon, a certificate management system based on NDN.
 *
 * ndnabac is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 *
 * ndnabac is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received copies of the GNU General Public License along with
 * ndnabacdaemon, e.g., in COPYING.md file.  If not, see <http://www.gnu.org/licenses/>.
 *
 * See AUTHORS.md for complete list of ndnabacdaemon authors and contributors.
 */

#include <ndn-cxx/security/key-chain.hpp>
#include <ndnabac/attribute-authority.hpp>
#include "abac-identity.hpp"

namespace ndn {
namespace ndnabacdaemon {

security::Identity
addIdentity(const ndn::Name& identityName,
			ndn::KeyChain& keyChain,
			const ndn::KeyParams& params)
{
  auto identity = keyChain.createIdentity(identityName, params);
  return identity;
}

} // namespace ndnabacdaemon
} // namespace ndn