#include "../include/Objects.h"
#include <iostream>
#include <algorithm>
#include <time.h>
#include <random>
#include <thread>
#include <chrono>

using namespace std;

int main(int argc, char const *argv[])
{

    Node q1("q1", "first node");
    Node q2("q2", "second node");
    Node q3("q3", "third node");

    q1.addTransition("x>=5 && y<=5", q2);   // q1 -> q2
    q2.addTransition("x>=10 && z!=10", q3); // q2 -> q3
    unordered_map<Transition, Node *, TransitionHash, TransitionEqual> copy = q1.getTransitions();
    vector<Transition> copy2 = q1.getTransitionKeys();

    for (Transition t : copy2)
    {
        cout << "Valore di q2, Name: " << copy[t]->getName() << ", Size: " << copy[t]->getTransitionKeys().size() << "\n\n\n";
    }

    q3.addTransition("x>50", q1);
    vector<Node> nodes = {q1, q2, q3};
    Node initialNode = q1;
    vector<Node> finalNodes = {q3};
    unordered_map<string, double> variables;
    variables["x"] = 1.56;
    variables["y"] = 2.33;
    variables["z"] = 3.4;
    Status status = RUNNING;

    Automata automata(nodes, initialNode, finalNodes, variables, status);
    automata.setCurrentNode(initialNode);
    cout << automata;

    // for rand
    // Declaring the upper and lower
    // bounds
    double lower_bound = 0;
    double upper_bound = 15;
    uniform_real_distribution<double> unif(lower_bound, upper_bound);
    default_random_engine re;

    vector<Node> aux;
    int help;

    cout << "Size: " << q2.getTransitionKeys().size() << "\n";

    while (1)
    {
        aux = automata.getFinalNodes();
        if (!(find(aux.begin(), aux.end(), automata.getCurrentNode()) == aux.end()))
        {
            cout << "Raggiunto il nodo finale: " << automata.getCurrentNode().getName() << "\n";
            break;
        }
        // random variables

        variables["x"] = unif(re);
        variables["y"] = unif(re);
        variables["z"] = unif(re);

        cout << "\nX: " << variables["x"];
        cout << " Y: " << variables["y"];
        cout << " Z: " << variables["z"] << "\n";

        cin >> help;

        automata.setAutomataVariables(variables);
        automata.checkForChanges();
        cout << "Nodo corrente: " << automata.getCurrentNode().getName() << "\n";

        cin >> help;
        // this_thread::sleep_for(chrono::seconds(10));
        cout << "Size: " << q2.getTransitionKeys().size() << "\n";
    }

    return 0;
}
