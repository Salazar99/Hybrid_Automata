#include "../include/UtilsJson.h"
#include "../include/json.hpp"
#include "../include/global_variables.h"
#include "../include/tinyexpr.h"
#include <iostream>

using namespace std;
using json = nlohmann::json;

/// @brief creates all the automatas
/// @return the automatas

double delta;
double finaltime;

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

    // set global variables
    string h_string = data["system"]["global"]["h"];
    delta = stod(h_string);
    string tfinal_string = data["system"]["global"]["tfinal"];
    finaltime = stod(tfinal_string);

    // find all the automata in settings.json
    for (json automata : data["system"]["automata"])
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
            y = new double(te_interp(var.c_str(), 0));
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