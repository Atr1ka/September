#pragma once
#include <iostream>
#include <cstring>
#include <map>
#include <vector>
#include <utility>
#include <cctype>
#include <algorithm>
#include <cstdbool>
#include <sstream>
#include <ranges>
#include <set>

enum Rotor {I, II, III, IV, V, VI, VII, VIII, Beta, Gamma};
enum Reflector {B, C};
enum Mode {include, ignore};

class Enigma {
    private:
        int mode, debug;
        std::vector<std::vector<int>> rotors;
        std::vector<int> notches;
        std::map<int, int> reflector;
        std::vector<int> positions;
        std::map<int, int> switchBoard;

        template <typename A, typename B> 
        void validateInput(Reflector, std::vector<Rotor>, A, B, std::string);

        template <typename A, typename B> 
        void M3toM4(Reflector&, std::vector<Rotor>&, A&, B&);

        void setReflector(int);
        
        void setRotors(std::vector<Rotor>);

        template<typename A>
        void setPositions(A);

        template<typename A>
        void setOffsets(A);

        void setSwitches(std::string);

        int convert(char);

        char convert(int);

        void rotate();

        void typeF(int, int&);

        void typeB(int, int&);

        void switchB (int&);

        void reflect(int&);

    public:
        template <typename A, typename B> 
        Enigma(Reflector, std::vector<Rotor>, std::initializer_list<A>, std::initializer_list<B>, std::string, Mode, int=0);
        
        std::string type(std::string);

        char type(char);
};




// Definitions with templates (have to be included in header file as templates cannot be deduced at runtimes)



// Validate input for the Enigma settings
template <typename A, typename B> 
void Enigma::validateInput(Reflector ref, std::vector<Rotor> inputRotors, A rotorPositions, B rotorSettings, std::string switches) {
    // if it is neither a valid M3 or M4 combinations, throw error
    if (!((inputRotors.size()==3 && rotorPositions.size()==3 && rotorSettings.size()==3) || (inputRotors.size()==4 && rotorPositions.size()==4 && rotorSettings.size()==4))) {
        // std::cout << inputRotors.size() << " " << strlen(rotorPositions) << " " << strlen(rotorSettings) << std::endl;
        throw std::invalid_argument("Please use only 3 values for all of {rotors, initial positions, and ring settings} or 4 values for all of {rotors, initial positions, and ring settings}");
    }
    // check the switchboard settings are all valid
    std::istringstream ss (switches);
    std::string str;
    std::set<int> s;
    int c=0;
    while (ss >> str) {
        if (str.size()!=2 || !isalpha(str.at(0))  || !isalpha(str.at(1))) // if the section is not XX
            throw std::invalid_argument("Please format the plugboard settings as 0-13 pairs of characters, e.g. \"XX XX XX XX\", where each X is a unique alphabetic character");
        s.insert(convert(str[0]));
        s.insert(convert(str[1]));
        c+=2;
    }
    if (s.size()!=c) // if one or more of the characters was not unique
        throw std::invalid_argument("Please format the plugboard settings as 0-13 pairs of characters, e.g. \"XX XX XX XX\", where each X is a unique alphabetic character");
}



// Converts a valid M3 setting to a equivalent M4 setting for backwards compatiability
template <typename A, typename B> 
void Enigma::M3toM4(Reflector &ref, std::vector<Rotor> &inputRotors, A &rotorPositions, B &rotorSettings) {
    inputRotors.insert(inputRotors.begin(), (Rotor)(ref ? 9 : 8));
    rotorPositions.insert(rotorPositions.begin(), 0);
    rotorSettings.insert(rotorSettings.begin(), 0);
}



// Takes in string of intial positions and adds to vector of positions
template<typename A>
void Enigma::setPositions(A pos) {
    for (int i=0; i<4; i++) { 
        int n = isalpha(pos[i]) ? convert(pos[i]) : pos[i];
        positions.push_back(n);
    }
}



// Takes in vector of ring setting values, then modifies the rotors vector to apply them
template<typename A>
void Enigma::setOffsets(A offsets) {
    int currOffset;
    for (int i=0; i<4; i++) {
        for (int j=0; j<26; j++) {
            currOffset = (isalpha(offsets[i]) ? convert(offsets[i]) : offsets[i]);
            rotors[i][j]+=currOffset;
            rotors[i][j] %= 26;
        }
        if (offsets[i]>0) {
            std::vector<int> copy = rotors[i];
            rotors[i].clear();
            for (int j=26-currOffset; j<26; j++) 
                rotors[i].push_back(copy[j]);
            for (int j=0; j<26-currOffset; j++) 
                rotors[i].push_back(copy[j]);
        }
    }
}



// Constructor
template <typename A, typename B> 
Enigma::Enigma(Reflector ref, std::vector<Rotor> inputRotors, std::initializer_list<A> rotorPositionsInit, std::initializer_list<B> rotorSettingsInit, std::string switches, Mode m, int deb) {
    std::vector<A> rotorPositions (rotorPositionsInit);
    std::vector<B> rotorSettings (rotorSettingsInit);
    validateInput(ref, inputRotors, rotorPositions, rotorSettings, switches);
    if (inputRotors.size()==3) {
        M3toM4(ref, inputRotors, rotorPositions, rotorSettings);
    };
    setReflector(ref);
    setRotors(inputRotors);
    setPositions(rotorPositions);
    setOffsets(rotorSettings);
    setSwitches(switches);
    mode = m;
    debug = deb;
}



