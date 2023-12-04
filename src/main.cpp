#include "../include/UtilsJson.h"
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

    vector<Automata> v = j.ScrapingJson("C://Users//aleal//Desktop//evrthng//Hybrid_Automata//settings.json");

    unordered_map<string, double *> variables;
    double x = 1.56;
    double y = 2.0;
    variables["x"] = &x;
    variables["y"] = &y;

    for (Automata a : v)
    {
        a.setAutomataVariables(variables);
        cout << a;
        vector<Node> aux;

        while (1)
        {
            aux = a.getFinalNodes();
            if (!(find(aux.begin(), aux.end(), a.getCurrentNode()) == aux.end()))
            {
                cout << "Raggiunto il nodo finale: " << a.getCurrentNode().getName() << "\n";
                break;
            }
            // random variables

            // variables["x"] = unif(re);

            cout << "Inserisci valore di x: ";
            cin >> *(variables["x"]);

            // cout << "Inserisci valore di y: ";
            // cin >> *(variables["y"]);

            cout << "\nX: " << *(variables["x"]) << "\n";
            // cout << "\nY: " << *(variables["y"]);

            // automata.setAutomataVariables(variables);
            a.checkForChanges();
            cout << "Nodo corrente: " << a.getCurrentNode().getName() << "\n";
            //  this_thread::sleep_for(chrono::seconds(10));
        }
    }

    /*
    Node q1("q1", "first node");
    /Node q2("q2", "second node");
    Node q3("q3", "third node");

    q1.addTransition("(x>=5)", q2.getName());  // q1 -> q2
    q2.addTransition("(x>=10)", q3.getName()); // q2 -> q3
    q2.addTransition("(x<5)", q1.getName());

    q3.addTransition("(x<5)", q1.getName());
    vector<Node> nodes = {q1, q2, q3};
    Node initialNode = q1;
    vector<Node> finalNodes = {q3};
    unordered_map<string, double *> variables;
    double x = 1.56;
    double y = 2.0;
    variables["x"] = &x;
    variables["y"] = &y;
    Status status = RUNNING;
    Automata automata(nodes, initialNode, finalNodes, variables, status);
    automata.setCurrentNode(initialNode);
    cout << automata;
    */

    // for rand
    // Declaring the upper and lower
    // bounds
    // double lower_bound = 0;
    // double upper_bound = 15;
    // uniform_real_distribution<double> unif(lower_bound, upper_bound);
    // default_random_engine re;

    return 0;
}
