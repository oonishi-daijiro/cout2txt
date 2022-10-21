#include <iostream>
#include <string>
int main(void)
{
  std::string line;
  std::getline(std::cin, line);
  std::cout << line << std::endl;
  system("clip2image");

  return 0;
}
