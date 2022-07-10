#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "Argument.h"
#include "ErrorMessages.h"
#include "utils.h"

namespace cppargs {

using ErrorMessages::print_error;

class Args
{
    friend class Parser;

public:
    std::string program_name;

    bool operator[](std::string name);  // i flag with true status, or has value

    template <typename T>
    T get_value(std::string name);

    template <typename T>
    std::vector<T> get_vec_values(std::string name);

    template <typename T>
    T get_positional(int position);

    template <typename T>
    T get_positional(std::string name);

    template <typename T>
    std::vector<T> get_all_positionals(int start_pos = 0);

    unsigned long num_positionals() { return positionals.size(); }

private:
    bool is_defined(std::string name);
    template <typename CmdLineArgument>
    bool is_argument_defined(const std::vector<CmdLineArgument> &args,
                             std::string name);

    std::vector<Flag> flags;
    std::vector<Option> options;
    std::vector<VectorOption> vec_options;
    std::vector<Positional> positionals;
};

template <typename T>
T Args::get_value(std::string name)
{
    if (!this->operator[](name))
    {
        print_error(ErrorMessages::option_not_given(name));
        return {};
    }

    std::string value;

    for (Option &opt : options)
    {
        if (opt.has_value() and opt == name)
        {
            value = opt.get_value();
        }
    }
    return utils::convert_value<T>(value);
}

template <typename T>
std::vector<T> Args::get_vec_values(std::string name)
{
    if (not this->operator[](name))
    {
        print_error(ErrorMessages::option_not_given(name));
        return {};
    }

    std::vector<T> return_values;
    for (VectorOption &opt : vec_options)
    {
        if (opt.has_value() and opt == name)
        {
            for (std::string value : opt.get_values())
            {
                return_values.push_back(utils::convert_value<T>(value));
            }
        }
    }
    return return_values;
}

template <typename T>
T Args::get_positional(int position)
{
    return utils::convert_value<T>(positionals.at(position).value);
}

template <typename T>
T Args::get_positional(std::string name)
{
    auto it = std::find_if(positionals.begin(), positionals.end(),
                           [&name](Positional &p) { return p == name; });

    if (it != positionals.end())
    {
        return utils::convert_value<T>(it->value);
    }

    print_error(ErrorMessages::positional_not_given(name));
    return {};
}

template <typename T>
std::vector<T> Args::get_all_positionals(int start_pos)
{
    std::vector<T> return_values;
    return_values.reserve(positionals.size());
    for (int i = start_pos; i < positionals.size(); i++)
    {
        return_values.push_back(get_positional<T>(i));
    }
    return return_values;
}

template <typename CmdLineArgument>
bool Args::is_argument_defined(const std::vector<CmdLineArgument> &args,
                               std::string name)
{
    return std::any_of(args.begin(), args.end(),
                       [&name](const CmdLineArgument &arg) { return arg == name; });
}

}  // namespace cppargs
