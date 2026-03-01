#include "Context.h"

Context::Context(int number)
{
    this->number = number;
}

int Context::get_number()
{
    return this->number;
}