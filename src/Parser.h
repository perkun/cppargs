#ifndef PARSER_H_
#define PARSER_H_

#include <stdio.h>
#include <cstring>
#include <regex>
#include "Args.h"

class Parser
{
public:
	Parser();
	~Parser();

	std::string help_message;

	void add_flag(std::string long_name, std::string description);
	void add_flag(char short_name, std::string long_name, std::string description);

	void add_option(char short_name, std::string long_name, std::string description,
			        bool requred, std::string default_value);
	void add_option(std::string long_name, std::string description,
			        bool requred, std::string default_value);

	void add_vec_option(char short_name, std::string long_name, std::string description,
			            int num_values, bool requred);
	void add_vec_option(std::string long_name, std::string description,
			            int num_values, bool requred);

	void add_positional(std::string long_name, std::string description, int position);

	void add_positional_list(std::string long_name, std::string description);
	void add_description(std::string dsc);

	Args parse_args(int argc, char *argv[]);
	Args parse_args(std::vector<std::string> cmd_line);

	void print_help();


private:
	// the ones defined by user
	Args defined_args;
    std::vector<bool> occupied_positions;
	int num_positionals = 0;
	std::string program_description;
	std::string program_name;

	PositionalList positional_list;

	void validate(char short_name, std::string long_name);
	void validate(std::string long_name);

    std::vector<Flag> parse_flags(std::vector<std::string> cmd_line);
	std::vector<Option> parse_options(std::vector<std::string> cmd_line);
    std::vector<Positional> parse_positional(std::vector<std::string> cmd_line);
	std::vector<VectorOption> parse_vec_options(std::vector<std::string> cmd_line);

	void compose_help();
};


#endif /* PARSER_H_ */
