#include "stdafx.h"
#include "FileHandler.h"

using boost::filesystem::path;
using boost::filesystem::file_size;
using boost::posix_time::ptime;
using boost::posix_time::from_time_t;

string file_handler(string target)
{
	path p = target;
	string file_info = "File " + p.stem().string();
	file_info += " size " + to_string(file_size(p));

	ptime pt = from_time_t(last_write_time(p));
	file_info += " date " + to_simple_string(pt);

	ifstream file;
	file.open(p.c_str());
	unsigned int checksum(0), shift(0);
	for (unsigned int ch = file.get(); file; ch = file.get())
	{
		checksum += (ch << shift);
		shift += 8;
		if (shift == 32) shift = 0;
	}
	file.close();

	file_info += " checksum " + to_string(checksum) + "\n";

	return file_info;
}