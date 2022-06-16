#include "Parser.h"

using ErrorMessages::print_error;
using std::regex;

Parser::Parser() : is_parsing_successful(true)
{
    user_defined_args.flags.push_back(
        Flag("h", "help", "print this help message"));
}

Parser::~Parser() {}

bool Parser::is_valid(char short_name, const std::string &long_name)
{
    if (user_defined_args.is_defined(std::string(1, short_name)))
    {
        print_error(ErrorMessages::short_name_taken(short_name));
        parsing_failed();
        return false;
    }

    return is_valid(long_name);
}

bool Parser::is_valid(const std::string &long_name)
{
    if (user_defined_args.is_defined(long_name))
    {
        print_error(ErrorMessages::long_name_taken(long_name));
        parsing_failed();
        return false;
    }

    if (long_name.find(' ') != long_name.npos)
    {
        print_error(ErrorMessages::name_with_spaces(long_name));
        parsing_failed();
        return false;
    }

    if (long_name.size() < 2)
    {
        print_error(ErrorMessages::long_name_too_short(long_name));
        parsing_failed();
        return false;
    }

    return true;
}

void Parser::add_flag(char short_name, std::string long_name,
                      std::string description)
{
    if (is_valid(short_name, long_name))
    {
        user_defined_args.flags.emplace_back(
            Flag(std::string(1, short_name), long_name, description));
    }
}

void Parser::add_flag(std::string long_name, std::string description)
{
    if (is_valid(long_name))
    {
        user_defined_args.flags.emplace_back(Flag("", long_name, description));
    }
}

void Parser::add_option(char short_name, std::string long_name,
                        std::string description, bool required,
                        std::string default_value)
{
    if (is_valid(short_name, long_name))
    {
        user_defined_args.options.emplace_back(std::string(1, short_name),
                                               long_name, description, required,
                                               default_value);
    }
}

void Parser::add_option(std::string long_name, std::string description,
                        bool required, std::string default_value)
{
    if (is_valid(long_name))
    {
        user_defined_args.options.emplace_back("", long_name, description,
                                               required, default_value);
    }
}

void Parser::add_vec_option(char short_name, std::string long_name,
                            std::string description, int num_values,
                            bool requred)
{
    if (is_valid(short_name, long_name))
    {
        user_defined_args.vec_options.emplace_back(
            VectorOption(std::string(1, short_name), long_name, description,
                         num_values, requred));
    }
}

void Parser::add_vec_option(std::string long_name, std::string description,
                            int num_values, bool requred)
{
    if (is_valid(long_name))
    {
        user_defined_args.vec_options.emplace_back(
            VectorOption("", long_name, description, num_values, requred));
    }
}

void Parser::add_positional(std::string long_name, std::string description)
{
    if (is_valid(long_name))
    {
        Positional pos(long_name, "");
        pos.description = description;
        user_defined_args.positionals.push_back(pos);
    }
}

void Parser::add_positional_list(std::string long_name, std::string description)
{
    positional_list.long_name = long_name;
    positional_list.description = description;
    positional_list.required = true;
}

Args Parser::parse_args(int argc, char *argv[])
{
    return parse_args(std::vector<std::string>(argv, argv + argc));
}

void Parser::init_occupied_positions(std::vector<std::string> cmd_line)
{
    occupied_positions.assign(cmd_line.size(), false);
}

Args Parser::parse_args(const std::vector<std::string> &cmd_line)
{
    if (errors_occured())
    {
        return Args();
    }

    Args args;

    init_occupied_positions(cmd_line);

    program_name = cmd_line[0];
    args.program_name = cmd_line[0];
    // TODO: extract help to class
    compose_help();

    args.flags = parse_flags(cmd_line);

    if (args["help"])
    {
        print_help();
        // TODO: think about it... If user wants to print help, in program you
        // have to check errors_occured() state...
        parsing_failed();
        return Args();
    }

    args.options = parse_options(cmd_line);
    args.vec_options = parse_vec_options(cmd_line);
    args.positionals = parse_positional(cmd_line);  // has to be last!!

    // TODO: extract to func
    if (positional_list.required &&
        num_positionals <= user_defined_args.positionals.size() + 1)
    {
        print_error(ErrorMessages::list_required(positional_list.long_name));
        parsing_failed();
        return Args();
    }

    if (errors_occured())
    {
        return Args();
    }

    return args;
}

std::vector<Flag> Parser::parse_flags(const std::vector<std::string> &cmd_line)
{
    int argc = cmd_line.size();
    std::vector<Flag> flags;

    for (Flag defined_flag : user_defined_args.flags)
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

std::vector<Option> Parser::parse_options(
    const std::vector<std::string> &cmd_line)
{
    int argc = cmd_line.size();
    std::vector<Option> options;

    for (Option defined_option : user_defined_args.options)
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
                    print_error(
                        ErrorMessages::option_requires_value(option.long_name));
                    parsing_failed();
                    return std::vector<Option>();
                }

                option.value = cmd_line[i + 1];
                found = true;
                occupied_positions[i] = true;
                occupied_positions[i + 1] = true;
            }
        }

        if (option.required && !found)
        {
            print_error(ErrorMessages::option_required(option.long_name));
            parsing_failed();
            return std::vector<Option>();
        }

        options.push_back(option);
    }

    return options;
}

