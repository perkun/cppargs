#include "Parser.h"

using ErrorMessages::print_error;
using std::regex;

Parser::Parser() : is_parsing_successful(true)
{
    user_defined_args.flags.emplace_back("h", "help",
                                         "print this help message");
}

bool Parser::is_name_valid(char short_name, const std::string &long_name)
{
    if (user_defined_args.is_defined(std::string(1, short_name)))
    {
        print_error(ErrorMessages::short_name_taken(short_name));
        parsing_failed();
        return false;
    }

    return is_name_valid(long_name);
}

bool Parser::is_name_valid(const std::string &long_name)
{
    if (user_defined_args.is_defined(long_name))
    {
        print_error(ErrorMessages::long_name_taken(long_name));
        parsing_failed();
        return false;
    }

    if (long_name.find(' ') != std::string::npos)
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
    if (is_name_valid(short_name, long_name))
    {
        user_defined_args.flags.emplace_back(
            Flag(std::string(1, short_name), long_name, description));
    }
}

void Parser::add_flag(std::string long_name, std::string description)
{
    add_flag('\0', long_name, description);
}

void Parser::add_option(char short_name, std::string long_name,
                        std::string description, bool required,
                        std::string default_value)
{
    if (is_name_valid(short_name, long_name))
    {
        user_defined_args.options.emplace_back(std::string(1, short_name),
                                               long_name, description, required,
                                               default_value);
    }
}

void Parser::add_option(std::string long_name, std::string description,
                        bool required, std::string default_value)
{
    add_option('\0', long_name, description, required, default_value);
}

void Parser::add_vec_option(char short_name, std::string long_name,
                            std::string description, int num_values,
                            bool requred)
{
    if (num_values < 2)
    {
        print_error(ErrorMessages::specified_invalid_num_of_values(long_name));
        parsing_failed();
        return;
    }

    if (is_name_valid(short_name, long_name))
    {
        user_defined_args.vec_options.emplace_back(
            VectorOption(std::string(1, short_name), long_name, description,
                         num_values, requred));
    }
}

void Parser::add_vec_option(std::string long_name, std::string description,
                            int num_values, bool requred)
{
    add_vec_option('\0', long_name, description, num_values, requred);
}

