#include <iostream>
#include <string>
#include <vector>
#include "Enigma.hxx"

int main(int argc, char **argv) {

    Enigma enigma (C, {Beta, I, IV, VII}, "EAQO", {15, 7, 15, 25}, "mg de ct pi sk qa jo hr ny xl");
    // enigma.debug = 1;
    std::cout << enigma.type(argv[1]) <<  std::endl;
    return 0;
}