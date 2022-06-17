#ifndef ARGUMENT_H_
#define ARGUMENT_H_

#include <regex>
#include <string>
#include <vector>

class OptionBase
{
public:
    OptionBase(){};
    OptionBase(std::string short_name, std::string long_name,
               std::string description)
        : short_name(short_name),
          long_name(long_name),
          description(description)
    {}
    std::string short_name;
    std::string long_name, description;
    int num_values;

    std::regex regex_for_long_name() { return std::regex("--" + long_name); }

    virtual std::regex regex_for_short_name()
    {
        if (short_name != "") return std::regex("-" + short_name);
        return std::regex();
    }

    bool is_cmd_line_item(const std::string &cmd_line_item)
    {
        return regex_match(cmd_line_item, regex_for_long_name()) ||
               regex_match(cmd_line_item, regex_for_short_name());
    }
};

class Flag : public OptionBase
{
public:
    Flag(std::string short_name, std::string long_name, std::string description)
        : OptionBase(short_name, long_name, description)
    {
        num_values = 0;
    }
    bool status = false;

    std::regex regex_for_short_name() override
    {
        if (short_name != "")
            return std::regex("-[a-zA-Z]*" + short_name + "[a-zA-Z]*");
        return std::regex();
    }
};

class Option : public OptionBase
{
public:
    Option(std::string short_name, std::string long_name,
           std::string description, bool required, std::string default_value)
        : required(required),
          value(default_value),
          OptionBase(short_name, long_name, description)

    {
        num_values = 1;
    }
    std::string value;
    bool required;
};

class VectorOption : public OptionBase
{
public:
    VectorOption(std::string short_name, std::string long_name,
                 std::string description, int num_values, bool required)
        : required(required), OptionBase(short_name, long_name, description)
    {
        this->num_values = num_values;
    }

    std::vector<std::string> value_vec;
    bool required;
};

class Positional : public OptionBase
{
public:
    Positional(std::string long_name, std::string value)
    {
        this->value = value;
        this->long_name = long_name;
    }

    std::string value;
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
