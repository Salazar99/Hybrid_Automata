#include "../include/graph.h"
#include <unordered_map>
#include <iostream>

using namespace std;

#ifdef DEBUG_MODE
#define DEBUG_COMMENT(comment) std::cout << "[DEBUG] " << comment << std::endl;
#else
#define DEBUG_COMMENT(comment)
#endif

GraphNode::GraphNode(string name)
{
    this->nodeName = name;
}

string GraphNode::getName()
{
    return this->nodeName;
}

GraphEdge::GraphEdge(GraphNode *source, GraphNode *target)
{
    this->source = source;
    this->target = target;
}

void Graph::addNode(GraphNode *node)
{
    this->nodes.push_back(node);
}

void Graph::addEdge(GraphNode *source, GraphNode *target)
{
    GraphEdge *edge = new GraphEdge(source, target);
    source->outgoingEdges.push_back(edge);
}

vector<GraphNode *> Graph::topologicalSort()
{
    vector<GraphNode *> sortedOrder;
    stack<GraphNode *> stack;
    unordered_map<string, int> visited;

    for (GraphNode *node : nodes)
    {
        DEBUG_COMMENT("Inizia Visita " << node->nodeName);
        if (visited[node->nodeName] == 0 && !visitedNode(node, visited, stack))
        {
            DEBUG_COMMENT("Nodo Ciclico: " << node->nodeName << "\n");
            cerr << "Error: Graph contains a cycle!" << endl;
            return {}; // Handle cyclic graph
        }
    }

    while (!stack.empty())
    {
        sortedOrder.push_back(stack.top());
        stack.pop();
    }

    return sortedOrder;
}

bool Graph::visitedNode(GraphNode *node, unordered_map<string, int> &visited, stack<GraphNode *> &stack)
{
    DEBUG_COMMENT("Visiting: " << node->nodeName << "\n");

    if (visited[node->nodeName] == 1)
    {
        return false; // Detected a back edge, indicating a cycle
    }

    if (visited[node->nodeName] == 2)
    {
        return true; // Already visited
    }

    visited[node->nodeName] = 1;

    for (GraphEdge *edge : node->outgoingEdges)
    {
        if (!visitedNode(edge->target, visited, stack))
        {
            return false; // Cyclic graph
        }
    }

    visited[node->nodeName] = 2;
    stack.push(node);

    return true;
}