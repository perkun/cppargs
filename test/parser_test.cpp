#include "Parser.h"
#include <gtest/gtest.h>

TEST(ParserTest, SingleFlag) {
  Parser parser;
  parser.add_flag('f', "foo", "foo flag");

  std::vector<std::string> cmd_line = {"cppargsTEST", "-f"};

  Args args = parser.parse_args(cmd_line);

  EXPECT_TRUE(args["foo"]);
  EXPECT_TRUE(args["f"]);

  EXPECT_FALSE(args["foobar"]);
  EXPECT_FALSE(args["a"]);
}

TEST(ParserTest, MultipleFlags) {
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
}

TEST(ParserTest, SingleOption) {
  Parser parser;
  parser.add_option('f', "foo", "foo option, not required", false, "42");

  std::vector<std::string> cmd_line = {"cppargsTEST", "-f", "123"};

  Args args = parser.parse_args(cmd_line);

  EXPECT_TRUE(args["foo"]);
  EXPECT_TRUE(args["f"]);

  EXPECT_DOUBLE_EQ(123.0, args.get_value<double>("foo"));

  EXPECT_FALSE(args["foobar"]);
  EXPECT_FALSE(args["a"]);
}

TEST(ParserTest, SingleOptionValueNotGiven) {
  Parser parser;
  parser.add_option('f', "foo", "foo option, not required", false, "42");

  std::vector<std::string> cmd_line = {"cppargsTEST", "-f"};

  Args args = parser.parse_args(cmd_line);

//   ASSERT_EXIT();

//   EXPECT_TRUE(args["foo"]);
//   EXPECT_TRUE(args["f"]);
//
//   EXPECT_DOUBLE_EQ(123.0, args.get_value<double>("foo"));
//
//   EXPECT_FALSE(args["foobar"]);
//   EXPECT_FALSE(args["a"]);
}

// SingleOptionValueNotGiven
// SingleOptionRequiredNotSpecified
