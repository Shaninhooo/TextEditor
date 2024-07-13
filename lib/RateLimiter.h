#include <iostream>
#include <chrono>
#include <thread>

class RateLimiter {
public:
    RateLimiter(int timeout_ms);

    bool canCall();

    void updateLastCallTime();

private:
    int timeout; // Timeout duration in milliseconds
    std::chrono::steady_clock::time_point lastCallTime;
};
