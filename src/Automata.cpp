#include "../include/Objects.h"
#include <iostream>

Automata::Automata(vector<Node> nodes, Node initialNode, vector<Node> finalNodes, unordered_map<string, double> automataVariables, Status status)
{
    this->nodes = nodes;
    this->initialNode = initialNode;
    this->finalNodes = finalNodes;
    this->automataVariables = automataVariables;
    this->currentStatus = status;
}

vector<Node> Automata::getNodes()
{
    return this->nodes;
}

void Automata::setNodes(vector<Node> nodes)
{
    this->nodes = nodes;
}

Node Automata::getInitialNode()
{
    return this->initialNode;
}

void Automata::setInitialNode(Node initialNode)
{
    this->initialNode = initialNode;
}

Node Automata::getCurrentNode()
{
    return this->currentNode;
}

void Automata::setCurrentNode(Node currentNode)
{
    this->currentNode = currentNode;
}
vector<Node> Automata::getFinalNodes()
{
    return this->finalNodes;
}

void Automata::setFinalNodes(vector<Node> finalNodes)
{
    this->finalNodes = finalNodes;
}

unordered_map<string, double> Automata::getAutomataVariables()
{
    return this->automataVariables;
}

void Automata::setAutomataVariables(unordered_map<string, double> autoamtaVariables)
{
    this->automataVariables = autoamtaVariables;
}

Status Automata::getCurrentStatus()
{
    return this->currentStatus;
}

void Automata::setCurrentStatus(Status status)
{
    this->currentStatus = status;
}

bool Automata::checkForChanges()
{
    Node newNode = currentNode.checkTransitions(automataVariables);
    if (!(newNode == currentNode))
    {
        setCurrentNode(newNode);
        return 1;
    }
    return 0;
}

ostream &operator<<(ostream &os, Automata &obj)
{
    os << "Nodes: \n";
    for (Node n : obj.getNodes())
    {
        os << "- " << n << "\n";
    }
    os << "Initial Node: " << obj.getInitialNode().getName() << "\n";
    os << "Current Node: " << obj.getCurrentNode().getName() << "\n";
    os << "Final Nodes: \n";
    for (Node n : obj.getFinalNodes())
    {
        os << "- " << n.getName() << "\n";
    }
    os << "Status: " << obj.getCurrentStatus() << "\n";
    return os;
}
