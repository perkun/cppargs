#include "Parser.h"

#include <gtest/gtest.h>

#include "ErrorMessages.h"

TEST(ParserTest, Flag)
{
    Parser parser;
    parser.add_flag('f', "foo", "foo flag");

    std::vector<std::string> cmd_line = {"cppargsTEST", "-f", "some", "other",
                                         "stuff"};

    Args args = parser.parse_args(cmd_line);

    EXPECT_TRUE(args["foo"]);
    EXPECT_TRUE(args["f"]);

    EXPECT_FALSE(args["foobar"]);
    EXPECT_FALSE(args["a"]);

    EXPECT_FALSE(parser.errors_occured());
}

TEST(ParserTest, FlagNotSpecified)
{
    Parser parser;

    std::vector<std::string> cmd_line = {"cppargsTEST", "only", "positional",
                                         "args"};

    Args args = parser.parse_args(cmd_line);

    EXPECT_FALSE(args["f"]);
    // TODO: catch printing of "unknown flag" or sth. To be implemented
    //     EXPECT_FALSE(parser.errors_occured());
}

TEST(ParserTest, FlagShortNameTaken)
{
    testing::internal::CaptureStderr();
    Parser parser;

    parser.add_flag('f', "foo", "foo flag");
    parser.add_flag('f', "bar", "bar flag");

    std::vector<std::string> cmd_line = {"cppargsTEST", "-f"};

    Args args = parser.parse_args(cmd_line);

    std::string captured_error = testing::internal::GetCapturedStderr();

    EXPECT_STREQ(ErrorMessages::short_name_taken('f').c_str(),
                 captured_error.c_str());

    EXPECT_FALSE(args["f"]);
    EXPECT_TRUE(parser.errors_occured());
}

TEST(ParserTest, FlagLongNameTaken)
{
    testing::internal::CaptureStderr();
    Parser parser;

    parser.add_flag('f', "foo", "foo flag");
    parser.add_flag('b', "foo", "bar flag");

    std::vector<std::string> cmd_line = {"cppargsTEST", "-f"};

    Args args = parser.parse_args(cmd_line);

    std::string captured_error = testing::internal::GetCapturedStderr();

    EXPECT_STREQ(ErrorMessages::long_name_taken("foo").c_str(),
                 captured_error.c_str());

    EXPECT_FALSE(args["f"]);
    EXPECT_TRUE(parser.errors_occured());
}

TEST(ParserTest, MultipleFlags)
{
    Parser parser;
    parser.add_flag('f', "foo", "foo flag");
    parser.add_flag('b', "bar", "foo flag");
    parser.add_flag('z', "baz", "foo flag");

    std::vector<std::string> cmd_line = {"cppargsTEST", "-f", "--bar", "--baz",
                                         "-x"};

    Args args = parser.parse_args(cmd_line);

    EXPECT_TRUE(args["foo"]);
    EXPECT_TRUE(args["f"]);

    EXPECT_TRUE(args["bar"]);
    EXPECT_TRUE(args["b"]);

    EXPECT_TRUE(args["baz"]);
    EXPECT_TRUE(args["z"]);

    EXPECT_FALSE(args["foobar"]);
    EXPECT_FALSE(args["a"]);

    EXPECT_FALSE(parser.errors_occured());
}

TEST(ParserTest, MultipleFlagsCombinedShortNames)
{
    Parser parser;
    parser.add_flag('f', "foo", "foo flag");
    parser.add_flag('b', "bar", "foo flag");
    parser.add_flag('z', "baz", "foo flag");

    std::vector<std::string> cmd_line = {"cppargsTEST", "-fbz", "-x"};

    Args args = parser.parse_args(cmd_line);

    EXPECT_TRUE(args["foo"]);
    EXPECT_TRUE(args["f"]);

    EXPECT_TRUE(args["bar"]);
    EXPECT_TRUE(args["b"]);

    EXPECT_TRUE(args["baz"]);
    EXPECT_TRUE(args["z"]);

    EXPECT_FALSE(args["foobar"]);
    EXPECT_FALSE(args["a"]);

    EXPECT_FALSE(parser.errors_occured());
}

TEST(ParserTest, Option)
{
    Parser parser;
    parser.add_option('f', "foo", "foo option, not required", false, "42");

    std::vector<std::string> cmd_line = {"cppargsTEST", "-f", "123"};

    Args args = parser.parse_args(cmd_line);

    EXPECT_TRUE(args["foo"]);
    EXPECT_TRUE(args["f"]);

    EXPECT_DOUBLE_EQ(123.0, args.get_value<double>("foo"));

    EXPECT_FALSE(args["foobar"]);
    EXPECT_FALSE(args["a"]);

    EXPECT_FALSE(parser.errors_occured());
}

