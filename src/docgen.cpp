#include <getopt.h>
#include <cstdio>
#include <cctype>
#include <cstring>
#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>
#include "exceptions/exceptions.hpp"
#include "parse_file.hpp"

using namespace docgen;

/* Options */
static const char * const *files_src_paths;
static std::shared_ptr<std::istream> parsed_src(nullptr);
static std::shared_ptr<std::ostream> parsed_dst(nullptr);
static std::shared_ptr<std::ostream> logger(&std::cerr, [](std::ostream *){});
static std::shared_ptr<std::ostream> err(&std::cerr, [](std::ostream *){});
static const char *docs_dst_path = ".";

/*
 * Iterates over flags from passed argv to set global options accordingly
 */
static inline void set_options(int argc, char **argv)
{
	int c;
	while ((c = getopt(argc, argv, ":i:o:l:e:d:")) != -1) {
		switch (c) {
			case 'i':
				if (strcmp(optarg, "-") == 0) {
					parsed_src = std::shared_ptr<std::istream>(&std::cin, [](std::istream *){});
				}
				else {
					parsed_src = std::make_shared<std::ifstream>(optarg);
					if (parsed_src->fail()) {
						throw exceptions::file_open_error(optarg);
					}
				}
				break;
			case 'o':
				if (strcmp(optarg, "-") == 0) {
					parsed_dst = std::shared_ptr<std::ostream>(&std::cout, [](std::ostream *){});
				}
				else {
					parsed_dst = std::make_shared<std::ofstream>(optarg);
					if (parsed_dst->fail()) {
						throw exceptions::system_error("failed to open output stream");
					}
				}
				break;
			case 'l':
				logger = std::make_shared<std::ofstream>(optarg, std::ofstream::app);
				if (logger->fail()) {
					throw exceptions::system_error("failed to open logging stream");
				}
				break;
			case 'e':
				err = std::make_shared<std::ofstream>(optarg, std::ofstream::app);
				if (err->fail()) {
					throw exceptions::system_error("failed to open error stream");
				}
				break;
			case 'd':
			{
				size_t arg_len = strlen(optarg);
				if (optarg[arg_len-1] == '/') {
					optarg[arg_len-1] = 0;
				}
				docs_dst_path = optarg;
				break;
			}
			case ':':
				fprintf(stderr, "Option -%c requires an argument.\n", optopt);
				throw exceptions::bad_flags();
				break;
			case '?':
				if (isprint(optopt)) {
					fprintf (stderr, "Unknown option `-%c'.\n", optopt);
				}
				else {
					fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
				}
				throw exceptions::bad_flags();
				break;
			default:
				fprintf(stderr, "Encountered an unknown error parsing options.\n");
				throw exceptions::bad_flags();
		}
	}
	files_src_paths = argv + optind;
}

/* Parsing data JSON */
static nlohmann::json parsed;

/*
 * Populates global JSON with parsing data as per global options
 */
static inline void to_parsed()
{
	if (parsed_src) {
		*parsed_src >> parsed;	
		if (parsed_src->fail()) {
			throw exceptions::system_error("failed to read from input stream");
		}
		return;
	}

	if (!files_src_paths) {
		throw exceptions::control_flow_error("global var 'files_src_paths' not set for routine 'to_parsed'");
	}

	namespace fs = std::filesystem;

	while (*files_src_paths) {
		fs::file_status status = fs::status(*files_src_paths);

		if (fs::is_regular_file(status)) {
			*logger << "Processing \"" << *files_src_paths << '\"' << '\n';
			parse_file(*files_src_paths, parsed);
		}
		else if (fs::is_directory(status)) {
			for (const fs::directory_entry& entry : fs::recursive_directory_iterator(*files_src_paths)) {
				if (entry.is_regular_file()) {
					*logger << "Processing " << entry.path() << '\n';
					parse_file(entry.path().c_str(), parsed);
				}
				else if (!entry.is_directory()) {
					*logger << "Skipping " << entry.path() << " (not regular file)" << '\n';
				}
			}
		}
		else if (!fs::exists(status)) {
			throw exceptions::file_exist_error(*files_src_paths);
		}
		else {
			throw exceptions::bad_file(*files_src_paths);
		}

		++files_src_paths;
	}
}

/*
 * Handles parsing data from global JSON as per global options
 */
static inline void from_parsed()
{
	if (parsed.is_null()) {
		throw exceptions::bad_source();
	}

	if (parsed_dst) {
		*parsed_dst << parsed;
		if (parsed_dst->fail()) {
			throw exceptions::system_error("failed to write to output stream");
		}
		return;
	}

	// TODO generate docs (writing files as necessary) to global path docs_dst_path from json parsed
}

int main(int argc, char **argv)
{
	static int status = 0;

	try {
		set_options(argc, argv);

		to_parsed();

		from_parsed();
	}
	catch (const exceptions::exception& de) {
		*err << de.what() << '\n';
		status = 1;
	}
	catch (const std::exception& e) {
		*err << "Docgen encountered an error: " << e.what() << '\n';
		status = 1;
	}

	return status;	
}
