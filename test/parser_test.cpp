#include <gtest/gtest.h>
#include "Parser.h"

TEST(ParserTest, Flags)
{
    Parser parser;
    parser.add_flag('f', "foo", "foo flag");

    EXPECT_EQ(1, 2);
}
