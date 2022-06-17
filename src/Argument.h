#ifndef ARGUMENT_H_
#define ARGUMENT_H_

#include <regex>
#include <string>
#include <vector>

class CmdLineArgumentBase
{
public:
    CmdLineArgumentBase(){};
    CmdLineArgumentBase(std::string short_name, std::string long_name,
                        std::string description)
        : short_name(short_name), long_name(long_name), description(description)
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

class Flag : public CmdLineArgumentBase
{
public:
    Flag(std::string short_name, std::string long_name, std::string description)
        : CmdLineArgumentBase(short_name, long_name, description)
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

class OptionBase : public CmdLineArgumentBase
{
public:
    OptionBase(std::string short_name, std::string long_name,
               std::string description, bool required)
        : required(required),
          CmdLineArgumentBase(short_name, long_name, description)
    {}
    bool required;
    virtual void set_value(std::string val) = 0;
};

class Option : public OptionBase
{
public:
    Option(std::string short_name, std::string long_name,
           std::string description, bool required, std::string default_value)
        : value(default_value),
          OptionBase(short_name, long_name, description, required)

    {
        num_values = 1;
    }
    std::string value;

    void set_value(std::string val)
    {
        value = val;
    }
};

class VectorOption : public OptionBase
{
public:
    VectorOption(std::string short_name, std::string long_name,
                 std::string description, int num_values, bool required)
        : OptionBase(short_name, long_name, description, required)
    {
        this->num_values = num_values;
    }

    std::vector<std::string> value_vec;

    void set_value(std::string val)
    {
        value_vec.push_back(val);
    }
private:

};

class Positional : public CmdLineArgumentBase
{
public:
    Positional(std::string long_name, std::string value)
    {
        this->value = value;
        this->long_name = long_name;
    }

    std::string value;
};

class PositionalList : public CmdLineArgumentBase
{
public:
    PositionalList() {}
    int start_pos = 0;
    bool required = false;

private:
};

#endif /* ARGUMENT_H_ */
