#include "shared_function.h"

double randomDouble(double lowerLimit, double upperLimit)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(lowerLimit, upperLimit);
    double result = dis(gen);
    std::cout << std::setprecision(3) << std::fixed << result << std::endl;
    return result;
}

int getDownloadSpeed(double rtt, int packet_size)
{
    double factor = 1.0 / rtt;
    double double_size = packet_size;
    double speed = double_size * factor;
    int result = int(double_size);
    return result;
}
