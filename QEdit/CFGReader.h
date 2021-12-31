#pragma once
#include <string>
#include <map>
#include <fstream>
#include <filesystem>

class CFGReader
{
private:
	std::map<std::string, std::string> pairs;

public:
	CFGReader(std::ifstream* file);
	std::string Read(std::string attribute) const;
};

