#include "../include/UtilsJson.h"
#include <iostream>

using namespace std;

int main(int argc, char const *argv[])
{
    UtilsJson j;
    vector<Automata> v = j.ScrapingJson("../settings.json");

    for (Automata a : v)
    {
        cout << a;
    }
    return 0;
}
