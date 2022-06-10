#ifndef ARGUMENT_H_
#define ARGUMENT_H_

#include <string>
#include <vector>

using namespace std;

class OptionBase
{
public:
    OptionBase(){};
    OptionBase(string short_name, string long_name, string description)
        : short_name(short_name), long_name(long_name), description(description)
    {}
    string short_name;
    string long_name, description;
};

class Flag : public OptionBase
{
public:
    Flag(string short_name, string long_name, string description)
        : OptionBase(short_name, long_name, description)
    {}
    bool status = false;
};

class Option : public OptionBase
{
public:
    Option(string short_name, string long_name, string description,
           bool required, string default_value)
        : required(required),
          value(default_value),
          OptionBase(short_name, long_name, description)

    {}
    string value;
    bool required;
};

class VectorOption : public OptionBase
{
public:
    VectorOption(string short_name, string long_name, string description,
                 int num_values, bool required)
        : num_values(num_values),
          required(required),
          OptionBase(short_name, long_name, description)
    {}

    int num_values;
    vector<string> value_vec;
    bool required;
};

class Positional : public OptionBase
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

class PositionalList : public OptionBase
{
public:
    PositionalList() {}
    int start_pos = 0;
    bool required = false;

private:
};

#endif /* ARGUMENT_H_ */
