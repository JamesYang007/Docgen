#include <getopt.h>
#include <cstdio>
#include <cctype>
#include <cstring>
#include <string>
#include <unordered_set>
#include <vector>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>
#include "exceptions/exceptions.hpp"
#include "parse_file.hpp"

using namespace docgen;

/* Defaults */
static constexpr const char * const CONFIG_PATH_DEFAULT = "./.docgen.json";
static constexpr const char * const SRC_PATH_DEFAULT = ".";
static constexpr const char * const DOCS_DST_PATH_DEFAULT = ".";
static constexpr std::ostream * const LOG_STREAM_DEFAULT = &std::cerr;
static constexpr std::ostream * const ERR_STREAM_DEFAULT = &std::cerr;

/* Configuration JSON keys */
static constexpr const char * const SOURCE_FILES_KEY = "source";
static constexpr const char * const EXCLUDE_FILES_KEY = "exclude";
static constexpr const char * const LOG_FILE_KEY = "logfile";
static constexpr const char * const ERR_FILE_KEY = "errfile";

/* Options */
static nlohmann::json config;
static std::vector<std::string> file_source_paths;
static std::unordered_set<std::string> file_source_excludes;
static std::shared_ptr<std::istream> parsed_src(nullptr);
static std::shared_ptr<std::ostream> parsed_dst(nullptr);
static std::shared_ptr<std::ostream> logger(LOG_STREAM_DEFAULT, [](std::ostream *){});
static std::shared_ptr<std::ostream> err(ERR_STREAM_DEFAULT, [](std::ostream *){});
static const char *docs_dst_path = DOCS_DST_PATH_DEFAULT;

/*
 * set_options() helper for opening output file stream and error-checking
 */
static inline void set_option_outfile_stream(std::shared_ptr<std::ostream>& stream, const std::string& path, bool append=false)
{
	stream = std::make_shared<std::ofstream>(path, append ? std::ofstream::app : std::ofstream::out);
	if (stream->fail()) {
		throw exceptions::system_error("failed to open output file stream");
	}
}

/*
 * set_options() helper for opening output file stream to append
 */
static inline void set_option_outfile_append_stream(std::shared_ptr<std::ostream>& stream, const char *path)
{
	set_option_outfile_stream(stream, path, true);
}

/*
 * Sets global options as per passed argv and docgen configuration file (if present)
 */
