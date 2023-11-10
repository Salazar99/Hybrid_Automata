#include "../include/Objects.h"
#include <iostream>

/// @brief empty constructor
Node::Node()
{
}

/// @brief constructor
/// @param name the name of the node
/// @param description the description of the node
Node::Node(string name, string description)
{
    this->name = name;
    this->description = description;
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
unordered_map<Transition, Node, TransitionHash, TransitionEqual> Node::getTransitions()
{
    return this->transitions;
}

/// @brief change the transitions of the node
/// @param transitions the new transitions
void Node::setTransitions(unordered_map<Transition, Node, TransitionHash, TransitionEqual> transitions)
{
    this->transitions = transitions;
}

/// @brief override of the '==' operator
/// @param other the other object
/// @return true if equals, false otherwise
bool Node::operator==(Node &other)
{
    return this->name == other.name;
}

/// @brief adds a transition to the transitions of the node
/// @param condition
/// @param destination
void Node::addTransition(string condition, Node destination)
{
    Transition aux(condition);
    transitionKeys.push_back(aux);
    this->transitions[aux] = destination;
}

/// @brief returns all the transitions' condition
/// @return Node.transitionKeys
vector<Transition> Node::getTransitionKeys()
{
    return transitionKeys;
}

/// @brief checks if any transition is satisfied
/// @param sharedVariables the variables of the automata
/// @return Node (the new current node)
Node Node::checkTransitions(unordered_map<string, double> sharedVariables)
{
    for (Transition t : transitionKeys)
    {
        if (t.checkCondition(sharedVariables))
        {
            return transitions[t];
        }
    }

    return *this;
}

/// @brief override of the '<<' operator
/// @param os the current stream
/// @param obj the object to print
/// @return the new current stream
ostream &operator<<(ostream &os, Node &obj)
{
    os << "Name: " << obj.name << ", description: " << obj.description << ", transitions: \n";
    for (Transition t : obj.getTransitionKeys())
        os << "  " << t << " ----> " << obj.getTransitions()[t].getName() << "\n";
    return os;
}