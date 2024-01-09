#include "../include/tools.h"

void printMap(unordered_map<string, double *> &sharedVariables)
{
    for (auto &pair : sharedVariables)
    {
        std::cout << pair.first << ": " << *(pair.second) << "\n";
    }
}

vector<string> split_string(const string &s, char delimiter)
{
    vector<string> tokens;
    istringstream ss(s);
    string token;

    while (getline(ss, token, delimiter))
    {
        tokens.push_back(token);
    }

    return tokens;
}