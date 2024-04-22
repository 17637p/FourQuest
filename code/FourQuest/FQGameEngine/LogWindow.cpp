#include "LogWindow.h"

#include <imgui.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

fq::game_engine::LogWindow::LogWindow()
{

}

fq::game_engine::LogWindow::~LogWindow()
{

}

void fq::game_engine::LogWindow::Render()
{
	spdlog::info("Info Le434454543vel");

	if (ImGui::Begin("Log"))
	{



	}
	ImGui::End();
}

void fq::game_engine::LogWindow::Initialize()
{
	auto logger = spdlog::basic_logger_mt("e", "4qlog/editor.txt");
	spdlog::set_default_logger(logger);
}

void fq::game_engine::LogWindow::Finalize()
{

}
