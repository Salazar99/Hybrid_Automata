#include <unordered_map>
#include <string>
#include <iostream>
#include <sstream>

using namespace std;

/// @brief prints the system variables map
/// @param map the pointer to unordered map
void printMap(unordered_map<string, double *> &sharedVariables);

/// @brief splits the string into different parts given a delimiter and returns a vector of substrings;
/// @param s the string to split
/// @param delimiter the delimiter
/// @return a vector of substrings
vector<string> split_string(const string &s, char delimiter);