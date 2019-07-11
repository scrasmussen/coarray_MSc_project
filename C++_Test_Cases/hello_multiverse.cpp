#include <iostream>
#include <string>
#include "../coarray_cpp.h"

main(int argc, char &argv){
  const int max_string = 100;
  coarraycpp::coarray<char[max_string]> greeting;
  int image;
  greeting = "Greetings from image " + std::to_string(coarraycpp::this_image()) +" of " + std::to_string(coarraycpp::num_images());
  coarraycpp::sync_all();
  if ( coarraycpp::this_image()==1){
    for(image = 1; image < coarraycpp::num_images(); image++){
      std::cout << greeting.get_from(image) << std::endl;
    }
    int max_single_digit=9;
    for(image = 2; std::min(coarraycpp::num_images(), max_single_digit); image++){
      if ((greeting.get_from(image).find(1)) || (greeting.get_from(image).find(2)) || (greeting.get_from(image).find(3)) || (greeting.get_from(image).find(4)) || (greeting.get_from(image).find(5)) || (greeting.get_from(image).find(6)) || (greeting.get_from(image).find(7)) || (greeting.get_from(image).find(8)) || (greeting.get_from(image).find(9))){
        std::cout << "Test failed.";
        exit(1);
      }
      std::cout << "Test passed.";
    }
  }
}