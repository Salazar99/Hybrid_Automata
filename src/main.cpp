#include "../include/global_variables.h"
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
    int stop;
    for (int j = 0; j < v.size(); j++)
    {
        cout << v[j];
    }
    int istanti = 0;

    for (double time = 1; time < finaltime; time = time + delta)
    {

        for (int j = 0; j < v.size(); j++)
        {
            v[j].checkForChanges();
            cout << "Nodo corrente: " << v[j].getCurrentNode().getName() << "\n\n";
        }

        // this_thread::sleep_for(chrono::seconds(1));

        this_thread::sleep_for(std::chrono::milliseconds(500));
        istanti++;
        /*for (Automata a : v)
        {
            a.checkForChanges();
            cout << "Nodo corrente: " << a.getCurrentNode().getName() << "\n\n";

            cin >> stop;
        }*/
    }

    cout << "Total Istanti: " << istanti;

    /*for (Automata a : v)
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
            cout << "Nodo corrente: " << a.getCurrentNode().getName() << "\n\n";
            // this_thread::sleep_for(chrono::seconds(2));
            cin >> stop;
        }
    }*/
    return 0;
}
