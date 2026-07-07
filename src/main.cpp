#include "app/first_app.hpp"
#include <cstdlib>
#include <exception>
#include <iostream>

int main() {
  lve::FirstApp app;

  try {
    app.run();
  } catch (std::exception &e) {
    std::cerr << "ERROR: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
