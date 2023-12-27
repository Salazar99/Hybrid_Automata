#include <string>
#include <vector>
#include <stack>
#include <unordered_map>

using namespace std;

// Forward declaration of GraphEdge class
class GraphEdge;

/// @brief class that represents a node in the graph
class GraphNode
{
public:
    string nodeName;
    vector<GraphEdge *> outgoingEdges;
    GraphNode(string name);
    string getName();
};

/// @brief class that represents a edge in the graph
class GraphEdge
{
public:
    GraphNode *source;
    GraphNode *target;
    GraphEdge(GraphNode *source, GraphNode *target);
};

/// @brief class that represents the automatas as a dag
class Graph
{
private:
    bool visitedNode(GraphNode *node, unordered_map<string, int> &visited, stack<GraphNode *> &stack);

public:
    vector<GraphNode *> nodes;
    void addNode(GraphNode *node);
    void addEdge(GraphNode *source, GraphNode *target);
    vector<GraphNode *> topologicalSort();
};
