#include "../include/Objects.h"
#include "../include/tools.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include "../include/tinyexpr.h"
#include <unordered_map>
#include <boost/numeric/odeint.hpp>
#include "../third_parties/exprtk/exprtk.hpp"

using namespace boost::numeric::odeint;

typedef std::vector<double> state_type;


#ifdef DEBUG_MODE
#define DEBUG_COMMENT(comment) std::cout << "[DEBUG] " << comment << std::endl;
#else
#define DEBUG_COMMENT(comment)
#endif

ExpressionEvaluator::ExpressionEvaluator(const std::string &expression, std::unordered_map<std::string, double> &variables)
{
  for(auto &var : variables)
    {
        set_variable(var.first, var.second);
    }
  expr.register_symbol_table(symbol_table);
        if (!parser.compile(expression, expr)) {
            std::cerr << "Error: Failed to parse expression: " << expression << std::endl;
            std::cerr << "Details: " << parser.error() << std::endl;
            valid = false;
        }
}

void ExpressionEvaluator::set_variable(const std::string &name, double &value)
{
 if (!symbol_table.add_variable(name, value)) {
            std::cerr << "Error: Failed to add variable: " << name << std::endl;
            valid = false;
        }
}

double ExpressionEvaluator::evaluate()
{
 if (!valid) {
            std::cerr << "Error: Invalid expression." << std::endl;
            return NAN;
        }
        return expr.value();}

// ODE system function using the ExpressionEvaluator
void system(const std::vector<double> &x, std::vector<double> &dxdt, double t, ExpressionEvaluator &evaluator, std::unordered_map<std::string, double> &variables) {
    // Update the time and state variables
    //variables["t"] = t;
    //variables["x"] = x[0];  // Assuming x[0] is the main variable for the ODE

    // Evaluate the expression to calculate dx/dt
    dxdt[0] = evaluator.evaluate();
}

// Function to solve the ODE with arbitrary expressions and variables
std::vector<double> solve_ode(const std::string &equation, double x0, double t0, double t1, double dt, std::unordered_map<std::string, double> &variables) {
    // Initialize the evaluator with the provided equation
    ExpressionEvaluator evaluator(equation,variables);

    // Set up initial conditions for the ODE solver
    std::vector<double> x(1, x0);  // Initial state

    // Integrate using the Runge-Kutta-Dopri5 method
    runge_kutta_dopri5<std::vector<double>> stepper;
    integrate_const(stepper,
                    [&](const std::vector<double> &x, std::vector<double> &dxdt, double t) {
                        system(x, dxdt, t, evaluator, variables);
                    },
                    x, t0, t1, dt);

    // Display the final result
    std::cout << "Final result: x(" << t1 << ") = " << x[0] << std::endl;
    return x;
}


/// @brief empty constructor
Node::Node()
{
}

/// @brief Constructor for Node class.
/// @param name The name of the node.
/// @param description The description of the node.
/// @param instructions The instructions associated with the node.
/// @param firstVisit Indicates whether the node is visited for the first time.
/// @param delta The time delta.
/// @param numSeconds The number of seconds.
Node::Node(string name, string description, string instructions, bool firstVisit, double delta, double numSeconds)
{
    this->name = name;
    this->description = description;
    this->instructions = instructions;
    this->firstVisit = firstVisit;
    this->delta = delta;
    this->numSeconds = numSeconds;
}

/// @brief returns the name of the node
/// @return Node.name
string Node::getName()
{
    return this->name;
}

/// @brief set the name of the node
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

/// @brief return the actual node istruction
/// @return the actual node istructions
string Node::getActualInstructions()
{
    return this->instructions;
}

/// @brief Sets new values for the node from a given unordered_map.
/// @param newValues The new values to set for the node.
void Node::setFileValues(unordered_map<string, double> newValues)
{
    for (const auto &pair : newValues)
    {
        fileValues[pair.first] = pair.second;
    }
}

