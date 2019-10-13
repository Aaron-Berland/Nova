#pragma once

#ifndef LOG_H
#define LOG_H
#include <memory.h>
#include <Nova/Core.h>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
namespace Nova {
	class NOVA_API Log
	{
	public:
		static void Init();
		inline static std::shared_ptr<spdlog::logger> & GetCoreLogger() { return s_coreLogger; }
		inline static std::shared_ptr<spdlog::logger> & GetClientLogger() { return s_clientLogger; }
	private:
		static std::shared_ptr<spdlog::logger> s_coreLogger;
		static std::shared_ptr<spdlog::logger> s_clientLogger;

	};
}
#if NOVA_DEBUG
#define NOVA_CORE_TRACE(...)              ::Nova::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define NOVA_CORE_INFO(...)               ::Nova::Log::GetCoreLogger()->info(__VA_ARGS__)
#define NOVA_CORE_WARN(...)               ::Nova::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define NOVA_CORE_ERROR(...)              ::Nova::Log::GetCoreLogger()->error(__VA_ARGS__)
#define NOVA_CORE_FATAL(...)              ::Nova::Log::GetCoreLogger()->critical(__VA_ARGS__)
#define NOVA_TRACE(...)                   ::Nova::Log::GetClientLogger()->trace(__VA_ARGS__)
#define NOVA_INFO(...)                    ::Nova::Log::GetClientLogger()->info(__VA_ARGS__)
#define NOVA_WARN(...)                    ::Nova::Log::GetClientLogger()->warn(__VA_ARGS__)
#define NOVA_ERROR(...)                   ::Nova::Log::GetClientLogger()->error(__VA_ARGS__)
#define NOVA_CRITICAL(...)                ::Nova::Log::GetClientLogger()->critical(__VA_ARGS__)
#else
#define NOVA_CORE_TRACE(...)
#define NOVA_CORE_INFO(...)
#define NOVA_CORE_WARN(...)
#define NOVA_CORE_ERROR(...) 
#define NOVA_CORE_FATAL(...)   
#define NOVA_TRACE(...)
#define NOVA_INFO(...)
#define NOVA_WARN(...)
#define NOVA_ERROR(...)
#define NOVA_CRITICAL(...)
#endif
#endif