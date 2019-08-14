#include <iostream>
#include "../coarray_cpp.h"

int main(int argc, char **argv)
{
  int me;
  coarraycpp::caf_init(&argc, &argv);
  me = coarraycpp::this_image();
  std::cout << "Hi! I am image " << me << std::endl;
  coarraycpp::sync_all();
  if(coarraycpp::this_image() == 1){
    std::cout << "End" << std::endl;
  }
  coarraycpp::caf_finalize();
  return 0;
}