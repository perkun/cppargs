#pragma once

#include <cstdio>
#include <cstring>
#include <regex>

#include "Args.h"
#include "ErrorMessages.h"

namespace cppargs {

class Parser
{
public:
    Parser();
    ~Parser() = default;

    std::string help_message;

    void add_flag(std::string long_name, std::string description);
    void add_flag(char short_name, std::string long_name,
                  std::string description);

    void add_option(char short_name, std::string long_name,
                    std::string description, bool required,
                    std::string default_value);
    void add_option(std::string long_name, std::string description,
                    bool required, std::string default_value);

    void add_vec_option(char short_name, std::string long_name,
                        std::string description, int num_values, bool requred);
    void add_vec_option(std::string long_name, std::string description,
                        int num_values, bool requred);

    void add_positional(std::string long_name, std::string description);

    void add_positional_list(std::string long_name, std::string description);
    void add_description(std::string dsc);

    Args parse_args(int argc, char *argv[]);
    Args parse_args(const std::vector<std::string> &cmd_line);

    bool errors_occured() const { return not is_parsing_successful; }

    void print_help() const;

private:
    bool is_parsing_successful;
    void parsing_failed() { is_parsing_successful = false; }
    Args user_defined_args;
    std::vector<bool> occupied_positions;
    unsigned long num_positionals = 0;
    std::string program_description;
    std::string program_name;

    PositionalList positional_list;

    bool is_name_valid(char short_name, const std::string &long_name);
    bool is_name_valid(const std::string &long_name);

    std::vector<Flag> parse_flags(const std::vector<std::string> &cmd_line);

    template <typename T>
    std::vector<T> parse_options(const std::vector<std::string> &cmd_line,
                                 const std::vector<T> &user_defined_options);
    std::vector<Positional> parse_positional(
        const std::vector<std::string> &cmd_line);

    void extract_option(const std::vector<std::string> &cmd_line,
                        OptionBase &option, bool &found,
                        bool &enough_values_given);

    void compose_help();

    static bool is_shell_argument(const std::string &str)
    {
        return regex_match(str, std::regex("--?[a-zA-Z]*"));
    }

    void init_occupied_positions(std::vector<std::string> cmd_line);
    std::vector<Positional> collect_positionals(
        const std::vector<std::string> &cmd_line);
    bool are_positionals_valid(const std::vector<Positional> &positionals);
    static bool is_num_values_correct(int defined_num_values,
                                      int current_position,
                                      const std::vector<std::string> &cmd_line);
};

}  // namespace cppargs
