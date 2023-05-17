#include "shared_function.h"

double randomDouble(double lowerLimit, double upperLimit)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(lowerLimit, upperLimit);
    double result = dis(gen);
    return result;
}

double calculateRTO(double rtt, bool isSpurious) {
    double alpha = 0.125;
    double gamma = 0.03125;
    double SRTT = rtt;
    int K = 4;
    double RTTVAR = rtt / 2;
    double RTO;

    // update SRTT
    SRTT = (1 - alpha) * SRTT + alpha * rtt;

    // update RTTVAR
    if (rtt > SRTT) {
        RTTVAR = (1 - alpha) * RTTVAR + alpha * abs(SRTT - rtt);
    }
    else {
        RTTVAR = (1 - gamma) * RTTVAR + gamma * abs(SRTT - rtt);
    }

    // update K
    if (isSpurious == true) {
        K = 7;
    }
    else {
        K = 4;
    }

    // calculate RTO
    RTO = SRTT + K * RTTVAR;
    return RTO;
}
