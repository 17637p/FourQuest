#include "GameVaribleHelper.h"

#include "../FQReflect/FQReflect.h"
#include "../FQCommon/FQPath.h"

#include "IGameVariable.h"

void fq::client::GameVaribleHelper::Load()
{
	fq::reflect::Converter c;
	const auto variableDirectory = fq::path::GetInternalPath() / "variable";

	auto files = fq::path::GetFileList(variableDirectory);

	for (auto& file : files)
	{
		auto type = c.DeserializeClass(file);
	}
}

void fq::client::GameVaribleHelper::Save()
{
	fq::reflect::Converter c;
	std::vector<entt::meta_type> variableTypes;

	// 게임 변수 타입을 가져옵니다 
	for (const auto& [id, type] : entt::resolve())
	{
		for (auto [bsaeID, baseType] : type.base())
		{
			if (baseType == entt::resolve<client::IGameVariable>())
			{
				variableTypes.push_back(type);
			}
		}
	}

	const auto variableDirectory = fq::path::GetInternalPath() / "variable";

	// 게임 변수 직렬화
	for (auto& type : variableTypes)
	{
		auto instance = type.construct();
		auto type_name = fq::reflect::GetName(type);

		auto path = variableDirectory / type_name;
		path += L".json";
		c.SerializeClass(path, instance);
	}
}