#include <boost/filesystem.hpp>
#include <boost/process.hpp>
#include <boost/program_options.hpp>
#include <iomanip>
#include <iostream>
#include "DisasmData.h"
#include "Logger.h"
#include "PrettyPrinter.h"

int main(int argc, char** argv) {
  boost::program_options::options_description desc("Allowed options");
  desc.add_options()("help", "Produce help message.");
  desc.add_options()("ir,i", boost::program_options::value<std::string>(), "gtirb file to print.");
  desc.add_options()("out,o", boost::program_options::value<std::string>(),
                     "The name of the assembly output file.");
  desc.add_options()("debug,D", boost::program_options::value<bool>()->default_value(false),
                     "Turn on debugging (will break assembly)");

  boost::program_options::variables_map vm;
  boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);

  if (vm.count("help") != 0) {
    std::cout << desc << "\n";
    return 1;
  }

  boost::program_options::notify(vm);

  gtirb::Context ctx;
  gtirb::IR* ir;

  if (vm.count("ir") != 0) {
    boost::filesystem::path irPath = vm["ir"].as<std::string>();
    if (boost::filesystem::exists(irPath) == true) {
      LOG_INFO << std::setw(24) << std::left << "Reading IR: " << irPath << std::endl;
      std::ifstream in(irPath.string());
      ir = gtirb::IR::load(ctx, in);
    } else {
      LOG_ERROR << "IR not found: \"" << irPath << "\".";
      return EXIT_FAILURE;
    }
  } else {
    ir = gtirb::IR::load(ctx, std::cin);
  }

  // Perform the Pretty Printing step.
  PrettyPrinter pp;
  pp.setDebug(vm["debug"].as<bool>());
  const auto assembly = pp.prettyPrint(ctx, ir);

  // Do we write it to a file?
  if (vm.count("out") != 0) {
    const auto asmPath = boost::filesystem::path(vm["out"].as<std::string>());
    std::ofstream ofs;
    ofs.open(asmPath.string());

    if (ofs.is_open() == true) {
      ofs << assembly;
      ofs.close();
      LOG_INFO << "Assembly written to: " << asmPath << "\n";
    } else {
      LOG_ERROR << "Could not output assembly output file: " << asmPath << "\n";
    }
  } else {
    std::cout << assembly << std::endl;
  }

  return EXIT_SUCCESS;
}