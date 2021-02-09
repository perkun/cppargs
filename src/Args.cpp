#include "Args.h"


bool Args::operator[](string name)
{
	for (Flag &f: flags)
	{
		if (f.long_name == name)
			return f.status;
		if (f.short_name != "")
			if (f.short_name == name)
				return f.status;
	}

	for (Option &opt: options)
	{
		if (opt.long_name == name && opt.value != "")
			return true;
		if (opt.short_name != "")
			if (opt.short_name == name && opt.value != "")
				return true;
	}

	for (VectorOption &opt: vec_options)
	{
		if (opt.long_name == name && opt.value_vec.size() != 0)
			return true;
		if (opt.short_name != "")
			if (opt.short_name == name && opt.value_vec.size() != 0)
				return true;
	}

	for (Positional &p: positionals)
	{
		if (p.long_name == name)
			return true;
	}

	return false;
}




bool Args::is_defined(string name)
{
	for (Flag &f: flags)
	{
		if (f.long_name == name)
			return true;
		if (f.short_name != "")
			if (f.short_name == name)
				return true;
	}

	for (Option &opt: options)
	{
		if (opt.long_name == name && opt.value != "")
			return true;
		if (opt.short_name != "")
			if (opt.short_name == name && opt.value != "")
				return true;
	}

	for (VectorOption &opt: vec_options)
	{
		if (opt.long_name == name && opt.value_vec.size() != 0)
			return true;
		if (opt.short_name != "")
			if (opt.short_name == name && opt.value_vec.size() != 0)
				return true;
	}

	for (Positional &p: positionals)
	{
		if (p.long_name == name)
			return true;
	}


	return false;
}
