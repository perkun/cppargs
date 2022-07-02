#include "Args.h"

#include <algorithm>

namespace cppargs {

bool Args::operator[](std::string name)
{
    for (Flag &f : flags)
    {
        if (f == name)
        {
            return f.status;
        }
    }

    return is_option_defined(name) or is_vec_option_defined(name) or
           is_positional_defined(name);
}

bool Args::is_defined(std::string name)
{
    return is_flag_defined(name) or is_option_defined(name) or
           is_vec_option_defined(name) or is_positional_defined(name);
}

bool Args::is_flag_defined(std::string name)
{
    return std::any_of(flags.begin(), flags.end(),
                       [&name](Flag &f) { return f == name; });
}

bool Args::is_option_defined(std::string name)
{
    return std::any_of(options.begin(), options.end(),
                       [&name](Option &opt) { return opt == name; });
}

bool Args::is_vec_option_defined(std::string name)
{
    return std::any_of(vec_options.begin(), vec_options.end(),
                       [&name](VectorOption &opt) { return opt == name; });
}

bool Args::is_positional_defined(std::string name)
{
    return std::any_of(positionals.begin(), positionals.end(),
                       [&name](auto pos) { return pos == name; });
}

}  // namespace cppargs
