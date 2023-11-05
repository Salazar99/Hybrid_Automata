#include "../include/Objects.h"
#include <iostream>

using namespace std;

int main(int argc, char const *argv[])
{

    Node q1("q1", "first node");
    Node q2("q2", "second node");
    Node q3("q3", "third node");

    q1.addTransition("x>=5", q2);  // q1 -> q2
    q2.addTransition("x>=10", q3); // q2 -> q3

    vector<Node> nodes = {q1, q2, q3};
    Node initialNode = q1;
    vector<Node> finalNodes = {q3};
    unordered_map<string, double> variables;
    variables["x"] = 1;
    Status status = RUNNING;

    Automata automata(nodes, initialNode, finalNodes, variables, status);
    automata.setCurrentNode(initialNode);
    cout << automata;

    return 0;
}
