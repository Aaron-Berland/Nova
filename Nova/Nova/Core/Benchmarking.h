#pragma once
#ifndef BENCHMARKING_H
#define BENCHMARKING_H
#include <Nova/Log.h>
#include <Nova/Core.h>
#include <chrono>
#include <string>
namespace Nova{
	class Timer
	{
	public:
		Timer(const std::string &s)
		{
			Nova::Log::GetCoreLogger()->trace("Starting Measuring {0} ", s);
			m_startTimePoint = std::chrono::high_resolution_clock::now();
		}

		~Timer()
		{

			auto endTimePoint = std::chrono::high_resolution_clock::now();
			auto startMicro = std::chrono::time_point_cast<std::chrono::microseconds>(m_startTimePoint).time_since_epoch().count();
			auto endMicro = std::chrono::time_point_cast<std::chrono::microseconds>(endTimePoint).time_since_epoch().count();
			auto duration = endMicro - startMicro;
			double mili = duration * 0.001;
			double seconds = duration *0.000001;
			if (mili > 10.0)
			{
				Nova::Log::GetCoreLogger()->error("{0} microseconds elapsed", duration);
				Nova::Log::GetCoreLogger()->error("{0} miliseconds elapsed", mili);
				Nova::Log::GetCoreLogger()->error("{0} seconds elapsed", seconds);

			}
			else
			{
				Nova::Log::GetCoreLogger()->trace("{0} microseconds elapsed", duration);
				Nova::Log::GetCoreLogger()->trace("{0} miliseconds elapsed", mili);
				Nova::Log::GetCoreLogger()->trace("{0} seconds elapsed", seconds);

			}

		}
	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> m_startTimePoint;

	};
}
#endif