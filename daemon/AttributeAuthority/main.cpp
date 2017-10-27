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

#include <ndn-cxx/face.hpp>
#include <ndn-cxx/security/key-chain.hpp>
#include <ndnabac/attribute-authority.hpp>
#include <boost/asio/io_service.hpp>

#include "ndnabacdaemon_common.hpp"

ndn::security::Identity
addIdentity(const ndn::Name& identityName,
						ndn::KeyChain& keyChain,
						const ndn::KeyParams& params = ndn::security::v2::KeyChain::getDefaultKeyParams())
{
  auto identity = keyChain.createIdentity(identityName, params);
  return identity;
}

int
main(int argc, char** argv)
{
	std::unique_ptr<boost::asio::io_service> io_service(new boost::asio::io_service);
	std::unique_ptr<ndn::Face> face(new ndn::Face(*io_service));
	ndn::KeyChain keyChain;
	// set up AA
  ndn::security::Identity identity = addIdentity("/aaPrefix", keyChain);
  ndn::security::Key key = identity.getDefaultKey();
  ndn::security::v2::Certificate cert = key.getDefaultCertificate();

  ndn::ndnabac::AttributeAuthority aa = ndn::ndnabac::AttributeAuthority(cert, *face, keyChain);
  return 0;
}