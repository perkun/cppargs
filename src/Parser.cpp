#include "Parser.h"

using std::regex;

Parser::Parser()
{
    defined_args.flags.push_back(Flag("h", "help", "print this help message"));
}


Parser::~Parser()
{
}

void Parser::validate(char short_name, std::string long_name)
{
    if (defined_args.is_defined(std::string(1, short_name)))
    {
        ErrorMessages::short_name_taken(short_name);
        exit(1);
    }

    validate(long_name);
}


void Parser::validate(std::string long_name)
{
    if (defined_args.is_defined(long_name))
    {
        ErrorMessages::long_name_taken(long_name);
        exit(1);
    }

    if (long_name.find(' ') != long_name.npos)
    {
        ErrorMessages::name_with_spaces(long_name);
        exit(1);
    }
}


void Parser::add_flag(char short_name, std::string long_name, std::string description)
{
    validate(short_name, long_name);
    defined_args.flags.emplace_back(
        Flag(std::string(1, short_name), long_name, description));
}


void Parser::add_flag(std::string long_name, std::string description)
{
    validate(long_name);
    defined_args.flags.emplace_back(Flag("", long_name, description));
}


void Parser::add_option(char short_name, std::string long_name, std::string description,
                        bool required, std::string default_value)
{
    validate(short_name, long_name);
    defined_args.options.emplace_back(std::string(1, short_name), long_name,
                                      description, required, default_value);
}


void Parser::add_option(std::string long_name, std::string description, bool required,
                        std::string default_value)
{
    validate(long_name);
    defined_args.options.emplace_back("", long_name, description, required,
                                      default_value);
}


void Parser::add_vec_option(char short_name, std::string long_name,
                            std::string description, int num_values, bool requred)
{
    validate(short_name, long_name);
    defined_args.vec_options.emplace_back(VectorOption(
        std::string(1, short_name), long_name, description, num_values, requred));
}


void Parser::add_vec_option(std::string long_name, std::string description,
                            int num_values, bool requred)
{
    validate(long_name);
    defined_args.vec_options.emplace_back(
        VectorOption("", long_name, description, num_values, requred));
}


void Parser::add_positional(std::string long_name, std::string description, int position)
{
    validate(long_name);
    Positional pos("", position);
    pos.long_name = long_name;
    pos.description = description;
    pos.required = true;
    defined_args.positionals.push_back(pos);
}


void Parser::add_positional_list(std::string long_name, std::string description)
{
    positional_list.long_name = long_name;
    positional_list.description = description;
    positional_list.required = true;
}

Args Parser::parse_args(int argc, char *argv[])
{
    std::vector<std::string> cmd_line;
    for(int i = 0; i < argc; i++)
    {
        cmd_line.push_back(std::string(argv[i]));
    }
    return parse_args(cmd_line);
}

Args Parser::parse_args(std::vector<std::string> cmd_line)
{
    Args args;

    occupied_positions.reserve(cmd_line.size());
    for (int i = 0; i < cmd_line.size(); i++)
        occupied_positions.emplace_back(false);

    program_name = cmd_line[0];
    args.program_name = cmd_line[0];
    compose_help();

    args.flags = parse_flags(cmd_line);

    if (args["help"])
    {
        print_help();
        exit(1);
    }

    args.options = parse_options(cmd_line);
    args.vec_options = parse_vec_options(cmd_line);
    args.positionals = parse_positional(cmd_line); // has to be last!!

    // check if positionals were given
    for (Positional pos : defined_args.positionals)
    {
        if (pos.required)
            if (pos.position >= args.positionals.size())
            {
                ErrorMessages::positional_required(pos.long_name);
                exit(1);
            }
    }


    if (positional_list.required &&
        num_positionals <= defined_args.positionals.size() + 1)
    {
        ErrorMessages::list_required(positional_list.long_name);
        exit(1);
    }


    return args;
}


std::vector<Flag> Parser::parse_flags(std::vector<std::string> cmd_line)
{
    int argc = cmd_line.size();
    std::vector<Flag> flags;

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

            if (regex_match(cmd_line[i], long_regex) ||
                regex_match(cmd_line[i], short_regex))
            {
                flag.status = true;
                occupied_positions[i] = true;
            }
        }

        flags.push_back(flag);
    }

    return flags;
}


std::vector<Option> Parser::parse_options(std::vector<std::string> cmd_line)
{
    int argc = cmd_line.size();
    std::vector<Option> options;

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

            if (regex_match(cmd_line[i], long_regex) ||
                regex_match(cmd_line[i], short_regex))
            {
                if (i == argc - 1 ||
                    regex_match(cmd_line[i + 1], regex("--?[a-zA-Z]*")))
                {
                    ErrorMessages::option_requires_value(option.long_name);
                    exit(1);
                }

                option.value = cmd_line[i + 1];
                found = true;
                occupied_positions[i] = true;
                occupied_positions[i + 1] = true;
            }
        }

        if (option.required && !found)
        {
            ErrorMessages::option_required(option.long_name);
            exit(1);
        }

        options.push_back(option);
    }

    return options;
}


