#ifndef PARSER_H_
#define PARSER_H_

#include <stdio.h>
#include <cstring>
#include <regex>
#include "Args.h"

using namespace std;

class Parser
{
public:
	Parser();
	~Parser();

	string help_message;

	void add_flag(string long_name, string description);
	void add_flag(char short_name, string long_name, string description);

	void add_option(char short_name, string long_name, string description,
			        bool requred, string default_value);
	void add_option(string long_name, string description,
			        bool requred, string default_value);

	void add_vec_option(char short_name, string long_name, string description,
			            int num_values, bool requred);
	void add_vec_option(string long_name, string description,
			            int num_values, bool requred);

	void add_positional(string long_name, string description, int position);

	void add_positional_list(string long_name, string description);
	void add_description(string dsc);

	Args parse_args(int argc, char *argv[]);

	void print_help();


private:
	// the ones defined by user
	Args defined_args;
	vector<bool> occupied_positions;
	int num_positionals = 0;
	string program_description;
	string program_name;

	PositionalList positional_list;

	void validate(char short_name, string long_name);
	void validate(string long_name);

	vector<Flag> parse_flags(int argc, char *argv[]);
	vector<Option> parse_options(int argc, char *argv[]);
	vector<VectorOption> parse_vec_options(int argc, char *argv[]);
	vector<Positional> parse_positional(int argc, char *argv[]);

	void compose_help();
};


#endif /* PARSER_H_ */
