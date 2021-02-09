#include <iostream>
#include <stdio.h>
#include "Parser.h"

using namespace std;

int main(int argc, char *argv[])
{
    Parser parser;

    // description for the program
    parser.add_description(
        "Program for testing the small library for handling arguments");

    // flags
    parser.add_flag('c', "count", "count something awesome");
    parser.add_flag("verbose", "print extra stuff");

    // option with a default value
    parser.add_option('i', "input", "input file name", false, "in.txt");

    // required option, only long name specified
    parser.add_option("output", "output file name", true, "");

    parser.add_vec_option('p', "pos", "specify position's xyz coordinates", 3,
                          false);

    parser.add_positional("name", "Your name", 1);
    parser.add_positional("last", "Youe last name", 2);


    parser.add_positional_list("FILES", "list of files"); // at the end

    // -h, --help flag is added automatically
    // Help is printed when it is given by the user


    // after defining everything, parse args
    Args args = parser.parse_args(argc, argv);

    // check if arument was given
    // a flag
    if (args["count"])
        cout << "You can count on me!" << endl;
    // others too
    if (args["pos"])
        cout << "position specified" << endl;

    string in_filename = args.get_value<string>("input");
    cout << in_filename << endl;

    vector<double> pos = args.get_vec_values<double>("pos");
    for (double d : pos)
        cout << d << ", ";
    cout << endl;

    // get first positional argument
    string name = args.get_positional<string>(1);

    // or by name (if defined earlier)
    string last_name = args.get_positional<string>("last");
    cout << "Last name: " << last_name << endl;

    // or get all of them (including program's name)
    for (string s : args.get_all_positionals<string>())
        cout << s << endl;

    // or all of them starting from position, e.g. get all the FILES
    vector<string> files = args.get_all_positionals<string>(3);

    return 0;
}
