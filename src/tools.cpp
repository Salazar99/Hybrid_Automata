#include "../include/tools.h"
#include <cctype>

#ifdef DEBUG_MODE
#define DEBUG_COMMENT(comment) std::cout << "[DEBUG] " << comment << std::endl;
#else
#define DEBUG_COMMENT(comment)
#endif

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

string replace_var(string origin, string remove, string to_replace)
{
    int next;
    int pos = origin.find(remove, 0);
    bool replace_string;
    while (pos != string::npos)
    {
        next = pos + remove.length();
        DEBUG_COMMENT("Siamo dentro\n");
        replace_string = false;
        if (pos + 1 < origin.length() && pos - 1 >= 0)
        { // something left and something right
            if (!isalpha(origin[next]) && !isdigit(origin[next]) && !isalpha(origin[pos - 1]) && !isdigit(origin[pos - 1]))
                replace_string = true;
        }
        else if (pos + 1 >= origin.length())
        { // something left
            if (!isalpha(origin[pos - 1]) && !isdigit(origin[pos - 1]))
                replace_string = true;
        }
        else
        { // something right
            if (!isalpha(origin[next]) && !isdigit(origin[next]))
                replace_string = true;
        }

        if (replace_string)
        {
            origin.replace(pos, remove.length(), to_replace);
        }
        pos = origin.find(remove, pos + 1);
    }

    return origin;
}
