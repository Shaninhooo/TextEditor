#include "../lib/RateLimiter.h"

RateLimiter::RateLimiter(int timeout_ms)
        : timeout(timeout_ms), lastCallTime(std::chrono::steady_clock::now() - std::chrono::milliseconds(timeout_ms)) {}

bool RateLimiter::canCall() {
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastCallTime);
    return duration.count() >= timeout;
}

void RateLimiter::updateLastCallTime() {
    lastCallTime = std::chrono::steady_clock::now();
}