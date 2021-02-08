#include <iostream>
#include <stdio.h>
#include "Parser.h"

using namespace std;

int main(int argc, char *argv[])
{
	Parser parser;

	parser.add_description("Program for testing the small library for handling arguments");

	parser.add_flag('d', "dupa", "jakaś fajna taka");
	parser.add_flag("cycki", "jakieś takie też fajne");

	parser.add_option("declination", "declination", false, "");

	parser.add_option('f', "file", "input file name", false, "");
	parser.add_option("output", "output file name", true, "out.txt");

	parser.add_vec_option('v', "vector", "positions", 3, false);
	parser.add_vec_option("bla", "positions", 5, true);

	parser.add_positional("Imię", "Twoje Imię, ziom", 1);
	parser.add_positional("Nazwisko", "Twoje nazwisko, ziom", 2);

	parser.add_positional_list("files", "list of files"); // at the end

	Args args = parser.parse_args(argc, argv);


	if (args["help"])
	{
		args.print_help();
		cout << "----------------------------------------" << endl;
	}


	if (args["declination"])
	{
		int dec = args.get_value<int>("declination");
		cout << dec << endl;
	}

	vector<double> pos = args.get_vec_values<double>("v");
	for (double d: pos)
		cout << d << endl;

	cout << "positionals:" << endl;
	for (string s: args.get_all_positionals<string>())
		cout << s << endl;


	return 0;
}

