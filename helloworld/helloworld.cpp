#include <iostream>

extern "C" {
  void _gfortran_caf_init(int *argc, char ***argv);
  void _gfortran_caf_finalize();
  int  _gfortran_caf_this_image();
}

int main(int argc, char **argv)
{
  int me;

  _gfortran_caf_init(&argc, &argv);
  me = _gfortran_caf_this_image();
  std::cout << "I am image " << me << std::endl;
  _gfortran_caf_finalize();

  std::cout << "Fin" << std::endl;
  return 0;
}