/// @brief Solves an ordinary differential equation (ODE) using Euler's method.
/// @param eq The ODE equation to solve.
/// @param cauchy The initial value (Cauchy condition) for the equation.
/// @param t0 The initial time for solving the equation.
/// @param h The step size.
/// @param t_final The final time for solving the equation.
/// @param sharedVariables A map of shared variables used in the equation.
/// @return The solution of the ODE at the specified time.
double Node::ode_solver(string eq, double cauchy, int t0, double h, double t_final, unordered_map<string, double *> &sharedVariables)
{
    vector<string> aux = split_string(eq, '=');

    //equation part is store in copia
    string copia = aux[1];

    vector<string> var = split_string(eq, '\'');

    //get the equation string from the complete instruntion string
    std::string equation = aux[1];  // Complex example equation

    // Set up variables as a map with arbitrary names and avaiable values if possible 
    std::unordered_map<std::string, double> variables;

    // if the string has variables, then we insert them in the map with their values
    //FIXME
    //Now we are adding all the shared variables to the map, maybe we should add only the ones that are used in the equation but this needs a good parser
    for (pair<string, double *> pair : sharedVariables)
    {
            variables[pair.first] = *(pair.second);
    }
    double new_time;

    //aux[1] = replace_var(aux[1], var[0], to_string(map_ode_solver_values[var[0]][t0 - 1]));
    //aux[1] = replace_var(aux[1], "t", to_string(ode_solver_times[t0 - 1]));
    //double k1 = te_interp(aux[1].c_str(), 0);
    //new_value = map_ode_solver_values[var[0]][t0 - 1] + h * k1;
    
    new_time = ode_solver_times[t0 - 1] + h;
    ode_solver_times.push_back(new_time);

    //Now we need to update the map_ode_solver_values with the new values because this code is terrible and not modular at all
    map_ode_solver_values[var[0]].push_back(solve_ode(equation, map_ode_solver_values[var[0]][t0 - 1], new_time-h, new_time, h, variables)[0]);

    return map_ode_solver_values[var[0]][t0];
}

/// @brief Executes the instructions associated with the node.
/// @param sharedVariables A map of shared variables used in the instructions.
/// @param tempVariables A map to store temporary variables generated during execution.
/// @param time The current time step.
void Node::executeNodeInstructions(unordered_map<string, double *> &sharedVariables, unordered_map<string, double> &tempVariables, int time)
{
    // removing all the spaces
    instructions.erase(std::remove(instructions.begin(), instructions.end(), ' '), instructions.end());
    instructions.erase(std::remove(instructions.begin(), instructions.end(), '\n'), instructions.end());

    vector<string> distinctInstructions = split_string(instructions, ';'); // splitting at ; character
    vector<string> aux;
    double *value;
    for (string s : distinctInstructions)
    {

        if (s.find("'") != string::npos)
        {
            aux = split_string(s, '\'');
            if (fileValues.contains(aux[0]))
            {
                tempVariables[aux[0]] = fileValues[aux[0]];
                continue;
            }

            value = new double;

            if (firstVisit)
            {
                auto it = map_ode_solver_values.find(aux[0]);
                if (it != map_ode_solver_values.end())
                {
                    map_ode_solver_values[aux[0]].clear();
                }
                ode_solver_values.clear();
                ode_solver_times.clear();
                ode_solver_times.push_back(0.0);
                double *newCauchy = new double;
                *newCauchy = *sharedVariables[aux[0]];
                cauchy[aux[0]] = newCauchy;
                ode_solver_values.push_back(*cauchy[aux[0]]);
                map_ode_solver_values[aux[0]].push_back(*cauchy[aux[0]]);
            }
            *value = ode_solver(s, *cauchy[aux[0]], time, delta, numSeconds, sharedVariables);
            tempVariables[aux[0]] = *(value);
            delete (value);
            continue;
        }

        aux = split_string(s, '=');
        if (fileValues.contains(aux[0]))
        {
            tempVariables[aux[0]] = fileValues[aux[0]];
            continue;
        }

        // check if the instruction is a simple assignment
        if (aux[1].find("+") == string::npos && aux[1].find("-") == string::npos && aux[1].find("*") == string::npos && aux[1].find("/") == string::npos)
        {
            setlocale(LC_ALL, "C");
            //value = new double;
            //*value = stod(aux[1]);
            //tempVariables[aux[0]] = *(value);
            //delete (value);

            tempVariables[aux[0]] = stod(aux[1]);
        }
        else
        {
            // if the string has variables, then we replace their values
            for (pair<string, double *> pair : sharedVariables)
            {
                aux[1] = replace_var(aux[1], pair.first, to_string(*(pair.second)));
            }
            //value = new double;
            //*value = te_interp(aux[1].c_str(), 0); // solve the instruction
            //tempVariables[aux[0]] = *(value);
            //delete (value);

            tempVariables[aux[0]] = te_interp(aux[1].c_str(), 0); // solve the instruction
        }
    }
    firstVisit = false;
}

/// @brief Checks the conditions of transitions associated with the node.
/// @param sharedVariables A map of shared variables used in the conditions.
/// @param tempVariables A map of temporary variables generated during execution.
/// @return The name of the node to transition to based on the satisfied condition, or the current node's name if no condition is satisfied.
string Node::checkTransitions(unordered_map<string, double *> &sharedVariables, unordered_map<string, double> &tempVariables)
{
    for (Transition t : getTransitionKeys())
    {
        if (t.checkCondition(sharedVariables, tempVariables))
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
