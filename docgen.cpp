#include "docgen.hpp"
#include "docgen_parser.hpp"
#include "docgen_gen.hpp"

int main(int argc, char **argv)
{
	set_options_global(argc, argv);
	status_check();

	to_parsed_global();
	status_check();

	from_parsed_global();
	status_check();

	cleanup:
		cleanup_options_global();
		status_report();
		return 0;	
}
