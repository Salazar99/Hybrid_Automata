#include "../include/global_variables.h"
#include "../include/UtilsJson.h"
#include "../include/tinyexpr.h"
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
    /*
    for (Automata a : v)
    {
        cout << a;
    }
    */
    int istanti = 0;

    DEBUG_COMMENT("Questo è un commento di debug" << istanti << " \n\n\n");

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

        // filling the actualIstructions' automatas
        for (Automata a : v)
        {
            automataActualIstruction[a.getInstructions()] = a.getName();
        }

        // for(Automa v : graph.getSortedList())
        // same code

        for (int j = 0; j < v.size(); j++)
        {
            v[j].checkForChanges();
            cout << "Nodo corrente: " << v[j].getCurrentNode().getName() << "\n\n";
        }

        this_thread::sleep_for(std::chrono::milliseconds(1000));
        istanti++;
    }

    cout << "Total Istanti: " << istanti;
    cout << "\nCi ha messo " << time(NULL) - start << " secondi";

    return 0;
}
