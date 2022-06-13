#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "Argument.h"
#include "utils.h"
#include "ErrorMessages.h"

class Args
{
    friend class Parser;

public:
    std::string program_name;

    bool operator[](std::string name);  // i flag with true status, or has value

    template <typename T>
    T get_value(std::string name)
    {
        if (!this->operator[](name)) {
            ErrorMessages::option_not_given(name);
            exit(1);
        }

        std::string value;

        for (Option &opt : options) {
            if (opt.long_name == name && opt.value != "") value = opt.value;
            if (opt.short_name != "")
                if (opt.short_name == name && opt.value != "")
                    value = opt.value;
        }
        return utils::convert_value<T>(value);
    }

    template <typename T>
    std::vector<T> get_vec_values(std::string name)
    {
        if (!this->operator[](name)) {
            ErrorMessages::option_not_given(name);
            exit(1);
        }

        std::vector<T> return_values;
        for (VectorOption &opt : vec_options)
        {
            if ((opt.long_name == name && opt.value_vec.size() != 0) or
                ((opt.short_name != "") and
                 (opt.short_name == name && opt.value_vec.size() != 0))) {
                for (std::string value : opt.value_vec)
                {
                    return_values.push_back(utils::convert_value<T>(value));
                }
            }
        }
        return return_values;
    }

    template <typename T>
    T get_positional(int position)
    {
        if (position < 1) {
            ErrorMessages::positionals_start_index();
            exit(1);
        } else if (position >= positionals.size()) {
            ErrorMessages::postional_index_too_big();
            exit(1);
        }

        return utils::convert_value<T>(positionals[position].value);
    }

    template <typename T>
    T get_positional(std::string name)
    {
        if (!this->operator[](name)) {
            ErrorMessages::positional_not_given(name);
            exit(1);
        }

        for (Positional &p : positionals) {
            if (p.long_name == name) {
                return utils::convert_value<T>(p.value);
            }
        }
        return "";
    }

    template <typename T>
    std::vector<T> get_all_positionals(int start_pos = 0)
    {
        std::vector<T> return_values;
        for (int i = start_pos; i < positionals.size(); i++)
            return_values.push_back(get_positional<T>(i));
        return return_values;
    }

    int num_positionals() { return positionals.size(); }

private:
    bool is_defined(std::string name);
    std::vector<Flag> flags;
    std::vector<Option> options;
    std::vector<VectorOption> vec_options;
    std::vector<Positional> positionals;
};
