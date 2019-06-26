#include <iostream>
#include <string>
#include "../coarrays_cpp.h"

main(int *argc, char argv){
  int max_string = 100;
  coarrays_cpp::coarray<char[max_string]> greeting;
  int image;
  greeting = "Greetings from image " + std::to_string(this_image()) +" of " + std::to_string(num_images());
  sync_all();
  if ( this_image()==1){
    for(image = 1; image < num_images(); image++){
      std::cout << greeting[image] << std::endl;
    }
    int expected_location=23,max_single_digit=9;
    for(image = 2; min(num_images(), max_single_digit)){
      if ((greeting[image].find(1)) || (greeting[image].find(2)) || (greeting[image].find(3)) || (greeting[image].find(4)) || (greeting[image].find(5)) || (greeting[image].find(6)) || (greeting[image].find(7)) || (greeting[image].find(8)) || (greeting[image].find(9))){
        std::cout << "Test failed.";
        exit(1);
      }
      std::cout << "Test passed."
    }
  }
}