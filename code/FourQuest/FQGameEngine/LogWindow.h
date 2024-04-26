#pragma once

#include "IEditorWindow.h"

#include <list>
#include <memory>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/callback_sink.h>

namespace fq::game_engine
{
	/// <summary>
	/// 디버그 로그에 대한 정보를 표시
	/// </summary>
	class LogWindow : public IEditorWindow
	{
		struct LogMessage
		{
			std::string time;
			std::string payload;
			std::string level;
		};

	public:
		LogWindow();
		~LogWindow();

		void Initialize();

		void Finalize();

		void Render() override;

		void AddLog(spdlog::details::log_msg msg);

		bool& IsWindowOpen() { return mbIsOpen; }

	private:
		void beginButton_LogController();
		void beginChild_LogList();
		void beginText_Log(const LogMessage& msg);

	private:
		UINT mMaxLogListSize;
		bool mbIsOpen;
		bool mbIsShowTime;
		spdlog::level::level_enum mLevel;

		std::shared_ptr<spdlog::logger> mLogger;
		std::list<LogMessage> mLogList;
	};


}