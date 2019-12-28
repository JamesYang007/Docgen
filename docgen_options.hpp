#pragma once

#include <getopt.h>
#include <cctype>
#include <cstdio>
#include <string>
#include <iostream>
#include <fstream>
#include "docgen_exceptions.hpp"

static char **files_src_paths;
static std::istream *parsed_src = NULL;
static std::ostream *parsed_dst = NULL;
static const char *docs_dst_path = ".";

static inline void set_options_global(int argc, char **argv)
{
	int c;
	while ((c = getopt(argc, argv, ":i:o:d:")) != -1) {
		switch (c) {
			case 'i':
				parsed_src = strcmp(optarg, "-") == 0 ? &std::cin : new std::ifstream(optarg);
				if (parsed_src->fail()) {
					throw docgen::system_error("failed to open istream");
				}
				break;
			case 'o':
				parsed_dst = strcmp(optarg, "-") == 0 ? &std::cout : new std::ofstream(optarg);
				if (parsed_dst->fail()) {
					throw docgen::system_error("failed to open ostream");
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

static inline void cleanup_options_global()
{
	if (parsed_src && parsed_src != &std::cin) {
		delete parsed_src;
	}
	if (parsed_dst && parsed_dst != &std::cout) {
		delete parsed_dst;
	}
}
