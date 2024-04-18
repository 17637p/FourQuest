#pragma once

namespace fq::loader
{
	enum class EFileMode
	{
		Read,
		Write
	};

	class FileUtil
	{
	public:
		FileUtil() = default;
		~FileUtil();

		bool Open(std::string filePath, EFileMode fileMode);

		template <typename T>
		inline void Write(const T& data);
		template <>
		inline void Write<std::string>(const std::string& data);
		template <>
		inline void Write<std::wstring>(const std::wstring& data);
		void Write(const void* data, unsigned int size);

		template <typename T>
		inline void Read(T* outData);
		template <>
		inline void Read<std::string>(std::string* outString);
		template <>
		inline void Read<std::wstring>(std::wstring* outString);
		void Read(void* outData, unsigned int size);

	private:
		EFileMode mFileMode;
		std::fstream mFstream;
	};

	template <typename T>
	void FileUtil::Write(const T& data)
	{
		assert(mFstream.is_open());
		mFstream.write((char*)&data, sizeof(data));
	}

	template <>
	void FileUtil::Write<std::string>(const std::string& data)
	{
		Write(data.size());

		if (data.size() == 0)
		{
			return;
		}

		Write((void*)data.data(), data.size());
	}
	template <>
	void FileUtil::Write<std::wstring>(const std::wstring& data)
	{
		Write(data.size());

		if (data.size() == 0)
		{
			return;
		}

		Write((void*)data.data(), sizeof(WCHAR) * data.size());
	}

	template <typename T>
	void FileUtil::Read(T* outData)
	{
		assert(mFstream.is_open());
		mFstream.read((char*)outData, sizeof(T));
	}

	template <>
	void FileUtil::Read<std::string>(std::string* outString)
	{
		assert(mFstream.is_open());
		size_t size;
		Read(&size);

		if (size == 0)
		{
			return;
		}

		char* str = new char[size + 1];
		str[size] = '\0';
		mFstream.read(str, size);
		*outString = str;
		delete[] str;
	}
	template <>
	void FileUtil::Read<std::wstring>(std::wstring* outString)
	{
		assert(mFstream.is_open());
		size_t size;
		Read(&size);

		if (size == 0)
		{
			return;
		}

		WCHAR* str = new WCHAR[size + 1];
		str[size] = '\0';
		mFstream.read((char*)str, sizeof(WCHAR) * size);
		*outString = str;
		delete[] str;
	}
}
