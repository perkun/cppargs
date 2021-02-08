#include "Parser.h"


bool Args::operator[](string name)
{
	for (Flag &f: flags)
	{
		if (f.long_name == name)
			return f.status;
		if (f.short_name != "")
			if (f.short_name == name)
				return f.status;
	}

	for (Option &opt: options)
	{
		if (opt.long_name == name && opt.value != "")
			return true;
		if (opt.short_name != "")
			if (opt.short_name == name && opt.value != "")
				return true;
	}

	for (VectorOption &opt: vec_options)
	{
		if (opt.long_name == name && opt.value_vec.size() != 0)
			return true;
		if (opt.short_name != "")
			if (opt.short_name == name && opt.value_vec.size() != 0)
				return true;
	}

	return false;
}




Parser::Parser()
{
	defined_args.flags.push_back(Flag("h", "help", "print this help message"));
}


Parser::~Parser()
{
}


void Parser::add_flag(char short_name, string long_name, string description)
{
    defined_args.flags.emplace_back(
        Flag(string(1, short_name), long_name, description));
}


void Parser::add_flag(string long_name, string description)
{
    defined_args.flags.emplace_back(Flag("", long_name, description));
}


void Parser::add_option(char short_name, string long_name, string description,
                        bool required, string default_value)
{
    defined_args.options.emplace_back(string(1, short_name), long_name,
                                      description, required, default_value);
}


void Parser::add_option(string long_name, string description, bool required,
                        string default_value)
{
    defined_args.options.emplace_back("", long_name, description, required,
                                      default_value);
}


void Parser::add_vec_option(char short_name, string long_name,
                            string description, int num_values, bool requred)
{
    defined_args.vec_options.emplace_back(VectorOption(
        string(1, short_name), long_name, description, num_values, requred));
}


void Parser::add_vec_option(string long_name, string description,
                            int num_values, bool requred)
{
    defined_args.vec_options.emplace_back(
        VectorOption("", long_name, description, num_values, requred));
}


void Parser::add_positional(string long_name, string description, int position)
{
	Positional pos("", position);
	pos.long_name = long_name;
	pos.description = description;
	pos.required = true;
	defined_args.positionals.push_back(pos);
}


void Parser::add_positional_list(string long_name, string description)
{
	positional_list.long_name = long_name;
	positional_list.description = description;
	positional_list.required = true;
}


Args Parser::parse_args(int argc, char *argv[])
{
    Args args;

    occupied_positions.reserve(argc);
    for (int i = 0; i < argc; i++)
        occupied_positions.emplace_back(false);

	program_name = argv[0];

    args.flags = parse_flags(argc, argv);
    args.options = parse_options(argc, argv);
    args.vec_options = parse_vec_options(argc, argv);
    args.positionals = parse_positional(argc, argv); // has to be last!!

    // check if positionals were given
    for (Positional pos : defined_args.positionals)
    {
        if (pos.required)
            if (pos.position >= args.positionals.size())
            {
                cout << "Positional argument " << pos.long_name << " is required!"
                     << endl;
                exit(1);
            }
    }

	if (num_positionals <= defined_args.positionals.size() + 1)
	{
		cout << "List "<< positional_list.long_name << " is required" << endl;
		exit(1);
	}

	compose_help();
	args.help_message = help_message;

    return args;
}


vector<Flag> Parser::parse_flags(int argc, char *argv[])
{
    vector<Flag> flags;

    for (Flag defined_flag : defined_args.flags)
    {
        Flag flag = defined_flag;
        flag.status = false;

        regex long_regex = regex("--" + flag.long_name);
        regex short_regex;
        if (flag.short_name != "")
            short_regex = regex("-[a-zA-Z]*" + flag.short_name + "[a-zA-Z]*");

        for (int i = 1; i < argc; i++)
        {

            if (regex_match(argv[i], long_regex) ||
                regex_match(argv[i], short_regex))
            {
                flag.status = true;
				occupied_positions[i] = true;
            }
        }

        flags.push_back(flag);
    }

    return flags;
}


vector<Option> Parser::parse_options(int argc, char *argv[])
{
    vector<Option> options;

    for (Option defined_option : defined_args.options)
    {
        Option option = defined_option;
        bool found = false;

        regex long_regex = regex("--" + option.long_name);
        regex short_regex;
        if (option.short_name != "")
            short_regex = regex("-" + option.short_name);

        for (int i = 1; i < argc; i++)
        {

            if (regex_match(argv[i], long_regex) ||
                regex_match(argv[i], short_regex))
            {
                if (i == argc - 1 ||
                    regex_match(argv[i + 1], regex("--?[a-zA-Z]*")))
                {
                    cout << "Option " << option.long_name << " requires a value"
                         << endl;
                    exit(1);
                }

                option.value = argv[i + 1];
                found = true;
				occupied_positions[i] = true;
				occupied_positions[i+1] = true;
            }
        }

        if (option.required && !found)
        {
            cout << "Option " << option.long_name << " is required" << endl;
            exit(1);
        }

        options.push_back(option);
    }

    return options;
}


