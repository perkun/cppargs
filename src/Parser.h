#ifndef PARSER_H_
#define PARSER_H_

#include <iostream>
#include <stdio.h>
#include <cstring>
#include <string>
#include <vector>
#include <regex>
#include <sstream>
#include "Argument.h"

using namespace std;


class Args
{
friend class Parser;
public:
	bool operator[](string name);  // i flag with true status, or has value
	void print_help() {
		cout << help_message;
	}

	template<typename T>
	T get_value(string name)
	{
		if (!this->operator[](name))
		{
			cout << "Option " << name << " does not exist!" << endl;
			exit(1);
		}

		string value;

		for (Option &opt: options)
		{
			if (opt.long_name == name && opt.value != "")
				value = opt.value;
			if (opt.short_name != "")
				if (opt.short_name == name && opt.value != "")
					value = opt.value;
		}


		T return_value;

		stringstream ss;
		ss << value;
		ss >> return_value;
		return return_value;
	}


	template<typename T>
	vector<T> get_vec_values(string name)
	{
		if (!this->operator[](name))
		{
			cout << "Option " << name << " does not exist!" << endl;
			exit(1);
		}

		vector<string> *values;

		for (VectorOption &opt: vec_options)
		{
			if (opt.long_name == name && opt.value_vec.size() != 0)
				values = &opt.value_vec;
			if (opt.short_name != "")
				if (opt.short_name == name && opt.value_vec.size() != 0)
					values = &opt.value_vec;
		}

		vector<T> return_values;
		for (int i = 0; i < values->size(); i++)
		{
			stringstream ss;
			T tmp;
			ss << values->at(i);
			ss >> tmp;
			return_values.push_back(tmp);
		}

		return return_values;
	}

	template<typename T>
	T get_positional(int position)
	{
		if (position < 0)
		{
			cout << "Positional arguments' ids start from 0" << endl;
			exit(1);
		}
		else if (position >= positionals.size())
		{
			cout << "positional id to big!" << endl;
			exit(1);
		}

		stringstream ss;
		T return_value;
		ss << positionals[position].value;
		ss >> return_value;
		return return_value;
	}

	template<typename T>
	vector<T> get_all_positionals(int start_pos = 0)
	{
		vector<T> return_values;
		for (int i = start_pos; i < positionals.size(); i++)
			return_values.push_back(get_positional<T>(i));
		return return_values;
	}

private:
	string help_message;
	vector<Flag> flags;
	vector<Option> options;
	vector<VectorOption> vec_options;
	vector<Positional> positionals;

};



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

	vector<Flag> parse_flags(int argc, char *argv[]);
	vector<Option> parse_options(int argc, char *argv[]);
	vector<VectorOption> parse_vec_options(int argc, char *argv[]);
	vector<Positional> parse_positional(int argc, char *argv[]);

	void compose_help();
};


#endif /* PARSER_H_ */
