#include "CFGReader.h"

CFGReader::CFGReader(std::ifstream* file)
{
	if(file == nullptr)
	{
		return;
	}
	std::string line;
	while(getline(*file, line))
	{
		long index = line.find('=');
		if(index != std::string::npos)
		{
			std::string attribute = line.substr(0, index);
			std::string value = line.substr(index + 1, line.length() - (index + 1));
			if(!pairs.contains(attribute))
			{
				pairs.insert(std::pair<std::string, std::string>(attribute, value));
			}
		}
	}
	file->close();
}

std::string CFGReader::Read(std::string attribute) const
{
	if(pairs.contains(attribute))
	{
		return pairs[attribute];
	}
	return "";
}