vector<VectorOption> Parser::parse_vec_options(int argc, char *argv[])
{
    vector<VectorOption> vec_options;

    for (VectorOption defined_vec_option : defined_args.vec_options)
    {
        VectorOption vec_opt = defined_vec_option;
        bool found = false;

        if (vec_opt.num_values < 2)
        {
            cout << "Wrong number of values for option " << vec_opt.long_name
                 << ". Should be 2 or more." << endl;
            exit(1);
        }

        regex long_regex = regex("--" + vec_opt.long_name);
        regex short_regex;
        if (defined_vec_option.short_name != "")
            short_regex = regex("-" + vec_opt.short_name);


        for (int i = 1; i < argc; i++)
        {
            if (regex_match(argv[i], long_regex) ||
                regex_match(argv[i], short_regex))
            {
                if (i >= argc - vec_opt.num_values ||
					regex_match(argv[i + 1], regex("--?[a-zA-Z]*")) ||
					regex_match(argv[i + 2], regex("--?[a-zA-Z]*")) ||
					regex_match(argv[i + 3], regex("--?[a-zA-Z]*")) )
                {
                    cout << "Option " << vec_opt.long_name << " requires "
                         << vec_opt.num_values << " values" << endl;
                    exit(1);
                }

				occupied_positions[i] = true;
				for (int j = 1; j <= vec_opt.num_values; j++)
				{
					vec_opt.value_vec.push_back(argv[i + j]);
					occupied_positions[i+j] = true;
				}

                found = true;

            }
        }

		if (vec_opt.required && !found)
        {
            cout << "Option " << vec_opt.long_name << " is required" << endl;
            exit(1);
        }

		vec_options.push_back(vec_opt);
    }

    return vec_options;
}


vector<Positional> Parser::parse_positional(int argc, char *argv[])
{
	vector<Positional> positionals;

	for (int i = 0; i < occupied_positions.size(); i++)
	{
		if (occupied_positions[i] == false)
		{
			positionals.push_back(Positional(argv[i], i));
		}
	}
	num_positionals = positionals.size();

	return positionals;
}

void Parser::add_description(string dsc)
{
	program_description = dsc;
}


void Parser::compose_help()
{
	stringstream ss;
    ss << endl << program_description << endl << endl;

    ss << "Usage:" << endl;
    ss << "\t" << program_name << " [FLAGS] [OPTIONS]";

    for (Option &opt : defined_args.options)
    {
        if (!opt.required)
            continue;
        ss << " --" << opt.long_name << " VALUE";
    }

    for (VectorOption &opt : defined_args.vec_options)
    {
        if (!opt.required)
            continue;
        ss << " --" << opt.long_name << " " << opt.num_values << " VALUES";
    }


    for (int i = 0; i < defined_args.positionals.size(); i++)
    {
        ss << " " << defined_args.positionals[i].long_name;
    }
    ss << " " << positional_list.long_name << "...";
    ss << endl << endl;

    ss << "FLAGS: " << endl;
    for (Flag &f : defined_args.flags)
    {
        if (f.short_name == "")
            ss << "    ";
        else
            ss << "-" << f.short_name << ", ";
        ss << "--" << f.long_name << "\t" << f.description << endl;
    }

    ss << endl << "OPTIONS (required):" << endl;
    for (Option &opt : defined_args.options)
    {
        if (!opt.required)
            continue;

        if (opt.short_name == "")
            ss << "    ";
        else
            ss << "-" << opt.short_name << ", ";
        ss << "--" << opt.long_name << " VALUE"
             << "\t" << opt.description << endl;
    }

    for (VectorOption &opt : defined_args.vec_options)
    {
        if (!opt.required)
            continue;

        if (opt.short_name == "")
            ss << "    ";
        else
            ss << "-" << opt.short_name << ", ";
        ss << "--" << opt.long_name << " " << opt.num_values << " VALUES"
             << "\t" << opt.description << endl;
    }


    ss << endl << "OPTIONS:" << endl;
    for (Option &opt : defined_args.options)
    {
        if (opt.required)
            continue;

        if (opt.short_name == "")
            ss << "    ";
        else
            ss << "-" << opt.short_name << ", ";
        ss << "--" << opt.long_name << " VALUE"
             << "\t" << opt.description << endl;
    }

    for (VectorOption &opt : defined_args.vec_options)
    {
        if (opt.required)
            continue;

        if (opt.short_name == "")
            ss << "    ";
        else
            ss << "-" << opt.short_name << ", ";
        ss << "--" << opt.long_name << " " << opt.num_values << " VALUES"
             << "\t" << opt.description << endl;
    }

    ss << endl << "POSOTIONAL ARGUMENTS:" << endl;
    for (Positional &opt: defined_args.positionals)
    {
        ss << opt.long_name << "\t" << opt.description << endl;
    }
    ss << endl;
    ss << positional_list.long_name << "\t" << positional_list.description
         << endl;

    ss << endl;

	help_message = ss.str();
}

void Parser::print_help()
{
	cout << help_message;
}

