#include "../include/global_variables.h"
#include "../include/UtilsJson.h"
#include "../include/tinyexpr.h"
#include "../include/graph.h"
#include <sstream>
#include <string>
#include <iostream>
#include <algorithm>
#include <time.h>
#include <random>
#include <thread>
#include <chrono>
#include <ctime>

#ifdef DEBUG_MODE
#define DEBUG_COMMENT(comment) std::cout << "[DEBUG] " << comment << std::endl;
#else
#define DEBUG_COMMENT(comment)
#endif

using namespace std;

/// @brief splits the string into different parts given a delimiter and returns a vector of substrings;
/// @param s the string to split
/// @param delimiter the delimiter
/// @return a vector of substrings
std::vector<string> split_string3(const std::string &s, char delimiter)
{
    std::vector<std::string> tokens;
    std::istringstream ss(s);
    std::string token;

    while (std::getline(ss, token, delimiter))
    {
        tokens.push_back(token);
    }

    return tokens;
}

/// @brief prints the system variables map
/// @param map the pointer to unordered map
void printMap2(unordered_map<string, double *> &sharedVariables)
{
    for (auto &pair : sharedVariables)
    {
        std::cout << pair.first << ": " << *(pair.second) << "\n";
    }
}

int main(int argc, char const *argv[])
{
    long start = time(NULL);
    UtilsJson j;
    unordered_map<string, string> automataActualIstruction;

    /*
        NEEDs TO BE ADDED IN YOUR CMAKELIST
        #target_compile_definitions(main PRIVATE $<$<BOOL:${DEBUG_MODE}>:DEBUG_MODE>)
    */
    // vector<Automata> v = j.ScrapingJson("C://Users//aleal//Desktop//evrthng//Hybrid_Automata//settings.json");
    //  vector<Automata> v = j.ScrapingJson("C://Users//tomvi//Hybrid_Automata//settings.json");
    System s = j.ScrapingJson("../settings.json");
    vector<Automata> v = s.getAutomata();
    cout << s;

    int istanti = 0;

    DEBUG_COMMENT("Questo è un commento di debug" << istanti << " \n\n\n");

    unordered_map<string, int> indici;
    for (int j = 0; j < v.size(); j++)
    {
        indici[v[j].getName()] = j;
    }

    for (double time = 1; time < finaltime; time = time + delta)
    {

        /*
            secondo me ci servirà una mappa delle "dipendenze", ovvero x è calcolato nell'automa a, d in b....
            x -> a
            d -> b
            c -> d

            ci servirà una mappa che si aggiorna ad ogni time, con automa e le istruzioni del suo nodo corrente
            hashmap <automa-instructions>


            l'idea è di avere un grafo con gli automi come nodi e poi gli archi rappresentano le dipendenze, top-sort, abbiamo l'ordine
            (chatgpt dice che ha senso)
            guardatevi al volo algoritmisacrograal che in 3 secondi lo riprendete
            vi lascio uno pseudocodice

            crea grafo dell'istante
            per ognuno di questi:
                crea nodo nel grafo
                cicla ogni istruzione:
                    aggiungi arco (a->b, a va eseguito prima di b)
            calcola topological sort
            seguendo l'ordine esegui gli automi




        */
        unordered_map<string, GraphNode *> nodes_map;
        /*
        Automa A -> nodo_A
        Automa B -> nodo_B

        */

        Graph g;
        for (int j = 0; j < v.size(); j++)
        {
            cout << "\nAutoma " << v[j].getName() << " ,Nodo attuale: " << v[j].getCurrentNode().getName() << "\n";
            GraphNode *node = new GraphNode(v[j].getName());
            nodes_map[v[j].getName()] = node;
            g.addNode(node);
        }

        // filling the actualIstructions' automatas
        for (int j = 0; j < v.size(); j++)
        {
            /*
                y'=((3*x+2)^2) * ((y+1)^0.5);
                y'=((x+2)^2) * ((y+1)^0.5);
            */
            // automataActualIstruction[v[j].getInstructions()] = v[j].getName();
            //  cout << "\nIstruzioni attuali nodo corrente: " << v[j].getInstructions() << "\n";
            string tmp_instr = v[j].getInstructions();
            tmp_instr.erase(std::remove(tmp_instr.begin(), tmp_instr.end(), ' '), tmp_instr.end());
            tmp_instr.erase(std::remove(tmp_instr.begin(), tmp_instr.end(), '\n'), tmp_instr.end());
            vector<string> distinctInstructions = split_string3(tmp_instr, ';'); // splitting at ; character
            string aux;

            for (string str : distinctInstructions) // loop single istructions
            {
                aux = split_string3(str, '=')[1]; // take right part of the instruction

                for (auto const &x : s.getAutomataDependence())
                {
                    if (aux.find(x.first) != string::npos)
                    {
                        if (v[j].getName() != x.second)
                        {

                            /*
                            v[j] ha una dipendenza su x.second, ovvero deve aspettare x.second
                            x.second -> v[j] ----- x.second deve essere eseguito per primo
                            */
                            g.addEdge(nodes_map[x.second], nodes_map[v[j].getName()]);
                        }
                    }
                    // cout << "- " << x.first << " : " << x.second << ";\n";
                }
            }
        }

        // for(Automa v : graph.getSortedList())
        // same code

        /*
        "A" -> 0
        "B" -> 1
        return top -> "B", "A"
        ordine di s.v -> "A", "B"

        */
        vector<GraphNode *> sorted = g.topologicalSort();

        for (int j = 0; j < sorted.size(); j++)
        {
            cout << "\ntocca all'automa: " << sorted[j]->getName() << "\n";
            v[indici[sorted[j]->getName()]].checkForChanges();
            cout << "\nAutoma " << v[indici[sorted[j]->getName()]].getName() << " ,Nodo attuale dpc: " << v[indici[sorted[j]->getName()]].getCurrentNode().getName() << "\n";
            // cout << "Nodo corrente: " << v[indici[sorted[j]->getName()]].getCurrentNode().getName() << "\n\n";
        }

        /*for (int j = 0; j < v.size(); j++)
        {
            v[j].checkForChanges();
            cout << "Nodo corrente: " << v[j].getCurrentNode().getName() << "\n\n";
        }*/

        for (int j = 0; j < v.size(); j++)
        {
            cout << "Mappa per Automa" << j << "\n";
            printMap2(*v[j].getAutomataVariables());
        }

        this_thread::sleep_for(std::chrono::milliseconds(2000));
        istanti++;
    }

    cout << "Total Istanti: " << istanti;
    cout << "\nCi ha messo " << time(NULL) - start << " secondi";

    return 0;
}
