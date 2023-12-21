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

    /*
        NEEDs TO BE ADDED IN YOUR CMAKELIST
        #target_compile_definitions(main PRIVATE $<$<BOOL:${DEBUG_MODE}>:DEBUG_MODE>)
    */
    vector<Automata> v = j.ScrapingJson("C://Users//aleal//Desktop//evrthng//Hybrid_Automata//settings.json");
    // vector<Automata> v = j.ScrapingJson("C://Users//tomvi//Hybrid_Automata//settings.json");
    // vector<Automata> v = j.ScrapingJson("../settings.json");

    for (int j = 0; j < v.size(); j++)
    {
        cout << v[j];
    }
    int istanti = 0;

    DEBUG_COMMENT("Questo è un commento di debug" << istanti << " \n\n\n");

    for (double time = 1; time < finaltime; time = time + delta)
    {

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
