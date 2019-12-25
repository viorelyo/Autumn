#pragma once
#include "IMultiplier.h"

class Multiplier : public IMultiplier
{
public:
    int Multiply(const int x, const  int y) override;
};