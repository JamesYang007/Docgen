#include <getopt.h>
#include <cstdio>
#include <cctype>
#include <cstring>
#include <string>
#include <iostream>
#include <fstream>
#include <memory>
#include <nlohmann/json.hpp>
#include "exceptions.hpp"

/* Options */
static const char * const *files_src_paths;
static std::shared_ptr<std::istream> parsed_src(nullptr, [](std::istream *p){});
static std::shared_ptr<std::ostream> parsed_dst(nullptr, [](std::ostream *p){});
static const char *docs_dst_path = ".";

/*
 * Iterates over flags from passed argv to set global options accordingly
 */
static inline void set_options(int argc, char **argv)
{
	int c;
	while ((c = getopt(argc, argv, ":i:o:d:")) != -1) {
		switch (c) {
			case 'i':
				if (strcmp(optarg, "-") == 0) {
					parsed_src = std::shared_ptr<std::istream>(&std::cin, [](std::istream *p){});
				}
				else {
					parsed_src = std::make_shared<std::ifstream>(optarg);
					if (parsed_src->fail()) {
						throw docgen::system_error("failed to open istream");
					}
				}
				break;
			case 'o':
				if (strcmp(optarg, "-") == 0) {
					parsed_dst = std::shared_ptr<std::ostream>(&std::cout, [](std::ostream *p){});
				}
				else {
					parsed_dst = std::make_shared<std::ofstream>(optarg);
					if (parsed_dst->fail()) {
						throw docgen::system_error("failed to open ostream");
					}
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
				throw docgen::bad_flags();
				break;
			case '?':
				if (isprint(optopt)) {
					fprintf (stderr, "Unknown option `-%c'.\n", optopt);
				}
				else {
					fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
				}
				throw docgen::bad_flags();
				break;
			default:
				fprintf(stderr, "Encountered an unknown error parsing options.\n");
				throw docgen::bad_flags();
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
			throw docgen::system_error("failed to read from istream");
		}
		return;
	}

	if (!files_src_paths) {
		throw docgen::control_flow_error("global var 'files_src_paths' not set for routine 'to_parsed'");
	}

	// TODO parse files from global NULL-terminated list files_src_paths and write information to json parsed
}

/*
 * Handles parsing data from global JSON as per global options
 */
static inline void from_parsed()
{
	if (parsed_dst) {
		*parsed_dst << parsed;
		if (parsed_dst->fail()) {
			throw docgen::system_error("failed to write to ostream");
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
	catch (const docgen::exception& de) {
		std::cerr << de.what() << '\n';
		status = 1;
	}
	catch (const std::exception& e) {
		std::cerr << "Docgen encountered an error: " << e.what() << '\n';
		status = 1;
	}

	return status;	
}