TEST(ParserTest, OptionValueNotGiven)
{
    testing::internal::CaptureStderr();
    Parser parser;
    parser.add_option('f', "foo", "foo option, not required", false, "42");

    std::vector<std::string> cmd_line = {"cppargsTEST", "-f"};

    Args args = parser.parse_args(cmd_line);
    std::string captured_error = testing::internal::GetCapturedStderr();

    EXPECT_STREQ(ErrorMessages::option_requires_value("foo").c_str(),
                 captured_error.c_str());
    EXPECT_TRUE(parser.errors_occured());
}

TEST(ParserTest, OptionShortNameTaken)
{
    testing::internal::CaptureStderr();
    Parser parser;
    parser.add_option('f', "foo", "foo option, not required", false, "42");
    parser.add_option('f', "bar", "bar option, not required", false, "123");

    std::vector<std::string> cmd_line = {"cppargsTEST", "-f", "421"};

    Args args = parser.parse_args(cmd_line);
    std::string captured_error = testing::internal::GetCapturedStderr();

    EXPECT_STREQ(ErrorMessages::short_name_taken('f').c_str(),
                 captured_error.c_str());
    EXPECT_TRUE(parser.errors_occured());
}

TEST(ParserTest, OptionLongNameTaken)
{
    testing::internal::CaptureStderr();
    Parser parser;
    parser.add_option('f', "foo", "foo option, not required", false, "42");
    parser.add_option('b', "foo", "bar option, not required", false, "123");

    std::vector<std::string> cmd_line = {"cppargsTEST", "-f", "421"};

    Args args = parser.parse_args(cmd_line);
    std::string captured_error = testing::internal::GetCapturedStderr();

    EXPECT_STREQ(ErrorMessages::long_name_taken("foo").c_str(),
                 captured_error.c_str());
    EXPECT_TRUE(parser.errors_occured());
}

TEST(ParserTest, OptionLongNameTooShort)
{
    testing::internal::CaptureStderr();
    Parser parser;
    parser.add_option('f', "f", "foo option, not required", false, "42");
    parser.add_option('b', "bar", "bar option, not required", false, "123");

    std::vector<std::string> cmd_line = {"cppargsTEST", "-f", "421"};

    Args args = parser.parse_args(cmd_line);
    std::string captured_error = testing::internal::GetCapturedStderr();

    EXPECT_STREQ(ErrorMessages::long_name_too_short("f").c_str(),
                 captured_error.c_str());
    EXPECT_TRUE(parser.errors_occured());
}

TEST(ParserTest, OptionRequiredNotSpecified)
{
    testing::internal::CaptureStderr();
    Parser parser;
    parser.add_option('f', "foo", "foo option, required", true, "42");

    std::vector<std::string> cmd_line = {"cppargsTEST"};

    Args args = parser.parse_args(cmd_line);
    std::string captured_error = testing::internal::GetCapturedStderr();

    EXPECT_STREQ(ErrorMessages::option_required("foo").c_str(),
                 captured_error.c_str());
    EXPECT_TRUE(parser.errors_occured());
}

TEST(ParserTest, VectorOption)
{
    Parser parser;
    parser.add_vec_option('f', "foo", "vector option, not required", 3, false);
    parser.add_flag('b', "bar", "a flag");

    std::vector<std::string> cmd_line = {"cppargsTEST", "-f", "10",
                                         "-20",         "30", "--bar"};

    Args args = parser.parse_args(cmd_line);

    EXPECT_TRUE(args["foo"]);
    EXPECT_TRUE(args["f"]);

    std::vector<double> vec_args = args.get_vec_values<double>("foo");
    EXPECT_DOUBLE_EQ(10.0, vec_args[0]);
    EXPECT_DOUBLE_EQ(-20.0, vec_args[1]);
    EXPECT_DOUBLE_EQ(30.0, vec_args[2]);

    EXPECT_FALSE(args["foobar"]);
    EXPECT_FALSE(args["a"]);

    EXPECT_FALSE(parser.errors_occured());
}

TEST(ParserTest, VectorOptionInvalidNumValues)
{
    testing::internal::CaptureStderr();
    Parser parser;
    parser.add_vec_option('f', "foo", "vector option, not required", 0, false);

    std::vector<std::string> cmd_line = {"cppargsTEST", "-f"};

    Args args = parser.parse_args(cmd_line);
    std::string captured_error = testing::internal::GetCapturedStderr();

    EXPECT_STREQ(ErrorMessages::specified_invalid_num_of_values("foo").c_str(),
                 captured_error.c_str());
    EXPECT_TRUE(parser.errors_occured());
}

