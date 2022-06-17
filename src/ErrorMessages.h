#pragma once

#include <iostream>
#include <string>

namespace ErrorMessages {

inline void print_error(std::string msg) { std::cerr << msg; }

inline std::string short_name_taken(char short_name)
{
    return "Option or flag with short name '" + std::string(1, short_name) +
           "' is already specified\n";
}

inline std::string long_name_taken(std::string long_name)
{
    return "Option or flag with name '" + long_name +
           "' is already specified\n";
}

inline std::string long_name_too_short(std::string long_name)
{
    return "Long names '" + long_name + "' is too short.\n";
}

inline std::string name_with_spaces(std::string long_name)
{
    return "Names cannot contain spaces (" + long_name + ")\n";
}

inline std::string positional_required(std::string long_name)
{
    return "Positional argument " + long_name + " is required!\n";
}

inline std::string list_required(std::string long_name)
{
    return "List " + long_name + " is required\n";
}

inline std::string option_required(std::string long_name)
{
    return "Option " + long_name + " is required\n";
}

inline std::string specified_invalid_num_of_values(std::string long_name)
{
    return "Invalid number of values for option " + long_name +
           ". Specify more than 1 values for vector option, or use option.\n";
}

inline std::string invalid_num_of_values(std::string long_name, int num_values)
{
    return "Option " + long_name + " requires " + std::to_string(num_values) +
           " value(s)\n";
}

inline std::string option_not_given(std::string name)
{
    return "Error getting value. Option " + name + " was not given!\n";
}

inline std::string positional_not_given(std::string name)
{
    return "Error getting positional. " + name + " was not given!\n";
}

inline std::string positionals_start_index()
{
    return "Positional arguments' ids start from 1\n";
}

inline std::string postional_index_too_big()
{
    return "positional id to big!\n";
}

}  // namespace ErrorMessages
