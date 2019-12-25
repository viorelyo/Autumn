#pragma once
#include "IAdder.h"

class Adder : public IAdder
{
public:
    int Add(const int x, const int y) override;
};