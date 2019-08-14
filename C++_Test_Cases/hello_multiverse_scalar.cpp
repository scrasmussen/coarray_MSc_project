#include <iostream>
#include <string>
#include "../coarray_cpp.h"
#include "../coarray_cpp.cpp"

int main(int argc, char **argv){
  coarraycpp::coarray<int> greeting;

  int image;
  greeting = coarraycpp::this_image();
  coarraycpp::sync_all();
  if ( coarraycpp::this_image() == 2) std::cout << "Image 2 value" << *(int *)greeting.descriptor.base_addr << std::endl;
  if ( coarraycpp::this_image()==1){
    for(image = 1; image < coarraycpp::num_images() + 1 ; image++){
      std::cout << "I am image " << coarraycpp::this_image() << " and I just received a hello from image " << greeting.get_from(image) << " of " << coarraycpp::num_images() << "! =D" <<std::endl;
    }
    int max_single_digit=9;
    for(image = 2; std::min(coarraycpp::num_images(), max_single_digit); image++){
      if ((greeting.get_from(image) == 1) || (greeting.get_from(image) == 2) || (greeting.get_from(image) == 3) || (greeting.get_from(image) == 4) || (greeting.get_from(image) == 5) || (greeting.get_from(image) == 6) || (greeting.get_from(image) == 7) || (greeting.get_from(image) == 8) || (greeting.get_from(image) == 9)){
        std::cout << "Test failed.";
        exit(1);
      }
      std::cout << "Test passed.";
    }
  }
}