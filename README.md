# cppargs

Simple library for handling command line arguments in C++.

See `src/main.cpp` for example usage.


# Usage

First, create a `Parser` object

```cpp
Parser parser;
```

Then you can start defining things. To define a description for your program,
just

```cpp
parser.add_description("Your description of a program");
```


## Flags

You can define flags, which have bool values. If it is given as an argument, it
will have `true` value. Otherwise, `false`.

```cpp
void add_flag(char short_name, std::string long_name, std::string description);
void add_flag(std::string long_name, std::string description);
```

## Options with a value

Options have a value. Define them with the following `Parser` methods:

```cpp
void add_option(char short_name, std::string long_name, std::string description, bool requred, std::string default_value);
void add_option(std::string long_name, std::string description, bool requred, std::string default_value);
```

All values are of a type `std::string`. You can convert them to any value after
parsing (see below). If the `required` argument is `true`, then if it is not
given by the user, program will print appropriate message.

## Options with multiple values

These are called *vector options*, and they can store any number of values in a
std::vector.

```cpp
void add_vec_option(char short_name, std::string long_name, std::string description, int num_values, bool requred);
void add_vec_option(std::string long_name, std::string description, int num_values, bool requred);
```

## Positional arguments and list

Everything else is a positional argument. You can also specify them. The purpose
of this is to have them listed in automatically generated help message and being
able to refer to find them by name.

```cpp
void add_positional(std::string long_name, std::string description, int position);
void add_positional_list(std::string long_name, std::string description);
```

A `positional_list` is intended to be a collection of positional arguments
separate from single positional arguments. For example, you can specify the name
of output file as a first positional argument, and then a list of numerous input
files under one name in the help message.

## Parsing

After everything is defined, parse the arguments. As a result, you get the
`Args` object.

```cpp
Args args = parser.parse_args(argc, argv);
```

You can chack if any errors occured during parsing. If so, you can exit the
program, or handle this in any other way.

```cpp
bool errors_occured() { return not is_parsing_successful; }
```

## Getting flags and checking for options' existance

You can ask, whether an option was specified by the user

```cpp
if (args["flag_1"])
	cout << "flag_1 defined" << endl;
```

Both short and long names work. This is the intended way of checking the values
of flags. You can also check for the existence of an option before asking for
its value.

## Getting options' values

To get the values of options, vector options or positional arguments, use these
template methods of the `Args` class object:

```cpp
args.get_value<int>("option_A");
args.get_value<double>("option_B");

/// e.g. xyz coordinates:  --position 1.23 -1.412 5.11
std::vector<double> position = args.get_vec_values<double>("position");

// value of the third positional argument
std::string filename = get_positional<std::string>(3);

// retrieve a vector of all positional arguments
for (std::string s: args.get_all_positionals<std::string>())
		cout << s << endl;
```

## Help

Help flag (`-h`, `--help`) is added automatically. When it is given by the user,
help gets printed and parsing is not performed (returned empty Args). 
You can also manually print help with
`parser.print_help()` method.

# Compile and install

Run `premake5 gmake && make config=release` to compile static library and tests. The binary files are generated under bin folder. If you wish to compilek Shared Library, edit `kind "..."` field in premake5.lua file (see comment above the field).

