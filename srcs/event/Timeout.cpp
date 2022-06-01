#include "event/Timeout.hpp"

#include <ctime>

Timeout::Timeout() {}

void Timeout::update_last_time_event();
bool Timeout::is_timeout_over();