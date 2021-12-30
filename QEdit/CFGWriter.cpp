#include "CFGWriter.h"

CFGWriter::CFGWriter(std::ofstream* file)
{
	this->file = file;
}

CFGWriter::~CFGWriter()
{
	if(file)
	{
		file->flush();
		file->close();
	}
}

bool CFGWriter::Write(std::string attribute, std::string value)
{
	if(pairs.contains(attribute))
	{
		return false;
	}
	pairs.insert(std::pair<std::string, std::string>(attribute, value));

	if(file != nullptr)
	{
		(*file) << attribute << "=" << value << std::endl;
	}

	return true;
}
