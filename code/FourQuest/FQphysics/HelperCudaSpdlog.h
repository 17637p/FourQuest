#pragma once

#include <string>
#include <spdlog/spdlog.h>

class Cudalog
{
	template <typename ...Arg>
	static void trace(const std::string& text, Arg&&... arg)
	{
		spdlog::trace(text, std::make_shared<Arg>(arg)...);
	}

	template <typename ...Arg>
	static void warn(const std::string& text, Arg&&... arg)
	{
		spdlog::warn(text, std::make_shared<Arg>(arg)...);
	}

	template <typename ...Arg>
	static void error(const std::string& text, Arg&&... arg)
	{
		spdlog::error(text, std::make_shared<Arg>(arg)...);
	}
};