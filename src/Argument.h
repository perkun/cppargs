#ifndef ARGUMENT_H_
#define ARGUMENT_H_

#include <vector>
#include <string>

using namespace std;

class Argument
{
public:
	string short_name;
	string long_name, description;
};


class Flag : public Argument
{
public:
	Flag(string short_name, string long_name, string description)
	{
		this->short_name = short_name;
		this->long_name = long_name;
		this->description = description;
	}
	bool status = false;
};



class Option : public Argument
{
public:
	Option(string short_name, string long_name, string description,
                        bool required, string default_value)
	{
		this->short_name = short_name;
		this->long_name = long_name;
		this->description = description;
		this->required = required;
		this->value = default_value;
	}
	string value;
	bool required;
};



class VectorOption : public Argument
{
public:
	VectorOption(string short_name, string long_name, string description,
                 int num_values, bool required)
	{
		this->short_name = short_name;
		this->long_name = long_name;
		this->description = description;
		this->required = required;
		this->num_values = num_values;
	}

	int num_values;
	vector<string> value_vec;
	bool required;
};

class Positional : public Argument
{
public:
	Positional(string value, int position)
	{
		this->value = value;
		this->position = position;
	}

	string value;
	int position;
	bool required;
};

class PositionalList : public Argument
{
public:
	int start_pos = 0;
	bool required = false;

private:

};


#endif /* ARGUMENT_H_ */
