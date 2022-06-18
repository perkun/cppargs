#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "Argument.h"
#include "ErrorMessages.h"
#include "utils.h"

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
    std::vector<Flag> flags;
    std::vector<Option> options;
    std::vector<VectorOption> vec_options;
    std::vector<Positional> positionals;

//     Flag find_flag(std::string name);
    bool is_flag_defined(std::string name);
    bool is_option_defined(std::string name);
    bool is_vec_option_defined(std::string name);
    bool is_positional_defined(std::string name);
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
        if (opt.long_name == name && not opt.value.empty())
        {
            value = opt.value;
        }
        if (not opt.short_name.empty())
        {
            if (opt.short_name == name && not opt.value.empty())
            {
                value = opt.value;
            }
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
        if ((opt.long_name == name && not opt.value_vec.empty()) or
            ((not opt.short_name.empty()) and
             (opt.short_name == name && not opt.value_vec.empty())))
        {
            for (std::string value : opt.value_vec)
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
    for (Positional &p : positionals)
    {
        if (p.long_name == name)
        {
            return utils::convert_value<T>(p.value);
        }
    }

    print_error(ErrorMessages::positional_not_given(name));
    return T();
}

template <typename T>
std::vector<T> Args::get_all_positionals(int start_pos)
{
    std::vector<T> return_values;
    for (int i = start_pos; i < positionals.size(); i++)
    {
        return_values.push_back(get_positional<T>(i));
    }
    return return_values;
}
