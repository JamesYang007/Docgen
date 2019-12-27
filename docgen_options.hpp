#pragma once

#include <getopt.h>
#include <cctype>
#include <cstdio>
#include <iostream>
#include <fstream>
#include "docgen_status.hpp"

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
					perror("std::ifstream open failed");
					status = DG_FILE_NO;
					return;
				}
				break;
			case 'o':
				parsed_dst = strcmp(optarg, "-") == 0 ? &std::cout : new std::ofstream(optarg);
				if (parsed_dst->fail()) {
					perror("std::ofstream open failed");
					status = DG_FILE_NO;
					return;
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
				status = DG_FLAGS_NO;
				return;
			case '?':
				if (isprint(optopt)) {
					fprintf (stderr, "Unknown option `-%c'.\n", optopt);
				}
				else {
					fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
				}
				status = DG_FLAGS_NO;
				return;
			default:
				fprintf(stderr, "Encountered an unknown error parsing options.\n");
				status = DG_FLAGS_NO;
				return;
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
