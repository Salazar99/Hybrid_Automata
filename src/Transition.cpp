#include "../include/Objects.h"
#include <iostream>
#include <stack>

Transition::Transition(string condition)
{
    this->condition = condition;
}

string Transition::getCondition()
{
    return condition;
}

void Transition::setCondition(string condition)
{
    this->condition = condition;
}

// Evaluate the single expression like "(5>9)" --> 0 (false))
bool Transition::evaluateSingleEquation(string &expression)
{

    size_t pos = expression.find("<=");
    if (pos != string::npos)
    {
        string left = expression.substr(0, pos);
        string right = expression.substr(pos + 2);
        double a = stod(left);
        double b = stod(right);
        return a <= b;
    }

    pos = expression.find(">=");
    if (pos != string::npos)
    {
        string left = expression.substr(0, pos);
        string right = expression.substr(pos + 2);
        double a = stod(left);
        double b = stod(right);
        return a >= b;
    }

    pos = expression.find(">");
    if (pos != string::npos)
    {
        string left = expression.substr(0, pos);
        string right = expression.substr(pos + 1);
        double a = stod(left);
        double b = stod(right);
        return a > b;
    }

    pos = expression.find("<");
    if (pos != string::npos)
    {
        string left = expression.substr(0, pos);
        string right = expression.substr(pos + 1);
        double a = stod(left);
        double b = stod(right);
        return a < b;
    }

    pos = expression.find("==");
    if (pos != string::npos)
    {
        string left = expression.substr(0, pos);
        string right = expression.substr(pos + 2);
        double a = stod(left);
        double b = stod(right);
        return a == b;
    }

    pos = expression.find("!=");
    if (pos != string::npos)
    {
        string left = expression.substr(0, pos);
        string right = expression.substr(pos + 2);
        double a = stod(left);
        double b = stod(right);
        return a != b;
    }

    // Return false for invalid expressions
    return false;
}

// Evaluate ONLY logical expression with true false like "(1 & 1) | (1 | 0)"
char Transition::evaluateLogicalEquation(string str)
{
    stack<char> arr;

    // traversing string from the end.
    for (int i = str.length() - 1; i >= 0; i--)
    {
        if (str[i] == '[')
        {
            vector<char> s;
            while (arr.top() != ']')
            {
                s.push_back(arr.top());
                arr.pop();
            }
            arr.pop();

            // for NOT operation
            if (s.size() == 3)
            {
                s[2] == '1' ? arr.push('0') : arr.push('1');
            }
            // for AND and OR operation
            else if (s.size() == 5)
            {
                int a = s[0] - 48, b = s[4] - 48, c;
                s[2] == '&' ? c = a &&b : c = a || b;
                arr.push((char)c + 48);
            }
        }
        else
        {
            arr.push(str[i]);
        }
    }
    return arr.top();
}

// Scraping the initial equation to obtain a simple logical expression and then solve it
bool Transition::solve(string str)
{
    string expressionFinal = "";
    string expressionTemp = "";
    int flag = 0;
    for (char c : str)
    {
        if (c == '(') // Identifying the beginning of comparison between numbers
        {
            flag = 1;
        }
        else if (c == ')') // Identifying the ending of comparison between numbers
        {
            flag = 0;
            expressionFinal += evaluateSingleEquation(expressionTemp) ? "1" : "0";
            expressionTemp = "";
        }

        if (flag) // build the string with two numbers to be comparised
        {
            if (c != '(')
                expressionTemp += c;
        }
        else // build the final logic expression string to be analyzed
        {
            if (c != ')')
                expressionFinal += c;
        }
    }
    return evaluateLogicalEquation(expressionFinal) == '1' ? 1 : 0;
}

string preProcessing(string str)
{
    string newStr = "";
    for (char c : str)
    {
        newStr += c;
        if (c == '&' || c == '|')
        {
            newStr[newStr.size() - 1] = ',';
            newStr += c;
            newStr += ',';
        }
    }
    return newStr;
}

bool Transition::checkCondition(unordered_map<string, double *> &variables)
{
    return solve(preProcessing(getCondition()));
}

ostream &operator<<(ostream &os, Transition &obj)
{
    os << obj.getCondition();
    return os;
}
