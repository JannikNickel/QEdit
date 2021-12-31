#pragma once
#include <string>
#include <map>
#include <fstream>
#include <filesystem>

class CFGWriter
{
private:
	std::ofstream* file;
	std::map<std::string, std::string> pairs;

public:
	CFGWriter(std::ofstream* file);
	~CFGWriter();

	bool Write(std::string attribute, std::string value);
};

