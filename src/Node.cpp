#include "../include/Objects.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include "../include/tinyexpr.h"

/// @brief empty constructor
Node::Node()
{
}

/// @brief constructor
/// @param name the name of the node
/// @param description the description of the node
Node::Node(string name, string description, string instructions, bool firstVisit)
{
    this->name = name;
    this->description = description;
    this->instructions = instructions;
    this->firstVisit = firstVisit;
}

/// @brief returns the name of the node
/// @return Node.name
string Node::getName()
{
    return this->name;
}

/// @brief change the name of the node
/// @param name the new name
void Node::setName(string name)
{
    this->name = name;
}

/// @brief returns the description of the node
/// @return Node.description
string Node::getDescription()
{
    return this->description;
}

/// @brief change the description of the node
/// @param description the new description
void Node::setDescription(string description)
{
    this->description = description;
}

/// @brief returns all the transitions of the node
/// @return Node.transitions
unordered_map<Transition, string, TransitionHash, TransitionEqual> Node::getTransitions()
{
    return this->transitions;
}

/// @brief change the transitions of the node
/// @param transitions the new transitions
void Node::setTransitions(unordered_map<Transition, string, TransitionHash, TransitionEqual> &transitions)
{
    this->transitions = transitions;
}

/// @brief override of the '==' operator
/// @param other the other object
/// @return true if equals, false otherwise
bool Node::operator==(const Node &other)
{
    return this->name == other.name;
}

/// @brief adds a transition to the transitions of the node
/// @param condition
/// @param destination
void Node::addTransition(string condition, string destination)
{
    Transition *aux = new Transition(condition);
    this->transitionKeys.push_back(*aux);
    this->transitions[*aux] = destination;
}

/// @brief returns all the transitions' condition
/// @return Node.transitionKeys
vector<Transition> Node::getTransitionKeys()
{
    return transitionKeys;
}

/// @brief set the value of firstVisit
/// @param value the value to assign
void Node::setFirstVisit(bool value)
{
    this->firstVisit = value;
}

/// @brief return the value of firstVisit
/// @return value if firstVisit
bool Node::getFirstVisit()
{
    return this->firstVisit;
}

/// @brief splits the string into different parts given a delimiter and returns a vector of substrings;
/// @param s the string to split
/// @param delimiter the delimiter
/// @return a vector of substrings
std::vector<std::string> split(const std::string &s, char delimiter)
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
void printMap(unordered_map<string, double *> &sharedVariables)
{
    for (auto &pair : sharedVariables)
    {
        std::cout << pair.first << ": " << *(pair.second) << "\n";
    }
}

/// @brief resolve a first order differential equation
/// @param eq the equation
/// @param cauchy initial condition
/// @param t0 aim moment
/// @param h delta
/// @param t_final final time
/// @return
double Node::ode_solver(string eq, double cauchy, int t0, double h, double t_final)
{
    int num_steps = static_cast<int>(t_final / h) + 1;
    vector<double> t(t0 + 1);
    vector<double> ystar(t0 + 1);
    t[0] = 0.0;
    ystar[0] = cauchy;
    vector<string> aux = split(eq, '=');

    // cout << "aux[0]: " << aux[0] << ", [1]: " << aux[1] << "\n";

    string copia = aux[1];

    vector<string> var = split(eq, '\'');

    int i;
    for (i = 0; (i < num_steps - 1) && (i < t0); ++i)
    {
        // replacing variables with their actual values
        int pos = aux[1].find(var[0]);
        while (pos != string::npos)
        {
            aux[1].replace(pos, var[0].length(), to_string(ystar[i]));
            pos = aux[1].find(var[0]);
        }

        pos = aux[1].find("t");
        while (pos != string::npos)
        {
            aux[1].replace(pos, 1, to_string(t[i]));
            pos = aux[1].find("t");
        }

        double k1 = te_interp(aux[1].c_str(), 0);
        ystar[i + 1] = ystar[i] + h * k1;
        t[i + 1] = t[i] + h;
        aux[1] = copia;
    }

    return ystar[i];
}

/// @brief execute all the node instructions
/// @param sharedVariables the system variables
void Node::executeNodeInstructions(unordered_map<string, double *> &sharedVariables, int time)
{
    // removing all the spaces
    instructions.erase(std::remove(instructions.begin(), instructions.end(), ' '), instructions.end());
    instructions.erase(std::remove(instructions.begin(), instructions.end(), '\n'), instructions.end());

    vector<string> distinctInstructions = split(instructions, ';'); // splitting at ; character
    vector<string> aux;
    double *value;
    for (string s : distinctInstructions)
    {
        // cout << s << "\n";

        if (s.find("'") != string::npos)
        {
            aux = split(s, '\'');

            // cout << "aux[0]: " << aux[0] << "\n";

            value = new double;
            // cout << "First Visit: " << getFirstVisit() << "\n";
            if (firstVisit)
            {
                // cout << "First Visit, new cauchy: " << *sharedVariables[aux[0]] << "\n";
                double *newCauchy = new double;
                *newCauchy = *sharedVariables[aux[0]];
                cauchy[aux[0]] = newCauchy;
                firstVisit = false;
            }
            *value = ode_solver(s, *cauchy[aux[0]], time, 0.1, 1000000);
            // cout << "New Value X: " << *value << "\n";
            sharedVariables[aux[0]] = value;
            continue;
        }

        // aux[0] = leftoperand -- aux[1] = rightoperand
        aux = split(s, '=');

        // check if the instruction is a simple assignment
        if (aux[1].find("+") == string::npos && aux[1].find("-") == string::npos && aux[1].find("*") == string::npos && aux[1].find("/") == string::npos)
        {
            value = new double;
            *value = stod(aux[1]);
            sharedVariables[aux[0]] = value;
        }
        else
        {
            // if the string has variables, then we replace their values
            for (pair<string, double *> pair : sharedVariables)
            {
                int pos = aux[1].find(pair.first);
                while (pos != string::npos)
                {
                    aux[1].replace(pos, pair.first.length(), to_string(*(pair.second)));
                    pos = aux[1].find(pair.first);
                }
            }
            value = new double;
            *value = te_interp(aux[1].c_str(), 0); // solve the instruction
            sharedVariables[aux[0]] = value;       // insert or assign the value
        }
    }
    printMap(sharedVariables);
}

/// @brief checks if any transition is satisfied
/// @param sharedVariables the variables of the automata
/// @return Node (the new current node)
string Node::checkTransitions(unordered_map<string, double *> &sharedVariables)
{
    // cout << "CheckTransitions, name: " << getName() << ", size transitions: " << transitionKeys.size() << "\n";
    for (Transition t : getTransitionKeys())
    {
        if (t.checkCondition(sharedVariables))
            return transitions[t];
    }
    return this->getName();
}

/// @brief override of the '<<' operator
/// @param os the current stream
/// @param obj the object to print
/// @return the new current stream
ostream &operator<<(ostream &os, Node &obj)
{
    os << "Name: " << obj.name << ", description: " << obj.description << ", transitions: \n";
    for (Transition t : obj.getTransitionKeys())
        os << "  " << t << " ----> " << obj.getTransitions()[t] << "\n";
    return os;
}