#include "../include/UtilsJson.h"
#include "../include/tinyexpr.h"
#include <iostream>
#include <algorithm>
#include <time.h>
#include <random>
#include <thread>
#include <chrono>

using namespace std;

int main(int argc, char const *argv[])
{
    UtilsJson j;

    // vector<Automata> v = j.ScrapingJson("C://Users//aleal//Desktop//evrthng//Hybrid_Automata//settings.json");
    vector<Automata> v = j.ScrapingJson("../settings.json");

    unordered_map<string, double *> variables;
    double x = 20;
    variables["x"] = &x;

    for (Automata a : v)
    {
        cout << a;
        vector<Node> aux;
        int stop;
        while (1)
        {
            aux = a.getFinalNodes();

            if (!aux.empty() && !(find(aux.begin(), aux.end(), a.getCurrentNode()) == aux.end()))
            {
                cout << "Raggiunto il nodo finale: " << a.getCurrentNode().getName() << "\n";
                break;
            }

            // automata.setAutomataVariables(variables);
            a.checkForChanges();
            cout << "Nodo corrente: " << a.getCurrentNode().getName() << "\n";
            this_thread::sleep_for(chrono::seconds(2));
            // cin >> stop;
        }
    }
    return 0;
}
