#include "../include/global_variables.h"
#include "../include/UtilsJson.h"
#include "../include/tinyexpr.h"
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

    // System s = j.ScrapingJson("C://Users//aleal//Desktop//evrthng//Hybrid_Automata//secondexample.json");
    //  System s = j.ScrapingJson("C://Users//tomvi//Hybrid_Automata//settings.json");
    System s = j.ScrapingJson("../thirdexample.json");
    vector<Automata> v = s.getAutomata();
    cout << s;

    int istanti = 0;

    DEBUG_COMMENT("Questo è un commento di debug" << istanti << " \n\n\n");

    try // adding global variables and single automatas' variables' name in the first row
    {
        // csvfile csv("../src/export.csv", true);
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

        // execution of every automatas in topological order
        for (int j = 0; j < v.size(); j++)
        {
            v[j].checkForChanges();
            // cout << "\nAutoma " << v[j].getName() << " ,Nodo attuale: " << v[j].getCurrentNode().getName() << "\n";
        }

        // refreshing AutomataVariables
        s.refreshVariables();

        cout << "\nVariables Map: \n";
        printMap(*v[0].getAutomataVariables());

        // adding global variables and single automatas' variables' name in the first row
        try
        {
            // csvfile csv("../src/export.csv", false);
            csvfile csv("../../src/export.csv", false); // throws exceptions!
            csv << time;
            for (auto const &key : s.getAutomataDependence())
            {

                // if the variable has not already been assigned then i print 0
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