TEST(ParserTest, VectorOptionWrongNumOfValuesGivenSurrounded)
{
    testing::internal::CaptureStderr();
    Parser parser;
    parser.add_vec_option('f', "foo", "vector option, not required", 4, false);
    parser.add_flag("bar", "bar flag");

    std::vector<std::string> cmd_line = {"cppargsTEST", "-f", "1",
                                         "2",           "3",  "--bar"};

    Args args = parser.parse_args(cmd_line);
    std::string captured_error = testing::internal::GetCapturedStderr();

    EXPECT_STREQ(ErrorMessages::invalid_num_of_values("foo", 4).c_str(),
                 captured_error.c_str());
    EXPECT_TRUE(parser.errors_occured());
}

TEST(ParserTest, VectorOptionWrongNumOfValuesGiven)
{
    testing::internal::CaptureStderr();
    Parser parser;
    parser.add_vec_option('f', "foo", "vector option, not required", 4, false);
    parser.add_flag("bar", "bar flag");

    std::vector<std::string> cmd_line = {"cppargsTEST", "-f", "1", "2"};

    Args args = parser.parse_args(cmd_line);
    std::string captured_error = testing::internal::GetCapturedStderr();

    EXPECT_STREQ(ErrorMessages::invalid_num_of_values("foo", 4).c_str(),
                 captured_error.c_str());
    EXPECT_TRUE(parser.errors_occured());
}


TEST(ParserTest, VectorOptionRequiredNotGiven)
{
    testing::internal::CaptureStderr();
    Parser parser;
    parser.add_vec_option('f', "foo", "vector option, not required", 4, true);
    parser.add_flag("bar", "bar flag");

    std::vector<std::string> cmd_line = {"cppargsTEST", "--bar"};

    Args args = parser.parse_args(cmd_line);
    std::string captured_error = testing::internal::GetCapturedStderr();

    EXPECT_STREQ(ErrorMessages::option_required("foo").c_str(),
                 captured_error.c_str());
    EXPECT_TRUE(parser.errors_occured());
}

TEST(ParserTest, PositionalArguments)
{
    std::string first_positional = "in.txt";
    std::string second_positional = "out.dat";

    Parser parser;
    parser.add_vec_option('f', "foo", "vector option, not required", 3, true);
    parser.add_flag("bar", "bar flag");
    parser.add_positional("input", "A file name for input");
    parser.add_positional("output", "A file name for output");

    std::vector<std::string> cmd_line = {
        "cppargsTEST", "--bar", first_positional, "-f", "1",
        "2",           "3",     second_positional};

    Args args = parser.parse_args(cmd_line);

    EXPECT_EQ(2, args.num_positionals());

    EXPECT_STREQ(first_positional.c_str(),
                 args.get_positional<std::string>(0).c_str());
    EXPECT_STREQ(second_positional.c_str(),
                 args.get_positional<std::string>(1).c_str());

    std::vector<std::string> positionals =
        args.get_all_positionals<std::string>();

    EXPECT_STREQ(first_positional.c_str(), positionals.at(0).c_str());
    EXPECT_STREQ(second_positional.c_str(), positionals.at(1).c_str());

    EXPECT_STREQ(first_positional.c_str(),
                 args.get_positional<std::string>("input").c_str());
    EXPECT_STREQ(second_positional.c_str(),
                 args.get_positional<std::string>("output").c_str());

    testing::internal::CaptureStderr();
    auto foo = args.get_positional<int>("foo");
    std::string captured_error = testing::internal::GetCapturedStderr();

    EXPECT_STREQ(ErrorMessages::positional_not_given("foo").c_str(),
                 captured_error.c_str());

    EXPECT_FALSE(parser.errors_occured());
}

TEST(ParserTest, PositionalSpecifiedNotGiven)
{
    std::string first_positional = "in.txt";
    std::string second_positional = "out.dat";

    testing::internal::CaptureStderr();

    Parser parser;
    parser.add_vec_option('f', "foo", "vector option, not required", 3, true);
    parser.add_flag("bar", "bar flag");
    parser.add_positional("input", "A file name for input");
    parser.add_positional("output", "A file name for output");

    std::vector<std::string> cmd_line = {
        "cppargsTEST", "--bar", "-f", "1", "2", "3", second_positional};

    Args args = parser.parse_args(cmd_line);

    std::string captured_error = testing::internal::GetCapturedStderr();
    EXPECT_STREQ(ErrorMessages::positional_required("output").c_str(),
                 captured_error.c_str());

    EXPECT_TRUE(parser.errors_occured());
}

// OptionAlreadySpecified
