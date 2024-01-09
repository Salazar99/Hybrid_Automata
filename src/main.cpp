#include "../include/global_variables.h"
#include "../include/UtilsJson.h"
#include "../include/tinyexpr.h"
#include "../include/graph.h"
#include "../include/tools.h"

#include <algorithm>
#include <time.h>
#include <random>
#include <thread>
#include <chrono>
#include <ctime>

#include "../include/csvfile.h"

#ifdef DEBUG_MODE
#define DEBUG_COMMENT(comment) std::cout << "[DEBUG] " << comment << std::endl;
#else
#define DEBUG_COMMENT(comment)
#endif

using namespace std;

int main(int argc, char const *argv[])
{
    long start = time(NULL);
    UtilsJson j;
    unordered_map<string, string> automataActualIstruction;

    System s = j.ScrapingJson("C://Users//aleal//Desktop//evrthng//Hybrid_Automata//secondexample.json");
    // System s = j.ScrapingJson("C://Users//tomvi//Hybrid_Automata//settings.json");
    // System s = j.ScrapingJson("../settings.json");
    vector<Automata> v = s.getAutomata();
    cout << s;

    int istanti = 0;

    DEBUG_COMMENT("Questo è un commento di debug" << istanti << " \n\n\n");

    unordered_map<string, int> indici;
    for (int j = 0; j < v.size(); j++)
    {
        indici[v[j].getName()] = j;
    }

    try // adding global variables and single automatas' variables' name in the first row
    {
        csvfile csv("../../src/export.csv", true); // throws exceptions!
        csv << "TIMES";
        for (auto const &key : s.getAutomataDependence())
        {
            csv << key.first;
        }
        csv << endrow;
    }
    catch (const exception &ex)
    {
        cout << "Exception was thrown: " << ex.what() << endl;
    }

    for (double time = 1; time < finaltime; time = time + delta)
    {

        cout << "################## TIME = " << time << " ##################\n";

        unordered_map<string, GraphNode *> nodes_map;

        /*creating the graph*/
        Graph g;
        for (int j = 0; j < v.size(); j++)
        {
            GraphNode *node = new GraphNode(v[j].getName());
            nodes_map[v[j].getName()] = node;
            g.addNode(node);
        }

        // adding all the edges in the graph
        for (int j = 0; j < v.size(); j++)
        {
            string tmp_instr = v[j].getInstructions();
            tmp_instr.erase(remove(tmp_instr.begin(), tmp_instr.end(), ' '), tmp_instr.end());
            tmp_instr.erase(remove(tmp_instr.begin(), tmp_instr.end(), '\n'), tmp_instr.end());
            vector<string> distinctInstructions = split_string(tmp_instr, ';'); // splitting at ; character
            string aux;

            for (string str : distinctInstructions) // loop single istructions
            {
                aux = split_string(str, '=')[1]; // take right part of the instruction

                for (auto const &x : s.getAutomataDependence())
                {
                    if (aux.find(x.first) != string::npos)
                    {
                        if (v[j].getName() != x.second)
                        {

                            /*
                            v[j] needs to be executed after x.second
                            x.second -> v[j] is the edge in the graph
                            */
                            g.addEdge(nodes_map[x.second], nodes_map[v[j].getName()]);
                        }
                    }
                }
            }
        }

        vector<GraphNode *> sorted = g.topologicalSort();

        cout << "\n\nOrdine di esecuzione: ";
        for (int j = 0; j < sorted.size(); j++)
        {
            if (j != sorted.size() - 1)
            {
                cout << sorted[j]->getName() << ", ";
            }
            else
            {
                cout << sorted[j]->getName() << "\n";
            }
        }

        // execution of every automatas in topological order
        for (int j = 0; j < sorted.size(); j++)
        {
            v[indici[sorted[j]->getName()]].checkForChanges();
            cout << "\nAutoma " << v[indici[sorted[j]->getName()]].getName() << " ,Nodo attuale: " << v[indici[sorted[j]->getName()]].getCurrentNode().getName() << "\n";
        }
        cout << "\nVariables Map: \n";
        printMap(*v[0].getAutomataVariables());

        // adding global variables and single automatas' variables' name in the first row
        try
        {
            csvfile csv("../../src/export.csv", false); // throws exceptions!
            csv << time;
            for (auto const &key : s.getAutomataDependence())
            {

                // if the variable has not already been assigned than i print 0
                if ((*(v[0].getAutomataVariables())).find(key.first) == (*(v[0].getAutomataVariables())).end())
                {
                    csv << 0;
                }
                else
                {
                    csv << *(*(v[0].getAutomataVariables()))[key.first];
                }
            }

            csv << endrow;
        }
        catch (const exception &ex)
        {
            cout << "Exception was thrown: " << ex.what() << endl;
        }

        this_thread::sleep_for(chrono::milliseconds(0));
        istanti++;
        cout << "\n\n";
    }

    cout << "Total Istanti: " << istanti;
    cout << "\nCi ha messo " << time(NULL) - start << " secondi";

    return 0;
}
