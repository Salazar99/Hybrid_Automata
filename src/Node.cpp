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
Node::Node(string name, string description, string instructions)
{
    this->name = name;
    this->description = description;
    this->instructions = instructions;
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

/// @brief execute all the node instructions
/// @param sharedVariables the system variables
void Node::executeNodeInstructions(unordered_map<string, double *> &sharedVariables)
{
    // removing all the spaces
    instructions.erase(std::remove(instructions.begin(), instructions.end(), ' '), instructions.end());
    instructions.erase(std::remove(instructions.begin(), instructions.end(), '\n'), instructions.end());

    /*cout << "\n"
         << instructions << "\n";
    cout << "Variables: "
         << "\n";*/

    vector<string> distinctInstructions = split(instructions, ';'); // splitting at ; character
    vector<string> aux;
    double *value;
    for (string s : distinctInstructions)
    {
        cout << s << "\n";
        aux = split(s, '='); // aux[0] = leftoperand -- aux[1] = rightoperand

        // check if the instruction is a simple assignment
        if (aux[1].find("+") == string::npos && aux[1].find("-") == string::npos && aux[1].find("*") == string::npos && aux[1].find("/") == string::npos)
        {
            value = new double;
            *value = stod(aux[1]);
            sharedVariables[aux[0]] = value; // insert or assign the value
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
    cout << "CheckTransitions, name: " << getName() << ", size transitions: " << transitionKeys.size() << "\n";
    for (Transition t : getTransitionKeys())
    {

        if (t.checkCondition(sharedVariables))
        {
            return transitions[t];
        }
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