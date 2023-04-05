#include "shared_function.h"

double randomDouble(double lowerLimit, double upperLimit)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(lowerLimit, upperLimit);
    double result = dis(gen);
    return result;
}
