#include "SkyBox.h"

#include <fstream>
#include <iostream>

#include "../FQReflect/FQReflect.h"

using json = nlohmann::json;

void fq::game_module::SkyBox::Save(const Path& directory)
{
	auto filePath = directory / "skybox.txt";

	json skybox_json;

	skybox_json["skyBox"] = mSkyBox.wstring();
	skybox_json["diffuse"] = mDiffuse.wstring();
	skybox_json["specular"] = mSpecular.wstring();
	skybox_json["brdfLUT"] = mBrdfLUT.wstring();

	std::ofstream output(filePath);

	assert(output.is_open());

	if (output.is_open())
	{
		output << std::setw(4) << skybox_json;
		output.close();
	}
	else
	{
		assert(nullptr);
	}
}



void fq::game_module::SkyBox::Load(const Path& directory)
{
	auto filePath = directory / "skybox.txt";

	std::ifstream readData(filePath);

	json skybox_json;

	if (readData.is_open())
	{
		readData >> skybox_json;
		readData.close();

		mSkyBox = skybox_json.at("skyBox").get<std::wstring>();
		mDiffuse = skybox_json.at("diffuse").get<std::wstring>();
		mSpecular = skybox_json.at("specular").get<std::wstring>();
		mBrdfLUT = skybox_json.at("brdfLUT").get<std::wstring>();
	}
}

bool fq::game_module::SkyBox::HasSkyBox() const
{
	return std::filesystem::exists(mSkyBox);
}

bool fq::game_module::SkyBox::HasIBLTexture() const
{
	return std::filesystem::exists(mDiffuse)
		&& std::filesystem::exists(mSpecular)
		&& std::filesystem::exists(mBrdfLUT);
}
