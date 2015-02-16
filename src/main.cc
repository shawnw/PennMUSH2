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
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/locale.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/info_parser.hpp>

#include "syswrap.h"

namespace po = boost::program_options;

using namespace boost::locale;

generator gen;
boost::property_tree::ptree config;

std::mt19937 rand_gen;

// When running suide root, only use root privleges for binding
// sockets to allow ports < 1024. Otherwise run as normal user.
bool in_suid_root_mode = false;

int main(int argc, char **argv) {
  std::string pidfilename;

  /* Check to make sure we're not running as root. */
  if (getuid() == 0) {
    BOOST_LOG_TRIVIAL(fatal) << "PennMUSH will not run as root as a security measure. Please run the server as a different user.";
    return 1;
  }
  if (geteuid() == 0) {
    try {
      sys_seteuid(getuid());
      in_suid_root_mode = true;
      BOOST_LOG_TRIVIAL(info) << "Running in suid-root mode. Dropping root privileges.";
    } catch (errno_exception &e) {
      BOOST_LOG_TRIVIAL(fatal) << "Unable to drop root privileges: " << e.what();
      return 1;
    }
  }
    
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
    BOOST_LOG_TRIVIAL(info) << "Reading config file '" << cfile << '\'';
    try {
      boost::property_tree::read_info(cfile, config);
    } catch (boost::property_tree::ptree_error &e) {
      BOOST_LOG_TRIVIAL(fatal) << "Unable to read config file: " << e.what();
      return 1;
    }
  }
  
  
  // Set up log filters
  {
    namespace logging =  boost::log;
    std::string level = config.get("logging.minimum_level", "debug");
    int lev;
    if (level == "trace")
      lev = logging::trivial::trace;
    else if (level == "debug")
      lev = logging::trivial::debug;
    else if (level == "info")
      lev = logging::trivial::info;
    else if (level == "warning")
      lev = logging::trivial::warning;
    else if (level == "fatal")
      lev = logging::trivial::fatal;
    else {
      BOOST_LOG_TRIVIAL(fatal) << "Unknown logging level '" << level << "' specified.";
      return 1;
    }
    logging::core::get()->set_filter(logging::trivial::severity >= lev);
  }
  
  // Session management to detatch from a console.
  if (!vm.count("no-session")) {
    try {
      pid_t child = sys_fork();
      if (child == 0) {
	pid_t session = sys_setsid();
	BOOST_LOG_TRIVIAL(trace) << "Process id " << getpid() << " and session id " << session;
      } else {
	return 0;
      }
    } catch (errno_exception &e) {
      if (e.funcname() == "fork")
	BOOST_LOG_TRIVIAL(warning) << e.what();
      else
	BOOST_LOG_TRIVIAL(debug) << e.what();
    }
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
