#include <iostream>
#include <string>
#include "../coarray_cpp.h"

int main(int argc, char **argv){
  coarraycpp::coarray<char[3]> greeting();
  std::cout << "Succesfully created a new coarray" << std::endl;
}
