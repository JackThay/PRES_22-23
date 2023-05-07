#include "shared_function.h"

//using namespace std;

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

/*
int main() {
    double rtt = 100; // initial RTT value
    bool isSpurious = false; // initial value of spurious status

    // measure new RTT values and calculate RTO
    //for (int i = 0; i < 1000; i++) {
        double newrtt = // measure new RTT value
        RTO = calculateRTO(newrtt,isSpurious);

        // check if transmission is spurious
        if (newrtt > RTO) {
            isSpurious = true;
        }
        else {
            isSpurious = false;
        }

        // update R and print RTO
        rtt = newrtt;
        cout << "RTO: " << RTO <<"\n";
    //}

    return 0;
}
*/
