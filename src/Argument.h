#pragma once

#include <regex>
#include <string>
#include <utility>
#include <vector>

namespace cppargs {
class CmdLineArgumentBase
{
public:
    CmdLineArgumentBase() = default;
    CmdLineArgumentBase(std::string short_name, std::string long_name,
                        std::string description)
        : short_name(std::move(short_name)), long_name(std::move(long_name)), description(std::move(description))
    {}
    std::string short_name;
    std::string long_name, description;
    int num_values = 0;

    bool is_cmd_line_item(const std::string &cmd_line_item)
    {
        return regex_match(cmd_line_item, regex_for_long_name()) ||
               regex_match(cmd_line_item, regex_for_short_name());
    }

    bool operator==(std::string name) const
    {
        return (long_name == name or
                (not short_name.empty() and short_name == name));
    }

protected:
    std::regex regex_for_long_name() const
    {
        return std::regex("--" + long_name);
    }

    virtual std::regex regex_for_short_name()
    {
        if (not short_name.empty())
        {
            return std::regex("-" + short_name);
        }
        return {};
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
        if (not short_name.empty())
        {
            return std::regex("-[a-zA-Z]*" + short_name + "[a-zA-Z]*");
        }
        return {};
    }
};

class OptionBase : public CmdLineArgumentBase
{
public:
    OptionBase(std::string short_name, std::string long_name,
               std::string description, bool required)
        : CmdLineArgumentBase(short_name, long_name, description),
          required(required)
    {}
    bool required;
    virtual void set_value(std::string val) = 0;
    virtual bool has_value() = 0;
};

class Option : public OptionBase
{
public:
    Option(std::string short_name, std::string long_name,
           std::string description, bool required, std::string default_value)
        : OptionBase(short_name, long_name, description, required),
          value(default_value)

    {
        num_values = 1;
    }

    std::string get_value() { return value; }
    void set_value(std::string val) override { value = val; }
    bool has_value() override { return not value.empty(); }

private:
    std::string value;
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

    std::vector<std::string> get_values() { return value_vec; }
    void set_value(std::string val) override { value_vec.push_back(val); }
    bool has_value() override { return not value_vec.empty(); }

private:
    std::vector<std::string> value_vec;
};

class Positional : public CmdLineArgumentBase
{
public:
    Positional(std::string long_name, std::string value) : value(value)
    {
        this->long_name = long_name;
    }

    std::string value;
};

class PositionalList : public CmdLineArgumentBase
{
public:
    PositionalList() = default;
    int start_pos = 0;
    bool required = false;

private:
};

}  // namespace cppargs
