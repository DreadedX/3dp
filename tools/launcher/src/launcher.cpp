#include <iostream>

int main() {

	std::cout << "Launcher tool" << std::endl;

	int option = 0; 

	do {

		std::cout << "Input: ";
		std::cin >> option;

		std::cout << "Selected: " << option << std::endl;

	} while (option != 4);

	std::cout << "Exiting" << std::endl;
}
