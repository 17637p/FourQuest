#pragma once

#include <string>
#include <stringapiset.h>

namespace fq::common
{
	class StringUtil
	{
	public:
		static std::wstring ToWide(const std::string& str)
		{
			int num_chars = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.length(), NULL, 0);
			std::wstring wstrTo;
			if (num_chars)
			{
				wstrTo.resize(num_chars);
				MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.length(), &wstrTo[0], num_chars);
			}
			return wstrTo;
		}

		static std::string ToMultiByte(const std::wstring& wstr)
		{
			int num_chars = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.length(), NULL, 0, NULL, NULL);
			std::string strTo;
			if (num_chars > 0)
			{
				strTo.resize(num_chars);
				WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.length(), &strTo[0], num_chars, NULL, NULL);
			}
			return strTo;
		}
	};
}
