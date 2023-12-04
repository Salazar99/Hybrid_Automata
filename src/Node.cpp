#include "../include/Objects.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>

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

void printMap(unordered_map<string, double *> &sharedVariables)
{
    for (auto &pair : sharedVariables)
    {
        std::cout << pair.first << ": " << *(pair.second) << "\n";
    }
}

/// @brief evaluate the single expression like "(5>9)" --> 0 (false))
/// @param expression the expression to evaluate
/// @return the evaluation of the expression
string evaluateSingleEquation(string expression)
{

    size_t pos = expression.find("+");
    if (pos != string::npos)
    {
        string left = expression.substr(0, pos);
        string right = expression.substr(pos + 1);
        double a = stod(left);
        double b = stod(right);
        return to_string(a + b);
    }

    pos = expression.find("-");
    if (pos != string::npos)
    {
        string left = expression.substr(0, pos);
        string right = expression.substr(pos + 1);
        double a = stod(left);
        double b = stod(right);
        return to_string(a - b);
    }

    pos = expression.find("*");
    if (pos != string::npos)
    {
        string left = expression.substr(0, pos);
        string right = expression.substr(pos + 1);
        double a = stod(left);
        double b = stod(right);
        return to_string(a * b);
    }

    pos = expression.find("/");
    if (pos != string::npos)
    {
        string left = expression.substr(0, pos);
        string right = expression.substr(pos + 1);
        double a = stod(left);
        double b = stod(right);
        return to_string(a / b);
    }

    // Return false for invalid expressions
    return "ao";
}

/// @brief Scraping the initial equation to obtain a simple mathematical expression and then solve it
/// @param str the initial equation
/// @return the evaluation
double solve(string str)
{
    string expressionFinal = "";
    string expressionTemp = "";
    int flag = 0;
    int i = 0;
    char c;
    while (str.find("(") != string::npos)
    {
        c = str[i];
        if (c == '(') // Identifying the beginning of comparison between numbers
        {
            flag = 1;
        }
        else if (c == ')') // Identifying the ending of comparison between numbers
        {
            flag = 0;

            cout << str << "\n";

            // expressionFinal += evaluateSingleEquation(expressionTemp) ? "1" : "0";
            str.replace(str.find(expressionTemp) - 1, expressionTemp.length() + 2, evaluateSingleEquation(expressionTemp));

            cout << str << "\n";
            i = -1;
            expressionTemp = "";
        }

        if (flag) // build the string with two numbers to be comparised
        {
            if (c != '(')
                expressionTemp += c;
        }
        i++;
    }
    return stod(str);
}

void Node::executeNodeInstructions(unordered_map<string, double *> &sharedVariables)
{
    // removing all the spaces
    instructions.erase(std::remove(instructions.begin(), instructions.end(), ' '), instructions.end());
    instructions.erase(std::remove(instructions.begin(), instructions.end(), '\n'), instructions.end());

    cout << "\n"
         << instructions << "\n";
    cout << "Variables: "
         << "\n";
    printMap(sharedVariables);

    vector<string> distinctInstructions = split(instructions, ';'); // splitting at ; character
    vector<string> aux;
    double *value;
    for (string s : distinctInstructions)
    {
        cout << s << "\n";
        aux = split(s, '='); // aux[0] = leftoperand -- aux[1] = rightoperand

        if (aux[1].find("+") == string::npos && aux[1].find("-") == string::npos && aux[1].find("*") == string::npos && aux[1].find("/") == string::npos)
        {
            value = new double;
            *value = stod(aux[1]);
            sharedVariables[aux[0]] = value; // insert or assign the value
        }
        else
        {

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
            *value = solve(aux[1]);
            sharedVariables[aux[0]] = value;
        }
    }
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