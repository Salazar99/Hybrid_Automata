#include "../include/Objects.h"
#include <iostream>

/// @brief constructor
/// @param nodes the nodes of the automata
/// @param initialNodes the initial node of the automata
/// @param finalNodes the final nodes of the automata
/// @param automataVariables the variables of the automata
/// @param status the curren status of the automata
Automata::Automata(vector<Node> nodes, Node initialNode, vector<Node> finalNodes, unordered_map<string, double *> &automataVariables, Status status)
{
    this->nodes = nodes;

    for (int i = 0; i < nodes.size(); i++)
    {
        nodesNames[nodes.at(i).getName()] = this->nodes.at(i);
    }

    this->initialNode = initialNode;
    this->currentNode = initialNode;
    this->finalNodes = finalNodes;
    this->automataVariables = automataVariables;
    this->currentStatus = status;
}

/// @brief return the nodes of the automata
/// @return the nodes of the automata
vector<Node> Automata::getNodes()
{
    return this->nodes;
}

/// @brief set the automata's nodes
/// @param nodes the nodes to set
void Automata::setNodes(vector<Node> &nodes)
{
    this->nodes = nodes;
}

/// @brief return the initial node of the automata
/// @return the initial node
Node Automata::getInitialNode()
{
    return this->initialNode;
}

/// @brief set the initial node of the automata
/// @param initialNode the node to set
void Automata::setInitialNode(Node &initialNode)
{
    this->initialNode = initialNode;
}

/// @brief return the current node of the automata
/// @return the current node
Node Automata::getCurrentNode()
{
    return this->currentNode;
}

/// @brief set the current node of the automata
/// @param currentNode the node to set
void Automata::setCurrentNode(Node &currentNode)
{
    this->currentNode = currentNode;
}

/// @brief return the final nodes of the automata
/// @return the final nodes
vector<Node> Automata::getFinalNodes()
{
    return this->finalNodes;
}

/// @brief set the final nodes of the automata
/// @param finalNodes the nodes to set
void Automata::setFinalNodes(vector<Node> &finalNodes)
{
    this->finalNodes = finalNodes;
}

/// @brief return the automata variables
/// @return the automata variables
unordered_map<string, double *> Automata::getAutomataVariables()
{
    return this->automataVariables;
}

/// @brief set the map string->double of the variables
/// @param autoamtaVariables the new map
void Automata::setAutomataVariables(unordered_map<string, double *> &autoamtaVariables)
{
    this->automataVariables = autoamtaVariables;
}

/// @brief return the current status
/// @return the current status
Status Automata::getCurrentStatus()
{
    return this->currentStatus;
}

/// @brief set the current status
/// @param status the new status
void Automata::setCurrentStatus(Status status)
{
    this->currentStatus = status;
}

/// @brief return the map string->nodes
/// @return the current map
unordered_map<string, Node> Automata::getNodesNames()
{
    return this->nodesNames;
}

/// @brief set the map of names->nodes
/// @param nodeNames the nnew map
void Automata::setNodesNames(unordered_map<string, Node> &nodeNames)
{
    this->nodesNames = nodeNames;
}

/// @brief check if we are in the conditions to change the current node
/// @return 1 if the current node changed, 0 otherwise
bool Automata::checkForChanges()
{

    string newNode = currentNode.checkTransitions(automataVariables);

    nodesNames[newNode].executeNodeInstructions(automataVariables);

    if (newNode == currentNode.getName())
        return 0;

    setCurrentNode(nodesNames[newNode]);
    return 1;

    /*for (Node c : nodes)
    {
        if (c.getName() == newNode)
        {
            setCurrentNode(c);
            return 1;
        }
    }*/
}

/// @brief to string
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
    os << "Status: " << (obj.getCurrentStatus() == 0 ? "OFF" : (obj.getCurrentStatus() == 1 ? "RUNNING" : "PAUSE")) << "\n";
    return os;
}
