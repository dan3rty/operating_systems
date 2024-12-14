#pragma once

#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>

using namespace std::literals;
using Clock = std::chrono::system_clock;

inline std::string CurrentTimeToString()
{
	auto t = Clock::to_time_t(Clock::now());
	tm buf{};
#ifdef _MSC_VER
	localtime_s(&buf, &t);
#else
	localtime_r(&t, &buf);
#endif
	std::ostringstream os;
	os << std::setw(2) << std::setfill('0') << buf.tm_hour << ":"
	   << std::setw(2) << std::setfill('0') << buf.tm_min << ":"
	   << std::setw(2) << std::setfill('0') << buf.tm_sec;
	return os.str();
}