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
    vector<Automata> v = j.ScrapingJson("../settings.json");

    for (Automata a : v)
    {
        cout << a;
    }
    Node q1("q1", "first node");
    Node q2("q2", "second node");
    Node q3("q3", "third node");

    q1.addTransition("(x>=5)", q2);  // q1 -> q2
    q2.addTransition("(x>=10)", q3); // q2 -> q3
    q2.addTransition("(x<5)", q1);

    q3.addTransition("(x<5)", q1);
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

    // for rand
    // Declaring the upper and lower
    // bounds
    // double lower_bound = 0;
    // double upper_bound = 15;
    // uniform_real_distribution<double> unif(lower_bound, upper_bound);
    // default_random_engine re;

    vector<Node> aux;

    while (1)
    {
        aux = automata.getFinalNodes();
        if (!(find(aux.begin(), aux.end(), automata.getCurrentNode()) == aux.end()))
        {
            cout << "Raggiunto il nodo finale: " << automata.getCurrentNode().getName() << "\n";
            break;
        }
        // random variables

        // variables["x"] = unif(re);

        cout << "Inserisci valore di x: ";
        cin >> *(variables["x"]);

        // cout << "Inserisci valore di y: ";
        // cin >> *(variables["y"]);

        cout << "\nX: " << *(variables["x"]);
        // cout << "\nY: " << *(variables["y"]);

        // automata.setAutomataVariables(variables);
        automata.checkForChanges();
        cout << "Nodo corrente: " << automata.getCurrentNode().getName() << "\n";
        //  this_thread::sleep_for(chrono::seconds(10));
    }

    return 0;
}