std::vector<VectorOption> Parser::parse_vec_options(std::vector<std::string> cmd_line)
{
    int argc = cmd_line.size();
    std::vector<VectorOption> vec_options;

    for (VectorOption defined_vec_option : defined_args.vec_options)
    {
        VectorOption vec_opt = defined_vec_option;
        bool found = false;

        if (vec_opt.num_values < 2)
        {
            ErrorMessages::specified_invalid_num_of_values(vec_opt.long_name);
            exit(1);
        }

        regex long_regex = regex("--" + vec_opt.long_name);
        regex short_regex;
        if (defined_vec_option.short_name != "")
            short_regex = regex("-" + vec_opt.short_name);


        for (int i = 1; i < argc; i++)
        {
            if (regex_match(cmd_line[i], long_regex) ||
                regex_match(cmd_line[i], short_regex))
            {
                if (i >= argc - vec_opt.num_values ||
                    regex_match(cmd_line[i + 1], regex("--?[a-zA-Z]*")) ||
                    regex_match(cmd_line[i + 2], regex("--?[a-zA-Z]*")) ||
                    regex_match(cmd_line[i + 3], regex("--?[a-zA-Z]*")))
                {
                    ErrorMessages::invalid_num_of_values(vec_opt.long_name,
                                                         vec_opt.num_values);
                    exit(1);
                }

                occupied_positions[i] = true;
                for (int j = 1; j <= vec_opt.num_values; j++)
                {
                    vec_opt.value_vec.push_back(cmd_line[i + j]);
                    occupied_positions[i + j] = true;
                }

                found = true;
            }
        }

        if (vec_opt.required && !found)
        {
            ErrorMessages::option_required(vec_opt.long_name);
            exit(1);
        }

        vec_options.push_back(vec_opt);
    }

    return vec_options;
}


std::vector<Positional> Parser::parse_positional(std::vector<std::string> cmd_line)
{
    std::vector<Positional> positionals;

    for (int i = 0; i < occupied_positions.size(); i++)
    {
        if (occupied_positions[i] == false)
        {
            Positional p(cmd_line[i], i);

            for (Positional &dp : defined_args.positionals)
                if (dp.position == positionals.size())
                    p.long_name = dp.long_name;

            positionals.push_back(p);
        }
    }
    num_positionals = positionals.size();

    return positionals;
}

void Parser::add_description(std::string dsc)
{
    program_description = dsc;
}


void Parser::compose_help()
{
    std::stringstream ss;
    ss << std::endl << program_description << std::endl << std::endl;

    ss << "Usage:" << std::endl;
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
	if (positional_list.required)
		ss << " " << positional_list.long_name << "...";
    ss << std::endl << std::endl;

    ss << "FLAGS: " << std::endl;
    for (Flag &f : defined_args.flags)
    {
        if (f.short_name == "")
            ss << "    ";
        else
            ss << "-" << f.short_name << ", ";
        ss << "--" << f.long_name << "\t" << f.description << std::endl;
    }

    ss << std::endl << "OPTIONS (required):" << std::endl;
    for (Option &opt : defined_args.options)
    {
        if (!opt.required)
            continue;

        if (opt.short_name == "")
            ss << "    ";
        else
            ss << "-" << opt.short_name << ", ";
        ss << "--" << opt.long_name << " VALUE"
           << "\t" << opt.description << std::endl;
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
           << "\t" << opt.description << std::endl;
    }


    ss << std::endl << "OPTIONS:" << std::endl;
    for (Option &opt : defined_args.options)
    {
        if (opt.required)
            continue;

        if (opt.short_name == "")
            ss << "    ";
        else
            ss << "-" << opt.short_name << ", ";
        ss << "--" << opt.long_name << " VALUE"
           << "\t" << opt.description << std::endl;
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
           << "\t" << opt.description << std::endl;
    }

    ss << std::endl << "POSITIONAL ARGUMENTS:" << std::endl;
    for (Positional &opt : defined_args.positionals)
    {
        ss << opt.long_name << "\t" << opt.description << std::endl;
    }
    ss << std::endl;
	if (positional_list.required)
		ss << positional_list.long_name << "\t" << positional_list.description
			<< std::endl;

    ss << std::endl;

    help_message = ss.str();
}

void Parser::print_help()
{
    std::cout << help_message;
}
