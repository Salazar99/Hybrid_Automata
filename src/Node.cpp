#include "../include/Objects.h"
#include <iostream>

Node::Node()
{
}

Node::Node(string name, string description)
{
    this->name = name;
    this->description = description;
}

string Node::getName()
{
    return this->name;
}

void Node::setName(string name)
{
    this->name = name;
}

string Node::getDescription()
{
    return this->description;
}

void Node::setDescription(string description)
{
    this->description = description;
}

unordered_map<Transition, Node, TransitionHash, TransitionEqual> Node::getTransitions()
{
    return this->transitions;
}

void Node::setTransitions(unordered_map<Transition, Node, TransitionHash, TransitionEqual> transitions)
{
    this->transitions = transitions;
}

bool Node::operator==(Node &other)
{
    return this->name == other.name;
}

void Node::addTransition(string condition, Node destination)
{
    Transition aux(condition);
    transitionKeys.push_back(aux);
    this->transitions[aux] = destination;
}

vector<Transition> Node::getTransitionKeys()
{
    return transitionKeys;
}

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

ostream &operator<<(ostream &os, Node &obj)
{
    os << "Name: " << obj.name << ", description: " << obj.description << ", transitions: \n";
    for (Transition t : obj.getTransitionKeys())
        os << "  " << t << " ----> " << obj.getTransitions()[t].getName() << "\n";
    return os;
}