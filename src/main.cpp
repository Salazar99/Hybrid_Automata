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
#ifdef WINDOWS
    System s = j.ScrapingJson("..//..//watertanks.json");
#else
    System s = j.ScrapingJson("../watertanks.json");
#endif
    vector<Automata> v = s.getAutomata();
    cout << s;

    int istanti = 0;

    DEBUG_COMMENT("Questo è un commento di debug" << istanti << " \n\n\n");

    try
    {
#ifdef WINDOWS
        csvfile csv("../../export.csv", true);
#else
        csvfile csv("../export.csv", true);
#endif
        // throws exceptions!
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

    for (double time = 1; time < finaltime + 1 - delta; time = time + delta)
    {
        cout << "################## TIME = " << time << " ##################\n";

        // executing all automatas instructions and checking for possible transitions
        for (int j = 0; j < v.size(); j++)
        {
            v[j].checkForChanges();
            // cout << "\nAutoma " << v[j].getName() << " ,Nodo attuale: " << v[j].getCurrentNode().getName() << "\n";
        }

        // refreshing AutomataVariables
        s.refreshVariables();

        cout << "\nVariables Map: \n";
        printMap(*v[0].getAutomataVariables());
        try
        {
#ifdef WINDOWS
            csvfile csv("../../export.csv", false);
#else
            csvfile csv("../export.csv", false);
#endif
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
