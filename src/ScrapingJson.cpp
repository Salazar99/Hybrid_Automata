#include "../include/UtilsJson.h"
#include "../include/json.hpp"
#include <iostream>

using namespace std;
using json = nlohmann::json;

vector<Automata> UtilsJson::ScrapingJson(string c)
{
    std::ifstream f(c);

    json data = json::parse(f);

    vector<Automata> arrAutomata;
    vector<Node> arrNodes;
    Node startNode;
    vector<Node> finalNodes;
    int j = 0;
    int store = 0;
    for (json automata : data["automata"])
    {
        j = 0;
        store = 0;
        for (json node : automata["node"])
        {
            Node n(node["name"], node["description"]);
            arrNodes.push_back(n);
            if (node["flag"] == "start")
            {
                startNode = n;
                store = j;
            }
            else if (node["flag"] == "final")
            {
                finalNodes.push_back(n);
            }
            j++;
        }

        unordered_map<string, double *> variables;
        double *y;
        for (json variable : automata["variables"])
        {
            string var = variable["value"];
            y = new double(stod(var));
            variables[variable["name"]] = y;
        }

        int i = -1;
        for (Node n : arrNodes) // adding transictions to nodes
        {
            i++;
            for (json node : automata["node"])
            {
                if (node["name"] == n.getName())
                {

                    for (json transition : node["transitions"])
                    {
                        string to;
                        for (Node n1 : arrNodes)
                        {
                            to = (n1.getName() != transition["to"]) ? to : n1.getName();
                            /*if (n1.getName() == transition["to"])
                            {
                                to = n1;
                            }*/
                        }
                        arrNodes[i].addTransition(transition["condition"], to);
                    }
                }
            }
        }

        Status status = RUNNING;
        Automata c(arrNodes, arrNodes[store], finalNodes, variables, status);
        arrAutomata.push_back(c);
        arrNodes.clear(); // empty for next automata creation
    }
    return arrAutomata;
}