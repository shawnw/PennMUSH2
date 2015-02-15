#include "config.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include <cstring>
#include <cerrno>
#include <ctime>
#include <cstdint>
#include <random>
#include <unistd.h>
#include <boost/program_options.hpp>
#include <boost/log/trivial.hpp>
#include <boost/locale.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/info_parser.hpp>

namespace po = boost::program_options;

using namespace boost::locale;

generator gen;
boost::property_tree::ptree config;

std::mt19937 rand_gen;

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

  // READ CONFIG FILE
  if (!vm.count("config-file")) {
    BOOST_LOG_TRIVIAL(fatal) << "No configuration file specified on command line!";
    return 1;
  } else {
    std::string cfile = vm["config-file"].as<std::string>();
    BOOST_LOG_TRIVIAL(trace) << "Config file is " << cfile;
    boost::property_tree::read_info(cfile, config);
  }

  // Session management to detatch from a console.
  if (!vm.count("no-session")) {
    pid_t child = fork();
    if (child > 0)
      return 0;
    else if (child == 0) {
      if (setsid() < 0) 
	BOOST_LOG_TRIVIAL(warning) << "setsid failed: " << std::strerror(errno);
      else
	BOOST_LOG_TRIVIAL(trace) << "Process id " << getpid() << " and session id " << getsid(getpid());
    } else 
      BOOST_LOG_TRIVIAL(warning) << "initial fork failed: " << std::strerror(errno);
  }
  
  if (vm.count("pid-file")) {
    pidfilename = vm["pid-file"].as<std::string>();
    BOOST_LOG_TRIVIAL(trace) << "Writing pid to " << pidfilename;
    std::ofstream pidfile(pidfilename.c_str());
    pidfile << getpid() << '\n';
  }

  // SET TRANSLATION PATH AND LOCALE FROM CONFIG
  try {
    // Locale should be a utf-8 one.
    std::locale::global(gen(config.get("language.locale", "en_US.utf8")));
    std::cout.imbue(std::locale());
    std::cerr.imbue(std::locale());
    std::string translation_root = config.get("language.translation_root", "./translations/");
    BOOST_LOG_TRIVIAL(trace) << "Using " << translation_root << " for translation lookup.";
    gen.add_messages_path(translation_root);
    gen.add_messages_domain("PennMUSH2");
  } catch (conv::invalid_charset_error &e) {
    BOOST_LOG_TRIVIAL(fatal) << "Unable to set locale: " << e.what();
    return 1;
  }

  {
    std::random_device rd;
    BOOST_LOG_TRIVIAL(trace) << "Seeding RNG";
    rand_gen.seed(rd());
  }
  
  // READ DATABASE, SETUP GAME WORLD

  // SETUP NETWORKING
  
  // ENTER MAIN LOOP


  // SAVE DATABASE, OTHER CLEANUP
  
  if (!pidfilename.empty())
    std::remove(pidfilename.c_str());

  BOOST_LOG_TRIVIAL(info) << "netmush exiting";
  
  return 0;
}
