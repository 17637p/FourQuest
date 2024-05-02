#pragma once

#include "IEditorWindow.h"

#include <list>
#include <memory>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/callback_sink.h>

namespace fq::game_engine
{
	class GameProcess;

	/// <summary>
	/// 디버그 로그에 대한 정보를 표시
	/// </summary>
	class LogWindow : public IEditorWindow
	{
		struct LogMessage
		{
			std::string time;
			std::string loggerName;
			std::string payload;
			std::string level;
		};

	public:
		LogWindow();
		~LogWindow();

		void Initialize(GameProcess* game);

		void Finalize();

		void Render() override;

		void AddLog(spdlog::details::log_msg msg);

		bool& IsWindowOpen() { return mbIsOpen; }

		void SetLevel(spdlog::level::level_enum level);
	private:
		void beginButton_LogController();
		void beginChild_LogList();
		void beginText_Log(const LogMessage& msg)const;

	private:
		GameProcess* mGameProcess;

		UINT mMaxLogListSize;
		bool mbIsOpen;
		bool mbIsShowTime;
		spdlog::level::level_enum mLevel;

		std::shared_ptr<spdlog::logger> mEditorLogger;
		std::shared_ptr<spdlog::logger> mGraphicsLogger;
		std::shared_ptr<spdlog::logger> mPhysicsLogger;

		std::list<LogMessage> mLogList;
		std::string_view mLevelString[6];

	};


}