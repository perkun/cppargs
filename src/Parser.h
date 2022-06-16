#ifndef PARSER_H_
#define PARSER_H_

#include <stdio.h>

#include <cstring>
#include <regex>

#include "Args.h"
#include "ErrorMessages.h"

class Parser
{
public:
    Parser();
    ~Parser();

    std::string help_message;

    void add_flag(std::string long_name, std::string description);
    void add_flag(char short_name, std::string long_name,
                  std::string description);

    void add_option(char short_name, std::string long_name,
                    std::string description, bool requred,
                    std::string default_value);
    void add_option(std::string long_name, std::string description,
                    bool requred, std::string default_value);

    void add_vec_option(char short_name, std::string long_name,
                        std::string description, int num_values, bool requred);
    void add_vec_option(std::string long_name, std::string description,
                        int num_values, bool requred);

    void add_positional(std::string long_name, std::string description);

    void add_positional_list(std::string long_name, std::string description);
    void add_description(std::string dsc);

    Args parse_args(int argc, char *argv[]);
    Args parse_args(const std::vector<std::string> &cmd_line);

    bool errors_occured() { return not is_parsing_successful; }

    void print_help();

private:
    bool is_parsing_successful;
    void parsing_failed() { is_parsing_successful = false; }
    Args user_defined_args;
    std::vector<bool> occupied_positions;
    int num_positionals = 0;
    std::string program_description;
    std::string program_name;

    PositionalList positional_list;

    bool is_valid(char short_name, const std::string &long_name);
    bool is_valid(const std::string &long_name);

    std::vector<Flag> parse_flags(const std::vector<std::string> &cmd_line);
    std::vector<Option> parse_options(const std::vector<std::string> &cmd_line);
    std::vector<Positional> parse_positional(const std::vector<std::string> &cmd_line);
    std::vector<VectorOption> parse_vec_options(
        const std::vector<std::string> &cmd_line);
    void compose_help();

    void init_occupied_positions(std::vector<std::string> cmd_line);
};

#endif /* PARSER_H_ */
