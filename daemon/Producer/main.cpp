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

#include <boost/asio/io_service.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>
#include <ndnabac/producer.hpp>

#include <thread>

#include "abac-identity.hpp"
#include "ndnabacdaemon-common.hpp"
#include "io-service-manager.hpp"

void
printUsage(std::ostream& os, const std::string& programName)
{
  os << "Usage: \n"
     << "  " << programName << " [options]\n"
     << "\n"
     << "VO-NDN daemon\n"
     << "\n"
     << "Options:\n"
     << "  [--help]    - print this help message\n"
     << "  [--proName]    - assign the producer name"
     << "(default: " << "/producerPrefix" << ")\n"
     << "  [--aaName]    - assign the attribute authority name"
     << "(default: " << "/aaPrefix" << ")\n"
     ;
}

int
main(int argc, char** argv)
{
  namespace po = boost::program_options;

  po::options_description description;

  std::string producerName = "/producerPrefix";
  std::string aaName = "/aaPrefix";
  std::string configFile;
  description.add_options()
    ("help,h", "print this help message")
    ("pname,p", po::value<std::string>(&producerName), "Producer Name")
    ("aname,a", po::value<std::string>(&aaName), "Attribute Authority Name")
    ("config,c", po::value<std::string>(&configFile), "Config file path")
    ;

  po::variables_map vm;
  try {
    po::store(po::command_line_parser(argc, argv).options(description).run(), vm);
    po::notify(vm);
  }
  catch (const std::exception& e) {
    // avoid NFD_LOG_FATAL to ensure that errors related to command-line parsing always appear on the
    // terminal and are not littered with timestamps and other things added by the logging subsystem
    std::cerr << "ERROR: " << e.what() << std::endl;
    printUsage(std::cerr, argv[0]);
    return 1;
  }

  if (vm.count("help") > 0) {
    printUsage(std::cout, argv[0]);
    return 0;
  }

  std::unique_ptr<boost::asio::io_service> io_service(new boost::asio::io_service);
  std::unique_ptr<ndn::Face> face(new ndn::Face(*io_service));
  ndn::KeyChain keyChain("pib-memory:", "tpm-memory:");

  // Import config for data owner.
  std::string line;
  std::ifstream policyConfig(configFile);
  if (policyConfig.is_open())
  {
    while (getline(policyConfig, line))
    {
      std::size_t pos = line.find(",");
      if (pos == std::string::npos) {   
        std::cerr << "ERROR: " << "config format error" << std::endl;
        return 1;
      }
      ndn::Name dataName = line.substr(0, pos);
      std::string filePath = line.substr(pos+1);
      producerFace.setInterestFilter(producerCert.getIdentity().append(dataName),
        [&] (const ndn::InterestFilter&, const ndn::Interest& interest) {
          std::ifstream inputFile(filePath);

          if (inputFile.is_open()) {
            std::string content((std::istreambuf_iterator<char>(inputFile)),
                                (std::istreambuf_iterator<char>()));
            producer.produce(dataName, content.c_str(), sizeof(content.c_str()),
            [&] (const Data& data) {
              isProdCbCalled = true;

              NDN_LOG_INFO("data successfully encrypted");
              producerFace.put(data);
            },
            [&] (const std::string& err) {
              std::cout << err << std::endl;
            });
          }
          inputFile.close();
        }
      );
    }
  } else {
    std::cerr << "ERROR: " << "config doesn't exist" << std::endl;
    printUsage(std::cerr, argv[0]);
    return 1;
  }
  policyConfig.close();
	// set up AA
  ndn::security::Identity identity = ndn::ndnabacdaemon::addIdentity(producerName, keyChain);
  ndn::security::Key key = identity.getDefaultKey();
  ndn::security::v2::Certificate cert = key.getDefaultCertificate();
  ndn::ndnabac::Producer producer(cert, *face, keyChain, ndn::Name(aaName));
  try {
    boost::asio::io_service::work ioServiceWork(*io_service);
    io_service->run();
  }
  catch (const std::exception& e) {
    std::cout << "Start IO service or Face failed" << std::endl;
    return 1;
  }
  return 0;
}