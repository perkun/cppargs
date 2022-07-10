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

    return is_argument_defined(options, name) or
           is_argument_defined(vec_options, name) or
           is_argument_defined(positionals, name);
}

bool Args::is_defined(std::string name)
{
    return is_argument_defined(flags, name) or
           is_argument_defined(options, name) or
           is_argument_defined(vec_options, name) or
           is_argument_defined(positionals, name);
}


}  // namespace cppargs
