#include "config.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include <cstring>
#include <cerrno>
#include <unistd.h>
#include <boost/program_options.hpp>
#include <boost/log/trivial.hpp>
#include <boost/locale.hpp>

namespace po = boost::program_options;

using namespace boost::locale;

int main(int argc, char **argv) {
  std::string pidfilename;
  
  po::options_description desc("Available options");
  desc.add_options()
    ("help", "display options.")
    ("pid-file", po::value<std::string>(), "name of file to record pid")
    ("no-session", "disable session management")
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

  if (!vm.count("config-file")) {
    BOOST_LOG_TRIVIAL(fatal) << "No configuration file specified on command line!";
    return 1;
  }

  if (!vm.count("no-session")) {
    pid_t child = fork();
    if (child > 0)
      return 0;
    else if (child == 0) {
      if (setsid() < 0) 
	BOOST_LOG_TRIVIAL(warning) << "setsid failed: " << std::strerror(errno);
      else
	BOOST_LOG_TRIVIAL(debug) << "Process id " << getpid() << " and session id " << getsid(getpid());
    } else 
      BOOST_LOG_TRIVIAL(warning) << "initial fork failed: " << std::strerror(errno);
  }
  
  if (vm.count("pid-file")) {
    pidfilename = vm["pid-file"].as<std::string>();
    BOOST_LOG_TRIVIAL(info) << "Writing pid to " << pidfilename;
    std::ofstream pidfile(pidfilename.c_str());
    pidfile << getpid() << '\n';
  }

  generator gen;
  gen.add_messages_path("./translations/");
  gen.add_messages_domain("PennMUSH2");

  // Locale should be a utf-8 one.
  std::locale::global(gen(""));
  std::cout.imbue(std::locale());
  std::cerr.imbue(std::locale());

  std::cout << "Config file is: " << vm["config-file"].as<std::string>() << '\n';
  // READ CONFIG FILE

  // READ DATABASE, SETUP GAME WORLD

  // SETUP NETWORKING
  
  // ENTER MAIN LOOP


  // SAVE DATABASE, OTHER CLEANUP
  
  if (!pidfilename.empty())
    std::remove(pidfilename.c_str());

  BOOST_LOG_TRIVIAL(info) << "netmush exiting";
  
  return 0;
}
