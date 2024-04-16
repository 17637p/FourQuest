#include "pch.h"

#include "FileUtil.h"

namespace fq::loader
{
	FileUtil::~FileUtil()
	{
		if (mFstream.is_open())
		{
			mFstream.close();
		}
	}

	bool FileUtil::Open(std::string filePath, EFileMode fileMode)
	{
		if (mFstream.is_open())
		{
			mFstream.close();
		}

		if (fileMode == EFileMode::Read)
		{
			mFstream.open(filePath, std::ios_base::in | std::ios_base::binary);
		}
		else if (fileMode == EFileMode::Write)
		{
			mFstream.open(filePath, std::ios_base::out | std::ios_base::binary);
		}
		else
		{
			assert(false);
		}

		return mFstream.is_open();
	}

	void FileUtil::Write(const void* data, unsigned int size)
	{
		assert(mFstream.is_open());
		mFstream.write((char*)data, size);
	}

	void FileUtil::Read(void* outData, unsigned int size)
	{
		assert(mFstream.is_open());
		mFstream.read((char*)outData, size);
	}
}