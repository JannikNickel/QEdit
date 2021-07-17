#pragma once
#include <string>
#include <iostream>

struct ConsoleHelper
{
	static bool YesNoDialog(std::string text, bool def = false)
	{
		std::cout << text << " (y/n)";
		std::string answer;
		std::cin >> answer;
		if(answer == "y")
		{
			return true;
		}
		if(answer == "n")
		{
			return false;
		}
		return def;
	}
};