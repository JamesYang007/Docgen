#include <getopt.h>
#include <cctype>
#include <iostream>
#include <fstream>

static char **files_src_paths;
static std::istream *parsed_src = NULL;
static std::ostream *parsed_dst = NULL;
static const char *docs_dst_path = ".";

static inline void parse_args_global(int argc, char **argv)
{
	int c;
	while ((c = getopt(argc, argv, "i:o:d:")) != -1) {
		switch (c) {
			case 'i':
				parsed_src = strcmp(optarg, "-") == 0 ? &std::cin : new std::ifstream(optarg);
				break;
			case 'o':
				parsed_dst = strcmp(optarg, "-") == 0 ? &std::cout : new std::ofstream(optarg);
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
			case '?':
				if (optopt == 'c') {
					fprintf(stderr, "Option -%c requires an argument.\n", optopt);
				}
				else if (isprint(optopt)) {
					fprintf (stderr, "Unknown option `-%c'.\n", optopt);
				}
				else {
					fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
				}
				exit(1);
			default:
				fprintf(stderr, "Encountered an unknown error parsing options.\n");
				exit(1);
		}
	}
	files_src_paths = argv + optind;
}

static inline void cleanup_global()
{
	if (parsed_src && parsed_src != &std::cin) {
		delete parsed_src;
	}
	if (parsed_dst && parsed_dst != &std::cout) {
		delete parsed_dst;
	}
}
