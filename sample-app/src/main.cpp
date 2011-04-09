#include <iostream>
#include <unistd.h>

#include "MacKeyboardMouseSimulator.h"


int main (int argc, char const *argv[])
{
  mkms::MacKeyboardMouseSimulator simulator;
  
  std::cout << "Simulating mouse move" << std::endl;
  simulator.mouseMove(100, 100);
  
  sleep(1);
  
  std::cout << "Simulating single mouse click" << std::endl;
  simulator.mouseSingleClick(1);
  
  sleep(1);
  
  std::cout << "Simulating key press 'a' " << std::endl;
  simulator.pressOnce(VK_A);
  
  sleep(1);
  
  return 0;
}
