#include <vector>
#include <unordered_map>
#include <string>
#include <iostream>
using namespace std;

class Transition
{
private:
    string condition;

public:
    Transition(string condition);
    string getCondition();
    void setCondition(string condition);
    bool evaluateSingleEquation(string &expression);
    char evaluateLogicalEquation(string str);
    bool solve(string str);
    bool checkCondition(unordered_map<string, double> variables);
    friend ostream &operator<<(ostream &os, Transition &obj);
};

struct TransitionHash
{
    std::size_t operator()(const Transition &t) const
    {
        std::hash<std::string> hasher;
        Transition tCopy = t;
        return hasher(tCopy.getCondition());
    }
};

struct TransitionEqual
{
    bool operator()(const Transition &t1, const Transition &t2) const
    {
        Transition t1Copy = t1;
        Transition t2Copy = t2;
        return t1Copy.getCondition() == t2Copy.getCondition();
    }
};

class Node
{
private:
    string name;
    string description;
    vector<Transition> transitionKeys;
    unordered_map<Transition, Node, TransitionHash, TransitionEqual> transitions;

    // vector<Transition> transitions;

public:
    Node();
    Node(string name, string description);
    string getName();
    void setName(string name);
    string getDescription();
    void setDescription(string description);
    unordered_map<Transition, Node, TransitionHash, TransitionEqual> getTransitions();
    vector<Transition> getTransitionKeys();
    void setTransitions(unordered_map<Transition, Node, TransitionHash, TransitionEqual> transitions);
    void addTransition(string condition, Node destination);
    Node checkTransitions(unordered_map<string, double> sharedVariables);
    bool operator==(Node &other);
    friend ostream &operator<<(ostream &os, Node &obj);
};

enum Status
{
    OFF,
    RUNNING,
    PAUSE
};

class Automata
{
private:
    vector<Node> nodes;
    Node initialNode;
    Node currentNode;
    vector<Node> finalNodes;
    unordered_map<string, double> automataVariables;
    Status currentStatus;

public:
    Automata(vector<Node> nodes, Node initialNode, vector<Node> finalNodes, unordered_map<string, double> automataVariables, Status status);
    vector<Node> getNodes();
    void setNodes(vector<Node> nodes);
    Node getInitialNode();
    void setInitialNode(Node initialNode);
    Node getCurrentNode();
    void setCurrentNode(Node currentNode);
    vector<Node> getFinalNodes();
    void setFinalNodes(vector<Node> finalNodes);
    unordered_map<string, double> getAutomataVariables();
    void setAutomataVariables(unordered_map<string, double> automataVaribles);
    Status getCurrentStatus();
    void setCurrentStatus(Status status);
    bool checkForChanges();
    friend ostream &operator<<(ostream &os, Automata &obj);
};
