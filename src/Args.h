#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "Argument.h"

class Args
{
    friend class Parser;

public:
	std::string program_name;

    bool operator[](std::string name); // i flag with true status, or has value

    template <typename T> T get_value(std::string name)
    {
        if (!this->operator[](name))
        {
            std::cout << "Error getting value. Option " << name << " was not given!"
                 << std::endl;
            exit(1);
        }

        std::string value;

        for (Option &opt : options)
        {
            if (opt.long_name == name && opt.value != "")
                value = opt.value;
            if (opt.short_name != "")
                if (opt.short_name == name && opt.value != "")
                    value = opt.value;
        }


        T return_value;

        std::stringstream ss;
        ss << value;
        ss >> return_value;
        return return_value;
    }


    template <typename T> std::vector<T> get_vec_values(std::string name)
    {
        if (!this->operator[](name))
        {
            std::cout << "Error getting value. Option " << name << " was not given!"
                 << std::endl;
            exit(1);
        }

        std::vector<std::string> *values;

        for (VectorOption &opt : vec_options)
        {
            if (opt.long_name == name && opt.value_vec.size() != 0)
                values = &opt.value_vec;
            if (opt.short_name != "")
                if (opt.short_name == name && opt.value_vec.size() != 0)
                    values = &opt.value_vec;
        }

        std::vector<T> return_values;
        for (int i = 0; i < values->size(); i++)
        {
            std::stringstream ss;
            T tmp;
            ss << values->at(i);
            ss >> tmp;
            return_values.push_back(tmp);
        }

        return return_values;
    }

    template <typename T> T get_positional(int position)
    {
        if (position < 1)
        {
            std::cout << "Positional arguments' ids start from 1" << std::endl;
            exit(1);
        }
        else if (position >= positionals.size())
        {
            std::cout << "positional id to big!" << std::endl;
            exit(1);
        }

        std::stringstream ss;
        T return_value;
        ss << positionals[position].value;
        ss >> return_value;
        return return_value;
    }

    template <typename T> T get_positional(std::string name)
    {
        if (!this->operator[](name))
        {
            std::cout << "Error getting positional. " << name << " was not given!"
                 << std::endl;
            exit(1);
        }

		for (Positional &p: positionals)
		{
			if (p.long_name == name)
			{
				std::stringstream ss;
				T return_value;
				ss << p.value;
				ss >> return_value;
				return return_value;
			}
		}
		return "";
    }

    template <typename T> std::vector<T> get_all_positionals(int start_pos = 0)
    {
        std::vector<T> return_values;
        for (int i = start_pos; i < positionals.size(); i++)
            return_values.push_back(get_positional<T>(i));
        return return_values;
    }

	int num_positionals()
	{
		return positionals.size();
	}

private:
	bool is_defined(std::string name);
    std::vector<Flag> flags;
    std::vector<Option> options;
    std::vector<VectorOption> vec_options;
    std::vector<Positional> positionals;
};
