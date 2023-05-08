#include "shared_function.h"

double randomDouble(double lowerLimit, double upperLimit)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(lowerLimit, upperLimit);
    double result = dis(gen);
    return result;
}

int getDownloadSpeed(double rtt, int packet_size)
{
    double factor = 1.0 / rtt;
    double double_size = packet_size;
    double speed = double_size * factor;
    int result = int(speed);
    return result;
}
