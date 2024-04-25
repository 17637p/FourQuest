#include "LogWindow.h"

#include <imgui.h>
#include <ctime> 
#include <string>
#include <iomanip>

#include "ImGuiColor.h"

fq::game_engine::LogWindow::LogWindow()
	:mLogger{}
	, mLogList{}
	, mMaxLogListSize(10)
	, mbIsShowTime(true)
	, mLevel(spdlog::level::level_enum::trace)
{}

fq::game_engine::LogWindow::~LogWindow()
{

}

void fq::game_engine::LogWindow::Render()
{
	SPDLOG_TRACE("DD");

	if (ImGui::Begin("Log"))
	{
		beginButton_LogController();
		beginChild_LogList();
	}
	ImGui::End();
}

void fq::game_engine::LogWindow::Initialize()
{
	auto callback_sink = std::make_shared<spdlog::sinks::callback_sink_mt>([this](const spdlog::details::log_msg& msg) {
		this->AddLog(msg);
		});
	auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("fqlog/editor");
	spdlog::logger logger("custom_callback_logger", { fileSink, callback_sink });

	logger.set_level(mLevel);
	spdlog::set_default_logger(logger.clone("default"));
	mLogger = spdlog::get("default");
}

void fq::game_engine::LogWindow::Finalize()
{
}

void fq::game_engine::LogWindow::beginText_Log(const spdlog::details::log_msg& msg)
{
	// Time 
	if (mbIsShowTime)
	{
		std::time_t time = std::chrono::system_clock::to_time_t(msg.time);
		std::tm now_tm;
		localtime_s(&now_tm, &time);
		char buffer[80];
		std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &now_tm);
		ImGui::Text(buffer);
		ImGui::SameLine();
	}

	// level
	std::string level{};
	if (spdlog::level::trace == msg.level)
	{
		ImGui::PushStyleColor(0, ImGuiColor::STEEL_GRAY);
		level = "[trace]";
	}
	else if (spdlog::level::debug == msg.level)
	{
		ImGui::PushStyleColor(0, ImGuiColor::TURQUOISE);
		level = "[dubug]";
	}
	else if (spdlog::level::info == msg.level)
	{
		ImGui::PushStyleColor(0, ImGuiColor::SPRING_GREEN);
		level = "[info]";
	}
	else if (spdlog::level::warn == msg.level)
	{
		ImGui::PushStyleColor(0, ImGuiColor::ORANGE);
		level = "[warn]";
	}
	else if (spdlog::level::err == msg.level)
	{
		ImGui::PushStyleColor(0, ImGuiColor::YELLOW);
		level = "[error]";
	}
	else if (spdlog::level::critical == msg.level)
	{
		ImGui::PushStyleColor(0, ImGuiColor::RED);
		level = "[critical]";
	}

	ImGui::Text(level.c_str());
	ImGui::PopStyleColor();

	// payload
	ImGui::SameLine();
	ImGui::Text(msg.payload.data());
}

void fq::game_engine::LogWindow::AddLog(spdlog::details::log_msg msg)
{
	if (mMaxLogListSize < mLogList.size())
	{
		mLogList.pop_back();
	}
	mLogList.push_front(msg);
}

void fq::game_engine::LogWindow::beginChild_LogList()
{
	if (ImGui::BeginChild("LogChild", ImVec2(-FLT_MIN, 0.0f)
		, ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeY))
	{
		for (const auto& log : mLogList)
		{
			beginText_Log(log);
			ImGui::Separator();
		}
	}
	ImGui::EndChild();
}

void fq::game_engine::LogWindow::beginButton_LogController()
{
	if (ImGui::Button("Time"))
	{
		mbIsShowTime = !mbIsShowTime;
	}

}

