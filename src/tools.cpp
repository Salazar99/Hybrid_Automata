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

    /*

    x = beta+2+a+2+a
    x = beta+2+a+2+a



    x = beta*a
    x = beta*a

    log(a)
    a
        */
    int pos = find_true_occ(origin, remove, 0);

    // int pos = origin.find(remove);
    bool replace_string;
    while (pos != string::npos)
    {
        DEBUG_COMMENT("Siamo dentro\n");
        /*replace_string = false;
        if (pos + 1 < origin.length() && pos - 1 >= 0)
        { // in mezzo
            if (!isalpha(origin[pos + 1]) && !isdigit(origin[pos + 1]) && !isalpha(origin[pos - 1]) && !isdigit(origin[pos - 1]))
                replace_string = true;
        }
        else if (pos + 1 >= origin.length())
        { // solo roba prima
            if (!isalpha(origin[pos - 1]) && !isdigit(origin[pos - 1]))
                replace_string = true;
        }
        else
        { // solo roba dopo
            if (!isalpha(origin[pos + 1]) && !isdigit(origin[pos + 1]))
                replace_string = true;
        }

        if (replace_string)
        {
            origin.replace(pos, remove.length(), to_replace);
        }*/
        origin.replace(pos, remove.length(), to_replace);
        pos = find_true_occ(origin, remove, pos);
        // pos = origin.find(remove);
    }

    return origin;
}

/*

x = beta+a

*/

int find_true_occ(string origin, string to_find, int pos)
{

    /*
    0123456789
    x=beta+a;
    */
    int next;
    int pos = origin.find(to_find, pos);
    while (pos != string::npos)
    {
        next = pos + to_find.length();
        if (pos + 1 < origin.length() && pos - 1 >= 0)
        { // in mezzo
            if (!isalpha(origin[next]) && !isdigit(origin[next]) && !isalpha(origin[pos - 1]) && !isdigit(origin[pos - 1]))
                return pos;
        }
        else if (pos + 1 >= origin.length())
        { // solo roba prima
            if (!isalpha(origin[pos - 1]) && !isdigit(origin[pos - 1]))
                return pos;
        }
        else
        { // solo roba dopo
            if (!isalpha(origin[next]) && !isdigit(origin[next]))
                return pos;
        }
        origin.replace(pos, to_find.length(), "0");
        pos = origin.find(to_find, pos);
    }
    return string::npos;
}