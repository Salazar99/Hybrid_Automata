#include "../include/Objects.h"
#include <iostream>
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

bool Transition::checkCondition(unordered_map<string, double> variables)
{
    return 0;
}

ostream &operator<<(ostream &os, Transition &obj)
{
    os << "Transition: " << obj.getCondition();
    return os;
}
