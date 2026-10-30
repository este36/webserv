#include <fstream>
#include <iostream>
#include <string>


int main()
{
	std::ifstream file("config/default.conf");

	if (!file.is_open())
		return (std::cerr << "Error: Can't open config/default.conf file" << std::endl, 1);

	std::string 	line;
	int				line_num = 1;

	while(std::getline(file, line))
	{
		std::cout << line_num << ": " << line << std::endl;
		++line_num;
	}

	file.close();

	return 0;
}