void Parser::add_positional(std::string long_name, std::string description)
{
    if (is_name_valid(long_name))
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

Args Parser::parse_args(const std::vector<std::string> &cmd_line)
{
    if (errors_occured())
    {
        return {};
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
        // TODO: think about it... If user wants to print help, then iside a
        // program you have to check errors_occured() state... Is it an error?
        parsing_failed();
        return {};
    }

    args.options = parse_options<Option>(cmd_line, user_defined_args.options);
    args.vec_options =
        parse_options<VectorOption>(cmd_line, user_defined_args.vec_options);
    args.positionals = parse_positional(cmd_line);  // has to be last!!

    // TODO: extract to func
    if (positional_list.required &&
        num_positionals <= user_defined_args.positionals.size() + 1)
    {
        print_error(ErrorMessages::list_required(positional_list.long_name));
        parsing_failed();
        return {};
    }

    if (errors_occured())
    {
        return {};
    }

    return args;
}

void Parser::init_occupied_positions(std::vector<std::string> cmd_line)
{
    occupied_positions.assign(cmd_line.size(), false);
}

std::vector<Flag> Parser::parse_flags(const std::vector<std::string> &cmd_line)
{
    std::vector<Flag> flags;

    for (Flag defined_flag : user_defined_args.flags)
    {
        Flag flag = defined_flag;
        flag.status = false;

        for (int i = 1; i < cmd_line.size(); i++)
        {
            if (flag.is_cmd_line_item(cmd_line[i]))
            {
                flag.status = true;
                occupied_positions[i] = true;
            }
        }
        flags.push_back(flag);
    }
    return flags;
}

template <typename T>
std::vector<T> Parser::parse_options(const std::vector<std::string> &cmd_line,
                                     const std::vector<T> &user_defined_options)
{
    std::vector<T> options;

    for (T defined_option : user_defined_options)
    {
        T option = defined_option;
        bool found = false;
        bool enough_values_given = false;

        extract_option(option, cmd_line, found, enough_values_given);

        if (option.required && not found)
        {
            print_error(ErrorMessages::option_required(option.long_name));
            parsing_failed();
            return {};
        }

        if (found and not enough_values_given)
        {
            print_error(ErrorMessages::invalid_num_of_values(
                option.long_name, option.num_values));
            parsing_failed();
            return {};
        }

        options.push_back(option);
    }
    return options;
}

void Parser::extract_option(OptionBase &option,
                            const std::vector<std::string> &cmd_line,
                            bool &found, bool &enough_values_given)
{
    found = false;
    enough_values_given = false;

    for (int i = 1; i < cmd_line.size(); i++)
    {
        if (option.is_cmd_line_item(cmd_line[i]))
        {
            found = true;

            if (is_num_values_correct(option.num_values, i, cmd_line))
            {
                enough_values_given = true;
            } else
            {
                return;
            }

            occupied_positions.at(i) = true;
            for (int j = 1; j <= option.num_values; j++)
            {
                option.set_value(cmd_line[i + j]);
                occupied_positions[i + j] = true;
            }
        }
    }
}

bool Parser::is_num_values_correct(int defined_num_values, int current_position,
                                   const std::vector<std::string> &cmd_line)
{
    bool is_enough_values_given =
        (current_position < cmd_line.size() - defined_num_values);
    if (is_enough_values_given)
    {
        for (int i = 0; i < defined_num_values; i++)
        {
            if (is_shell_argument(cmd_line[current_position + 1 + i]))
            {
                return false;
            }
        }
    }
    return is_enough_values_given;
}

std::vector<Positional> Parser::parse_positional(
    const std::vector<std::string> &cmd_line)
{
    std::vector<Positional> positionals = collect_positionals(cmd_line);
    if (not are_positionals_valid(positionals))
    {
        return {};
    }
    num_positionals = positionals.size();

    return positionals;
}

std::vector<Positional> Parser::collect_positionals(
    const std::vector<std::string> &cmd_line)
{
    std::vector<Positional> positionals;
    positionals.reserve(user_defined_args.positionals.size());

    unsigned int num_processed_positionals = 0;
    for (int i = 1; i < occupied_positions.size(); i++)
    {
        if (not occupied_positions[i] and (not is_shell_argument(cmd_line[i])))
        {
            std::string long_name =
                num_processed_positionals < user_defined_args.positionals.size()
                    ? user_defined_args.positionals
                          .at(num_processed_positionals)
                          .long_name
                    : "";

            positionals.emplace_back(long_name, cmd_line[i]);
            num_processed_positionals++;
        }
    }
    return positionals;
}

bool Parser::are_positionals_valid(const std::vector<Positional> &positionals)
{
    if (positionals.size() < user_defined_args.positionals.size())
    {
        print_error(ErrorMessages::positional_required(
            user_defined_args.positionals.at(positionals.size()).long_name));
        parsing_failed();
        return false;
    }
    return true;
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
        if (!opt.required)
        {
            continue;
        }
        ss << " --" << opt.long_name << " VALUE";
    }

    for (VectorOption &opt : user_defined_args.vec_options)
    {
        if (!opt.required)
        {
            continue;
        }
        ss << " --" << opt.long_name << " " << opt.num_values << " VALUES";
    }

    for (auto &positional : user_defined_args.positionals)
    {
        ss << " " << positional.long_name;
    }
    if (positional_list.required)
    {
        ss << " " << positional_list.long_name << "...";
    }
    ss << std::endl << std::endl;

    ss << "FLAGS: " << std::endl;
    for (Flag &f : user_defined_args.flags)
    {
        if (f.short_name.empty())
        {
            ss << "    ";
        } else
        {
            ss << "-" << f.short_name << ", ";
        }
        ss << "--" << f.long_name << "\t" << f.description << std::endl;
    }

    ss << std::endl << "OPTIONS (required):" << std::endl;
    for (Option &opt : user_defined_args.options)
    {
        if (!opt.required)
        {
            continue;
        }

        if (opt.short_name.empty())
        {
            ss << "    ";
        } else
        {
            ss << "-" << opt.short_name << ", ";
        }
        ss << "--" << opt.long_name << " VALUE"
           << "\t" << opt.description << std::endl;
    }

    for (VectorOption &opt : user_defined_args.vec_options)
    {
        if (!opt.required)
        {
            continue;
        }

        if (opt.short_name.empty())
        {
            ss << "    ";
        } else
        {
            ss << "-" << opt.short_name << ", ";
        }
        ss << "--" << opt.long_name << " " << opt.num_values << " VALUES"
           << "\t" << opt.description << std::endl;
    }

    ss << std::endl << "OPTIONS:" << std::endl;
    for (Option &opt : user_defined_args.options)
    {
        if (opt.required)
        {
            continue;
        }

        if (opt.short_name.empty())
        {
            ss << "    ";
        } else
        {
            ss << "-" << opt.short_name << ", ";
        }
        ss << "--" << opt.long_name << " VALUE"
           << "\t" << opt.description << std::endl;
    }

    for (VectorOption &opt : user_defined_args.vec_options)
    {
        if (opt.required)
        {
            continue;
        }

        if (opt.short_name.empty())
        {
            ss << "    ";
        } else
        {
            ss << "-" << opt.short_name << ", ";
        }
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
    {
        ss << positional_list.long_name << "\t" << positional_list.description
           << std::endl;
    }

    ss << std::endl;

    help_message = ss.str();
}

void Parser::print_help() const { std::cout << help_message; }
