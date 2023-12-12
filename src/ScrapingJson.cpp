#include "../include/UtilsJson.h"
#include "../include/json.hpp"
#include <iostream>

using namespace std;
using json = nlohmann::json;

/// @brief creates all the automatas
/// @return the automatas

vector<Automata> UtilsJson::ScrapingJson(string c)
{
    std::ifstream f(c);
    json data = json::parse(f);
    vector<Automata> arrAutomata;
    vector<Node> arrNodes;
    Node startNode;
    vector<Node> finalNodes;
    unordered_map<string, double *> variables;
    int j = 0;
    int store = 0;

    // find all the automata in settings.json
    for (json automata : data["automata"])
    {
        j = 0;
        store = 0;

        // find all the nodes for each automata
        for (json node : automata["node"])
        {
            Node n(node["name"], node["description"], node["instructions"], (node["flag"] == "start") ? true : false);
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

        // find all the variables for each automata
        double *y;
        for (json variable : automata["variables"])
        {
            string var = variable["value"];
            y = new double(stod(var));
            variables[variable["name"]] = y;
        }

        // adding transictions to nodes
        int i = -1;
        for (Node n : arrNodes)
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
        Automata c(arrNodes, arrNodes[store], finalNodes, variables, status, 1);
        arrAutomata.push_back(c);
        // empty for next automata creation
        arrNodes.clear();
    }
    return arrAutomata;
}