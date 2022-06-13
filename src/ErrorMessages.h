#pragma once

#include <iostream>
#include <string>

namespace ErrorMessages {

inline void short_name_taken(char short_name)
{
    std::cout << "Option or flag with short name '" << short_name
              << "' is already specified" << std::endl;
}

inline void long_name_taken(std::string long_name)
{
    std::cout << "Option or flag with name '" << long_name
              << "' is already specified" << std::endl;
}

inline void name_with_spaces(std::string long_name)
{
    std::cout << "Names cannot contain spaces (" << long_name << ")"
              << std::endl;
}

inline void positional_required(std::string long_name)
{
    std::cout << "Positional argument " << long_name << " is required!"
              << std::endl;
}

inline void list_required(std::string long_name)
{
    std::cout << "List " << long_name << " is required" << std::endl;
}

inline void option_required(std::string long_name)
{
    std::cout << "Option " << long_name << " is required" << std::endl;
}

inline void option_requires_value(std::string long_name)
{
    std::cout << "Option " << long_name << " requires a value" << std::endl;
}

inline void specified_invalid_num_of_values(std::string long_name)
{
    std::cout
        << "Invalid number of values for option " << long_name
        << ". Specify more than 1 values for vector option, or use option."
        << std::endl;
}

inline void invalid_num_of_values(std::string long_name, int num_values)
{
    std::cout << "Option " << long_name << " requires " << num_values
              << " values" << std::endl;
}

inline void option_not_given(std::string name)
{
    std::cout << "Error getting value. Option " << name << " was not given!"
              << std::endl;
}

inline void positional_not_given(std::string name)
{
    std::cout << "Error getting positional. " << name << " was not given!"
              << std::endl;
}

inline void positionals_start_index()
{
    std::cout << "Positional arguments' ids start from 1" << std::endl;
}

inline void postional_index_too_big()
{
    std::cout << "positional id to big!" << std::endl;
}

}  // namespace ErrorMessages