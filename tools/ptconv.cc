#include <iostream>
#include <map>
#include <functional>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/info_parser.hpp>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

using namespace boost::property_tree;

typedef void readerf(std::istream &, ptree &);
typedef void writerf(std::ostream &, ptree &);

int
main(int argc, char **argv)
{
  ptree pt;
   
  std::map<std::string,  std::pair<std::function<readerf>, std::function<writerf>>> formats =
    {
      {"xml", {[](std::istream &i, ptree &p){ xml_parser::read_xml(i, p);},
	       [](std::ostream &o, ptree &p){ xml_parser::write_xml(o, p); }}},
      {"json", {[](std::istream &i, ptree &p){ json_parser::read_json(i, p);},
		[](std::ostream &o, ptree &p){ json_parser::write_json(o, p); }}},
      {"ini", {[](std::istream &i, ptree &p){ ini_parser::read_ini(i, p);},
	       [](std::ostream &o, ptree &p){ ini_parser::write_ini(o, p); }}},
      {"info", {[](std::istream &i, ptree &p){ info_parser::read_info(i, p);},
	       [](std::ostream &o, ptree &p){ info_parser::write_info(o, p); }}}
    };

  po::options_description desc("Available options.");
  desc.add_options()
    ("from,f", po::value<std::string>(), "from format")
    ("to,t", po::value<std::string>(), "to format");

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if (vm.count("from") != 1) {
    std::cerr << "No from format specified!\n";
    return 1;
  }
  std::string sfrom = vm["from"].as<std::string>();
  if (!formats.count(sfrom)) {
    std::cerr << "Invalid from format!\n";
    return 1;
  }
  if (vm.count("to") != 1) {
    std::cerr << "No to format specified!\n";
    return 1;
  }
  std::string sto = vm["to"].as<std::string>();
  if (!formats.count(sto)) {
    std::cerr << "Invalid to format!\n";
    return 1;
  }

  auto rd = formats[sfrom].first;
  auto wr = formats[sto].second;

  rd(std::cin, pt);
  wr(std::cout, pt);  
  std::cout << std::endl;
   
  return 0;
  
}
