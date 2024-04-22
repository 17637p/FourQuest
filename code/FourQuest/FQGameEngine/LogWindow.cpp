#include "LogWindow.h"

#include <imgui.h>

#include <string>

fq::game_engine::LogWindow::LogWindow()
	:mLogger{}
	, mLogList{}
{}

fq::game_engine::LogWindow::~LogWindow()
{

}

void fq::game_engine::LogWindow::Render()
{
	spdlog::error("dd");
	spdlog::error("dd");
	spdlog::error("dd");

	if (ImGui::Begin("Log"))
	{
		for (const auto& log : mLogList)
		{

		}

	}
	ImGui::End();
}

void fq::game_engine::LogWindow::Initialize()
{
	auto callback_sink = std::make_shared<spdlog::sinks::callback_sink_mt>([this](const spdlog::details::log_msg& msg) {
		this->mLogList.push_front(msg);
		});
	auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("fqlog/editor");
	spdlog::logger logger("custom_callback_logger", { fileSink, callback_sink });

	spdlog::set_default_logger(logger.clone("name"));
}

void fq::game_engine::LogWindow::Finalize()
{

}
