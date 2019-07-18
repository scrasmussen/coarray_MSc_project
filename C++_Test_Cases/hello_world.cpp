#include <iostream>
#include "../coarray_cpp.h"

int main(int argc, char **argv)
{
  int me;

  coarraycpp::caf_init(&argc, &argv);
  me = coarraycpp::this_image();
  std::cout << "I am image " << me << std::endl;
  coarraycpp::caf_finalize();

  std::cout << "Fin" << std::endl;
  return 0;
}