static inline void set_options(int argc, char **argv)
{
	// set by flags
	int c;
	while ((c = getopt(argc, argv, ":c:i:o:l:e:x:d:")) != -1) {
		switch (c) {
			case 'c':
				if (strcmp(optarg, "-") == 0) {
					std::cin >> config;	
				}
				else {
					std::ifstream in(optarg);
					if (in.fail()) {
						throw exceptions::file_open_error(optarg);
					}

					in >> config;
					if (in.fail()) {
						throw exceptions::system_error("failed to read from config file");
					}
				}
				break;
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
					set_option_outfile_stream(parsed_dst, optarg);
				}
				break;
			case 'l':
				set_option_outfile_append_stream(logger, optarg);
				break;
			case 'e':
				set_option_outfile_append_stream(err, optarg);
				break;

			case 'x':
				file_source_excludes.insert(std::filesystem::weakly_canonical(optarg));
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

	size_t file_source_count = argc - optind;

	// attempt to populate config json from default path if not set in flags
	if (config.is_null()) {
		std::filesystem::file_status status = std::filesystem::status(CONFIG_PATH_DEFAULT);
		if (std::filesystem::exists(status)) {
			std::ifstream in(CONFIG_PATH_DEFAULT);
			if (in.fail()) {
				throw exceptions::file_open_error(CONFIG_PATH_DEFAULT);
			}
			in >> config;
			if (in.fail()) {
				throw exceptions::system_error("failed to read from config file");
			}
		}
	}

	// set by information from config json if present
	nlohmann::json& config_source = config[SOURCE_FILES_KEY];
	nlohmann::json& config_exclude = config[EXCLUDE_FILES_KEY];
	nlohmann::json& config_logfile = config[LOG_FILE_KEY];
	nlohmann::json& config_errfile = config[ERR_FILE_KEY];
	std::string *val_ptr;

	file_source_count += config_source.size();
	file_source_paths.reserve(file_source_count);

	if (config_source.is_array()) {
		for (nlohmann::json& val : config_source) {
			if ((val_ptr = val.get_ptr<std::string *>())) {
				file_source_paths.push_back(std::move(*val_ptr));
			}
		}
	}
	if (config_exclude.is_array()) {
		for (nlohmann::json& val : config_exclude) {
			if ((val_ptr = val.get_ptr<std::string *>())) {
				file_source_excludes.insert(std::filesystem::weakly_canonical(std::move(*val_ptr)));
			}
		}
	}

	if (config_logfile.is_string() && logger.get() == LOG_STREAM_DEFAULT) {
		set_option_outfile_append_stream(logger, config_logfile.get_ref<std::string&>().c_str());
	}
	if (config_errfile.is_string() && err.get() == ERR_STREAM_DEFAULT) {
		set_option_outfile_append_stream(err, config_errfile.get_ref<std::string&>().c_str());
	}

	// set by any path arguments
	file_source_paths.insert<char **>(file_source_paths.end(), argv + optind, argv + argc);

	if (file_source_paths.empty()) {
		file_source_paths.push_back(SRC_PATH_DEFAULT);
	}
}

/* Parsing data JSON */
static nlohmann::json parsed;

/*
 * Populates global JSON with parsing data as per global options
 */
static inline void to_parsed()
{
	// if alternative parse data source is specified, use that
	if (parsed_src) {
		*parsed_src >> parsed;	
		if (parsed_src->fail()) {
			throw exceptions::system_error("failed to read from input stream");
		}
		return;
	}

	namespace fs = std::filesystem;
	using dir_iterator_t = fs::recursive_directory_iterator;

	std::unordered_set<std::string> processed;

	// iterate over paths specified by user
	fs::path src_path;
	for (std::string& p : file_source_paths) {
		// skip a path if it's already been processed
		src_path = fs::weakly_canonical(std::move(p));
		if (processed.find(src_path) != processed.end()) {
			continue;
		}
		processed.insert(fs::weakly_canonical(src_path));

		// handle by file, directory, or exception (irregular or nonexistent file)
		if (fs::is_regular_file(src_path)) {
			// parse regular files
			*logger << "Parsing " << src_path << '\n';
			parse_file(fs::relative(src_path).c_str(), parsed);
		}
		else if (fs::is_directory(src_path)) {
			// recursively iterate through directory
			for (dir_iterator_t it = dir_iterator_t(src_path); it != dir_iterator_t(); ++it) {
				const fs::directory_entry& entry = *it;

				// skip path within directory if it's already been processed
				if (processed.find(entry.path()) != processed.end()) {
					continue;
				}
				processed.insert(fs::weakly_canonical(entry.path()));

				// skip path if excluded by user
				if (file_source_excludes.find(entry.path()) != file_source_excludes.end()) {
					*logger << "Excluding " << entry.path() << '\n';
					if (entry.is_directory()) {
						it.disable_recursion_pending();
					}
					continue;
				}
				
				// handle by file if regular or not
				if (entry.is_regular_file()) {
					// parse regular file
					*logger << "Parsing " << entry.path() << '\n';
					parse_file(fs::relative(entry.path()).c_str(), parsed);
				}
				else if (!entry.is_directory()) {
					// skip irregular file
					*logger << "Skipping " << entry.path() << " (not regular file)" << '\n';
				}
			}
		}
		else if (!fs::exists(src_path)) {
			throw exceptions::file_exist_error(src_path);
		}
		else {
			throw exceptions::bad_file(src_path);
		}
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
