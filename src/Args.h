#include <iostream>
#include <sstream>
#include "Argument.h"

using namespace std;

class Args
{
    friend class Parser;

public:
	string program_name;

    bool operator[](string name); // i flag with true status, or has value

    template <typename T> T get_value(string name)
    {
        if (!this->operator[](name))
        {
            cout << "Error getting value. Option " << name << " was not given!"
                 << endl;
            exit(1);
        }

        string value;

        for (Option &opt : options)
        {
            if (opt.long_name == name && opt.value != "")
                value = opt.value;
            if (opt.short_name != "")
                if (opt.short_name == name && opt.value != "")
                    value = opt.value;
        }


        T return_value;

        stringstream ss;
        ss << value;
        ss >> return_value;
        return return_value;
    }


    template <typename T> vector<T> get_vec_values(string name)
    {
        if (!this->operator[](name))
        {
            cout << "Error getting value. Option " << name << " was not given!"
                 << endl;
            exit(1);
        }

        vector<string> *values;

        for (VectorOption &opt : vec_options)
        {
            if (opt.long_name == name && opt.value_vec.size() != 0)
                values = &opt.value_vec;
            if (opt.short_name != "")
                if (opt.short_name == name && opt.value_vec.size() != 0)
                    values = &opt.value_vec;
        }

        vector<T> return_values;
        for (int i = 0; i < values->size(); i++)
        {
            stringstream ss;
            T tmp;
            ss << values->at(i);
            ss >> tmp;
            return_values.push_back(tmp);
        }

        return return_values;
    }

    template <typename T> T get_positional(int position)
    {
        if (position < 0)
        {
            cout << "Positional arguments' ids start from 0" << endl;
            exit(1);
        }
        else if (position >= positionals.size())
        {
            cout << "positional id to big!" << endl;
            exit(1);
        }

        stringstream ss;
        T return_value;
        ss << positionals[position].value;
        ss >> return_value;
        return return_value;
    }

    template <typename T> T get_positional(string name)
    {
        if (!this->operator[](name))
        {
            cout << "Error getting positional. " << name << " was not given!"
                 << endl;
            exit(1);
        }

		for (Positional &p: positionals)
		{
			if (p.long_name == name)
			{
				stringstream ss;
				T return_value;
				ss << p.value;
				ss >> return_value;
				return return_value;
			}
		}
		return "";
    }

    template <typename T> vector<T> get_all_positionals(int start_pos = 0)
    {
        vector<T> return_values;
        for (int i = start_pos; i < positionals.size(); i++)
            return_values.push_back(get_positional<T>(i));
        return return_values;
    }

private:
	bool is_defined(string name);
    vector<Flag> flags;
    vector<Option> options;
    vector<VectorOption> vec_options;
    vector<Positional> positionals;
};
