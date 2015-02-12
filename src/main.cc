#include "config.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include <unistd.h>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

int main(int argc, char **argv) {
  std::string pidfilename;
  
  po::options_description desc("Available options");
  desc.add_options()
    ("help", "display options.")
    ("pid-file", po::value<std::string>(), "name of file to record pid")
    ("config-file", po::value<std::string>(), "name of config file");

  po::positional_options_description p;
  p.add("config-file", -1);
  
  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(),
	    vm);
  po::notify(vm);

  if (vm.count("help")) {
    std::cout << desc << '\n';
    return 0;
  }
  if (vm.count("pid-file")) {
    pidfilename = vm["pid-file"].as<std::string>();
    std::ofstream pidfile(pidfilename.c_str(), std::ofstream::out);
    pidfile << getpid() << '\n';
  }

  std::cout << "Config file is: " << vm["config-file"].as<std::string>() << '\n';
  
  if (!pidfilename.empty())
    std::remove(pidfilename.c_str());
  return 0;
}
