#pragma once
#include <iostream>
#include <string.h>
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

class Enigma {
    private:
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
        const std::vector<std::vector<int>> reflector_templates =  {{4,13,10,16,0,20,24,22,9,8,2,14,15,1,11,12,3,23,25,21,5,19,7,17,6,18},
                                                                    {17,3,14,1,9,13,19,10,21,4,7,12,11,5,2,22,25,0,23,6,24,8,15,18,20,16}};
        const std::vector<int> turnovers = {16,4,21,9,25,25,12};

        std::vector<std::vector<int>> rotors;
        std::vector<int> notches;
        std::map<int, int> reflector;
        std::vector<int> positions;
        std::map<int, int> switchBoard;
        
        void validateInput(Reflector ref, std::vector<Rotor> inputRotors, std::string rotorPositions, std::vector<int> rotorSettings, std::string switches) {
            if (!((inputRotors.size()==3 && rotorPositions.size()==3 && rotorSettings.size()==3) || (inputRotors.size()==4 && rotorPositions.size()==4 && rotorSettings.size()==4)))
                throw std::invalid_argument("Please use only 3 values for all of {rotors, initial positions, and ring settings} or 4 values for all of {rotors, initial positions, and ring settings}");
            std::istringstream ss (switches);
            std::string str;
            int c=0;
            std::set<int> s;
            while (ss >> str) {
                if (str.size()!=2 || !isalpha(str.at(0))  || !isalpha(str.at(1)))
                    throw std::invalid_argument("Please format the plugboard settings as 0-13 pairs of characters, e.g. \"XX XX XX XX\", where each X is a unique alphabetic character");
                s.insert(convert(str[0]));
                s.insert(convert(str[1]));
                c+=2;
            }
            if (s.size()!=c) // if one or more of the characters was not unique
                throw std::invalid_argument("Please format the plugboard settings as 0-13 pairs of characters, e.g. \"XX XX XX XX\", where each X is a unique alphabetic character");
        }

        void M3toM4(Reflector &ref, std::vector<Rotor> &inputRotors, std::string &rotorPositions, std::vector<int> &rotorSettings) {
            inputRotors.insert(inputRotors.begin(), (Rotor)(ref ? 9 : 8));
            rotorPositions.insert(0, "A");
            rotorSettings.insert(rotorSettings.begin(), 0);
        }

        void setReflector(int ref) {
            for (int i=0; i<26; i++)
                reflector[i] = reflector_templates[ref][i];
        }
        
        void setRotors(std::vector<Rotor> xrotors) {
            for (int i=0; i<4; i++) {
                rotors.push_back(rotor_templates[xrotors[i]]);
                if (xrotors[i]>7 || xrotors[i]<5) notches.push_back(turnovers[xrotors[i]]);
                else notches.push_back(50);
            }
        }

        void setPositions(std::string pos) {
            for (int i=0; i<4; i++) { 
                int n = convert(pos[i]);
                positions.push_back(n);
            }
        }

        void setOffsets(std::vector<int> offsets) {
            for (int i=0; i<4; i++) {
                for (int j=0; j<26; j++) {
                    rotors[i][j]+=offsets[i];
                    rotors[i][j] %= 26;
                }
                if (offsets[i]>0) {
                    std::vector<int> copy = rotors[i];
                    rotors[i].clear();
                    for (int j=26-offsets[i]; j<26; j++) 
                        rotors[i].push_back(copy[j]);
                    for (int j=0; j<26-offsets[i]; j++) 
                        rotors[i].push_back(copy[j]);
                }
            }
        }

        void setSwitches(std::string switches) {
            switchBoard.clear();
            std::stringstream ss (switches);
            std::string pair;
            while (ss >> pair) { 
                switchBoard[convert(pair[0])] = convert(pair[1]);
                switchBoard[convert(pair[1])] = convert(pair[0]);
            }
        }

        int convert(char c) {
            return (int)toupper(c) - 'A';
        }

        char convert(int n) {
            return (char)n + 'A';
        }

        void rotate() {
            int trigger = 0;
            if (positions[3]==notches[3] || (notches[3]==50 && (positions[3]==turnovers[5] || positions[3]==turnovers[6])))
                trigger = 1;
            positions[3] = (positions[3] + 1) % 26;
            // if rotor 2 was on turnover
            if (trigger) {
                trigger = 0;
                if (positions[2]==notches[2] || (notches[2]==50 && (positions[2]==turnovers[5] || positions[2]==turnovers[6])))
                    trigger = 1;
                positions[2] = (positions[2] + 1) % 26;
            }
            // check for double stepping
            else if (positions[2]==notches[2] || (notches[2]==50 && (positions[2]==turnovers[5] || positions[2]==turnovers[6]))) {
                positions[2] = (positions[2] + 1) % 26;
                positions[1] = (positions[1] + 1) % 26;
            }
            // if rotor 1 was on turnover
            if (trigger)
                positions[1] = (positions[1] + 1) % 26;
        }

        void typeF(int rotor, int &n) {
            n = rotors[rotor][ (n+positions[rotor])%26 ];
            n = (n - positions[rotor] + 26)%26;
        }

        void typeB(int rotor, int &n) {
            n = (n + positions[rotor]) % 26;
            n = std::distance(rotors[rotor].begin(), std::find(rotors[rotor].begin(), rotors[rotor].end(), n));
            n = (n - positions[rotor] + 26)%26;
        }

        void switchB (int &n) {
            n = switchBoard.count(n) ? switchBoard[n] : n;
        }

        void reflect(int &n) {
            n = reflector[n];
        }

    public:
        int debug;
        Enigma(Reflector ref, std::vector<Rotor> inputRotors, std::string rotorPositions, std::vector<int> rotorSettings, std::string switches) {
            validateInput(ref, inputRotors, rotorPositions, rotorSettings, switches);
            if (inputRotors.size()==3) M3toM4(ref, inputRotors, rotorPositions, rotorSettings);
            setReflector(ref);
            setRotors(inputRotors);
            setPositions(rotorPositions);
            for (auto e : rotorSettings) offsetCopyTest.push_back(e);
            setOffsets(rotorSettings);
            setSwitches(switches);
            debug = 0;
        }
        std::vector<int> offsetCopyTest;
        std::string type(std::string in) {
            std::string out = "";        
            for (auto c : in) {
                if (isalpha(c)) {  
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
                    out += convert(n);      if (debug) std::cout << "Output:\t\t\t" << convert(n) << std::endl;
                }
                else out += c;
            }
            return out;
        }
};