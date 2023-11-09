/*#include "../include/json.hpp"
#include "../include/Objects.h"
#include <iostream>
#include <fstream>
using namespace std;
using json = nlohmann::json;

int main(int argc, char const *argv[])
{

    std::ifstream f("../settings.json");
    json data = json::parse(f);
    vector<Automata> arrAutomata;
    vector<Node> arrNodes;
    Node startNode;
    vector<Node> finalNodes;
    for (json automata : data["automata"])
    {
        for (json node : automata["node"])
        {
            Node n(node["name"], node["description"]);
            arrNodes.push_back(n);
            if (node["flag"] == "start")
            {
                startNode = n;
            }
            else if (node["flag"] == "final")
            {
                finalNodes.push_back(n);
            }
        }

        unordered_map<string, double> variables;
        for (json variable : automata["variables"])
        {
            variables[variable["name"]] = variable["value"];
        }

        for (Node n : arrNodes) // adding transictions to nodes
        {
            for (json node : automata["node"])
            {
                if (node["name"] == n.getName())
                {
                    for (json transition : node["transitions"])
                    {
                        Node to;
                        for (Node n1 : arrNodes)
                        {
                            to = (n1.getName() != transition["to"]) ? to : n1;
                        }
                        n.addTransition(transition["condition"], to);
                    }
                }
            }
        }

        Status status = RUNNING;
        arrAutomata.push_back(Automata(arrNodes, startNode, finalNodes, variables, status));
        arrNodes.clear(); // empty for next automata creation
    }

    for (Automata a : arrAutomata)
    {
        cout << a;
    }

    return 0;
}
*/