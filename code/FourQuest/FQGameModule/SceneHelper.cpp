#include "SceneHelper.h"
#include "Scene.h"
#include "GameObject.h"

void fq::game_module::SceneHeleper::CheckNameDuplication(Scene& scene, GameObject& object)
{
	std::string name = object.GetName();

	while (HasSameName(scene,object,name))
	{
		AddIndexName(name);
	}

	object.SetName(name);
}

void fq::game_module::SceneHeleper::AddIndexName(std::string& outName)
{
	size_t open = outName.find('(');

	if (open == std::string::npos)
	{
		outName += "(1)";
		return;
	}

	size_t close = outName.find(')');

	assert(close != std::string::npos && "이상한 이름은 짓지 말아주세요");

	std::string sIndex = outName.substr(open + 1, close - open - 1);
	int index = std::stoi(sIndex);
	++index;

	std::string newName = outName.substr(0, open);

	newName += '(';
	newName += std::to_string(index);
	newName += ')';

	outName = newName;
}



bool fq::game_module::SceneHeleper::HasSameName(Scene& scene, GameObject& object, const std::string& name)
{
	for (const GameObject& viewObject : scene.GetObjectView(true))
	{
		if (viewObject.GetID() != object.GetID() 
			&& viewObject.GetName() == name)
		{
			return true;
		}
	}

	return false;
}
