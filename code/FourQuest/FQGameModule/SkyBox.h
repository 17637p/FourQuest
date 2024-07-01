#pragma once

#include <string>
#include <filesystem>

namespace fq::game_module
{
	// SkyBox 로딩 정보 
	class SkyBox
	{
		using Path = std::filesystem::path;

	public:
		bool HasSkyBox()const;
		bool HasIBLTexture()const;

	public:
		void Save(const Path& directory);
		void Load(const Path& directory);

	public:
		Path mSkyBox;
		Path mDiffuse;
		Path mSpecular;
		Path mBrdfLUT;
	};

}