std::vector<VectorOption> Parser::parse_vec_options(
    const std::vector<std::string> &cmd_line)
{
    int argc = cmd_line.size();
    std::vector<VectorOption> vec_options;

    for (VectorOption defined_vec_option : user_defined_args.vec_options)
    {
        VectorOption vec_opt = defined_vec_option;
        bool found = false;

        if (vec_opt.num_values < 2)
        {
            print_error(ErrorMessages::specified_invalid_num_of_values(
                vec_opt.long_name));
            parsing_failed();
            return std::vector<VectorOption>();
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
                bool is_enough_values_given = (i < argc - vec_opt.num_values);

                for (int j = i + 1; j < argc; j++)
                {
                    if (regex_match(cmd_line[j], regex("--?[a-zA-Z]*")))
                    {
                        is_enough_values_given = false;
                    }
                }

                if (not is_enough_values_given)
                {
                    print_error(ErrorMessages::invalid_num_of_values(
                        vec_opt.long_name, vec_opt.num_values));
                    parsing_failed();
                    return std::vector<VectorOption>();
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
            print_error(ErrorMessages::option_required(vec_opt.long_name));
            parsing_failed();
            return std::vector<VectorOption>();
        }

        vec_options.push_back(vec_opt);
    }

    return vec_options;
}

std::vector<Positional> Parser::parse_positional(
    const std::vector<std::string> &cmd_line)
{
    // TODO:
    // + remove specifying position when adding positionals
    // + do not add program name to positionals
    // - simplify search for positionals
    // - validate from parse_positional (here)

    std::vector<Positional> positionals;
    if (user_defined_args.positionals.size() == 0)
    {
        return positionals;
    }

    positionals.reserve(user_defined_args.positionals.size());

    unsigned int current_unprocessed_defined_positional = 0;
    for (int i = 1; i < occupied_positions.size(); i++)
    {
        if (not occupied_positions[i])
        {
            positionals.emplace_back(
                user_defined_args.positionals
                    .at(current_unprocessed_defined_positional)
                    .long_name,
                cmd_line[i]);
            current_unprocessed_defined_positional++;
        }
    }
    num_positionals = positionals.size();

    if (positionals.size() < user_defined_args.positionals.size())
    {
        print_error(ErrorMessages::positional_required(
            user_defined_args.positionals
                .at(current_unprocessed_defined_positional)
                .long_name));
        parsing_failed();
        return std::vector<Positional>();
    }

    return positionals;
}

void Parser::add_description(std::string dsc) { program_description = dsc; }

void Parser::compose_help()
{
    std::stringstream ss;
    ss << std::endl << program_description << std::endl << std::endl;

    ss << "Usage:" << std::endl;
    ss << "\t" << program_name << " [FLAGS] [OPTIONS]";

    for (Option &opt : user_defined_args.options)
    {
        if (!opt.required) continue;
        ss << " --" << opt.long_name << " VALUE";
    }

    for (VectorOption &opt : user_defined_args.vec_options)
    {
        if (!opt.required) continue;
        ss << " --" << opt.long_name << " " << opt.num_values << " VALUES";
    }

    for (int i = 0; i < user_defined_args.positionals.size(); i++)
    {
        ss << " " << user_defined_args.positionals[i].long_name;
    }
    if (positional_list.required)
        ss << " " << positional_list.long_name << "...";
    ss << std::endl << std::endl;

    ss << "FLAGS: " << std::endl;
    for (Flag &f : user_defined_args.flags)
    {
        if (f.short_name == "")
            ss << "    ";
        else
            ss << "-" << f.short_name << ", ";
        ss << "--" << f.long_name << "\t" << f.description << std::endl;
    }

    ss << std::endl << "OPTIONS (required):" << std::endl;
    for (Option &opt : user_defined_args.options)
    {
        if (!opt.required) continue;

        if (opt.short_name == "")
            ss << "    ";
        else
            ss << "-" << opt.short_name << ", ";
        ss << "--" << opt.long_name << " VALUE"
           << "\t" << opt.description << std::endl;
    }

    for (VectorOption &opt : user_defined_args.vec_options)
    {
        if (!opt.required) continue;

        if (opt.short_name == "")
            ss << "    ";
        else
            ss << "-" << opt.short_name << ", ";
        ss << "--" << opt.long_name << " " << opt.num_values << " VALUES"
           << "\t" << opt.description << std::endl;
    }

    ss << std::endl << "OPTIONS:" << std::endl;
    for (Option &opt : user_defined_args.options)
    {
        if (opt.required) continue;

        if (opt.short_name == "")
            ss << "    ";
        else
            ss << "-" << opt.short_name << ", ";
        ss << "--" << opt.long_name << " VALUE"
           << "\t" << opt.description << std::endl;
    }

    for (VectorOption &opt : user_defined_args.vec_options)
    {
        if (opt.required) continue;

        if (opt.short_name == "")
            ss << "    ";
        else
            ss << "-" << opt.short_name << ", ";
        ss << "--" << opt.long_name << " " << opt.num_values << " VALUES"
           << "\t" << opt.description << std::endl;
    }

    ss << std::endl << "POSITIONAL ARGUMENTS:" << std::endl;
    for (Positional &opt : user_defined_args.positionals)
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

void Parser::print_help() { std::cout << help_message; }
