#include "Enigma.hpp"

// General use functions

// Converting from letters to numbers (a=0, b=1, ..., z=25)
int Enigma::convert(char c) {
    return (int)toupper(c) - 'A';
}


// Converting from numbers to letters (0=a, 1=b, ..., 25=z)
char Enigma::convert(int n) {
    return (char)n + 'A';
}


// Functions to intitialise Enigma settings

// Takes in reflector number and sets map for the reflector
void Enigma::setReflector(int ref) {
    const std::vector<std::vector<int>> reflector_templates =  {{4,13,10,16,0,20,24,22,9,8,2,14,15,1,11,12,3,23,25,21,5,19,7,17,6,18},
                                                                {17,3,14,1,9,13,19,10,21,4,7,12,11,5,2,22,25,0,23,6,24,8,15,18,20,16}};
    for (int i=0; i<26; i++)
        reflector[i] = reflector_templates[ref][i];
}


// Takes in vector of rotor numbers and adds the vector representing that rotor to the rotors vector
void Enigma::setRotors(std::vector<Rotor> xrotors) {
    const std::vector<std::vector<int>> rotor_templates =  {{4,10,12,5,11,6,3,16,21,25,13,19,14,22,24,7,23,20,18,15,0,8,1,17,2,9},
                                                            {0,9,3,10,18,8,17,20,23,1,11,7,22,19,12,2,16,6,25,13,15,24,5,21,14,4},
                                                            {1,3,5,7,9,11,2,15,17,19,23,21,25,13,24,4,8,22,6,0,10,12,20,18,16,14},
                                                            {4,18,14,21,15,25,9,0,24,16,20,8,17,7,23,11,13,5,19,6,10,3,2,12,22,1},
                                                            {21,25,1,17,6,8,19,24,20,15,18,3,13,7,11,23,0,22,12,9,16,14,5,4,2,10},
                                                            {9,15,6,21,14,20,12,5,24,16,1,4,13,7,25,17,3,10,0,18,23,11,8,2,19,22},
                                                            {13,25,9,7,6,17,2,23,12,24,18,22,1,14,20,5,0,8,21,11,15,4,10,16,3,19},
                                                            {5,10,16,7,19,11,23,14,2,1,9,18,15,3,25,17,0,12,4,22,13,8,20,24,6,21},
                                                            {11,4,24,9,21,2,13,8,23,22,15,1,16,12,3,17,19,0,10,25,6,5,20,7,14,18},
                                                            {5,18,14,10,0,13,20,4,17,7,12,1,19,8,24,2,22,11,16,15,25,23,21,6,9,3}};
    const std::vector<int> turnovers = {16,4,21,9,25};
    for (int i=0; i<4; i++) {
        rotors.push_back(rotor_templates[xrotors[i]]);
        if (xrotors[i]>7 || xrotors[i]<5) notches.push_back(turnovers[xrotors[i]]);
        else notches.push_back(50);
    }
}


// Takes in string ("XX XX XX...") of switchboard settings and adds them to the switchboard map
void Enigma::setSwitches(std::string switches) {
    switchBoard.clear();
    std::stringstream ss (switches);
    std::string pair;
    while (ss >> pair) { 
        switchBoard[convert(pair[0])] = convert(pair[1]);
        switchBoard[convert(pair[1])] = convert(pair[0]);
    }
}


// Functions for typing each character

// Rotates the rotors in the standard way
void Enigma::rotate() {
    int trigger = 0;
    if (positions[3]==notches[3] || (notches[3]==50 && (positions[3]==25 || positions[3]==12)))
        trigger = 1;
    positions[3] = (positions[3] + 1) % 26;
    // if rotor 2 was on turnover
    if (trigger) {
        trigger = 0;
        if (positions[2]==notches[2] || (notches[2]==50 && (positions[2]==25 || positions[2]==12)))
            trigger = 1;
        positions[2] = (positions[2] + 1) % 26;
    }
    // check for double stepping
    else if (positions[2]==notches[2] || (notches[2]==50 && (positions[2]==25 || positions[2]==12))) {
        positions[2] = (positions[2] + 1) % 26;
        positions[1] = (positions[1] + 1) % 26;
    }
    // if rotor 1 was on turnover
    if (trigger)
        positions[1] = (positions[1] + 1) % 26;
}


// Passes a letter (in the form of a converted number) through a specific wheel forwards
void Enigma::typeF(int rotor, int &n) {
    n = rotors[rotor][ (n+positions[rotor])%26 ];
    n = (n - positions[rotor] + 26)%26;
}

// Passes a letter (in the form of a converted number) through a specific wheel backwards
void Enigma::typeB(int rotor, int &n) {
    n = (n + positions[rotor]) % 26;
    n = std::distance(rotors[rotor].begin(), std::find(rotors[rotor].begin(), rotors[rotor].end(), n));
    n = (n - positions[rotor] + 26)%26;
}


// Passes a letter (in the form of a converted number) through the switchboard
void Enigma::switchB (int &n) {
    n = switchBoard.count(n) ? switchBoard[n] : n;
}


// Passes a letter (in the form of a converted number) through the reflector
void Enigma::reflect(int &n) {
    n = reflector[n];
}


// Types in a string of letters and returns the output string
std::string Enigma::type(std::string in) {
    std::string out = "";
    if (mode==include) 
        for (auto c : in) 
            out += isalpha(c) ? type(c) : c;
    else {
        int i=0; 
        for (auto c : in) {
            if (isalpha(c)) {
                out+= type(c);
                i++;
                if (i%4==0)
                    out+=' ';
            }
        }
    }
    return out;
}


// Types in an individual letter and returns the output string
char Enigma::type(char c) {
    rotate();               
    int n = convert(c);     if (debug) std::cout << "\n\nConverted " << c << " to\t\t" << n << std::endl;
    switchB(n);             if (debug) std::cout << "Switchboard:\t\t" << convert(n) << std::endl;
    typeF(3, n);            if (debug) std::cout << "Wheel C:\t\t" << convert(n) << std::endl;
    typeF(2, n);            if (debug) std::cout << "Wheel B:\t\t" << convert(n) << std::endl;
    typeF(1,n);             if (debug) std::cout << "Wheel A:\t\t" << convert(n) << std::endl;
    typeF(0, n);            if (debug) std::cout << "Wheel G:\t\t" << convert(n) << std::endl;
    reflect(n);             if (debug) std::cout << "Reflector:\t\t" << convert(n) << std::endl;
    typeB(0, n);            if (debug) std::cout << "Wheel G:\t\t" << convert(n) << std::endl;
    typeB(1, n);            if (debug) std::cout << "Wheel A:\t\t" << convert(n) << std::endl;
    typeB(2, n);            if (debug) std::cout << "Wheel B:\t\t" << convert(n) << std::endl;
    typeB(3,n);             if (debug) std::cout << "Wheel C:\t\t" << convert(n) << std::endl;
    switchB(n);             if (debug) std::cout << "Switchboard:\t\t" << convert(n) << std::endl;
    return convert(n);
}