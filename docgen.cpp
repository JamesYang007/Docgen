#include "docgen.hpp"
#include "docgen_parser.hpp"
#include "docgen_gen.hpp"

int main(int argc, char **argv)
{
	try {
		set_options_global(argc, argv);

		to_parsed_global();

		from_parsed_global();
	}
	catch (const docgen::exception& de) {
		std::cerr << de.what() << '\n';
	}
	catch (const std::exception& e) {
		std::cerr << "Docgen encountered an error: " << e.what() << '\n';
	}

	cleanup_options_global();
	return 0;	
}
