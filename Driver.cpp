#include "Enigma.hpp"
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char **argv) {
    if (argc!=2) throw std::invalid_argument("Usage: ./Enigma \"String to be typed\"");
    Enigma enigma(B, {Beta, IV, I, II}, {'Z','I','X','O'}, {12,2,22,10}, "ev on qs tg wl xm ui jh pd zb", ignore);
    std::cout << enigma.type(argv[1]) <<  std::endl;
    return 0;
}