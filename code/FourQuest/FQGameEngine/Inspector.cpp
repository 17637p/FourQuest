#include "Inspector.h"

#include <imgui.h>
#include "imgui_stdlib.h"

#include "../FQReflect/FQReflect.h"
#include "GameProcess.h"
#include "EditorProcess.h"
#include "EditorEvent.h"
#include "CommandSystem.h"
#include "Command.h"

fq::game_engine::Inspector::Inspector()
	:mGameProcess(nullptr)
	, mEditorProcess(nullptr)
	, mSelectObject(nullptr)
{}

fq::game_engine::Inspector::~Inspector()
{}

void fq::game_engine::Inspector::Initialize(GameProcess* game, EditorProcess* editor)
{
	mGameProcess = game;
	mEditorProcess = editor;

	// 이벤트 핸들 등록
	mSelectObjectHandle = mGameProcess->mEventManager->RegisterHandle<editor_event::SelectObject>
		([this](editor_event::SelectObject event) {
		mSelectObject = event.object;
			});
}

void fq::game_engine::Inspector::Render()
{
	if (ImGui::Begin("Inspector"))
	{
		if (mSelectObject)
		{
			beginGameObject(mSelectObject);
		}
	}
	ImGui::End();
}

void fq::game_engine::Inspector::Finalize()
{

}

void fq::game_engine::Inspector::beginGameObject(std::shared_ptr<fq::game_module::GameObject> object)
{
	beginClass(object);

	// 컴포넌트 정보 표시
	const auto& components = object->GetComponentContainer();

	entt::meta_any d = object.get();

	for (auto [id, metaData] : entt::resolve<fq::game_module::GameObject>().data())
	{
		bool va2 = metaData.set(*object, 10);

		bool a = false;
		//beginMember(metaData, object);
	}

}

void fq::game_engine::Inspector::beginComponent(entt::id_type id, fq::game_module::Component* component)
{
	auto metaType = entt::resolve(id);

	entt::meta_any anyComponent = metaType.from_void(component);
}

void fq::game_engine::Inspector::beginClass(entt::meta_any object)
{
	for (auto [id, metaData] : entt::resolve<fq::game_module::GameObject>().data())
	{
		bool va2 = metaData.set(*object, 10);

		entt::meta_any a; 
		
		//beginMember(metaData, object);
	}
}

void fq::game_engine::Inspector::beginMember(entt::meta_data data, entt::meta_any instance)
{
	auto metaType = data.type();

	if (metaType.is_enum())
	{
		beginCombo_EnumClass(data, instance);
	}
}

void fq::game_engine::Inspector::beginCombo_EnumClass(entt::meta_data data, entt::meta_any instance)
{
	std::map<int, entt::meta_data> enumMembers;

	for (auto [id, metaData] : data.type().data())
	{
		entt::meta_any any = metaData.get({});
		if (any.allow_cast<int>())
		{
			int val = any.cast<int>();
			enumMembers.insert({ val,metaData });
		}
	}
	assert(!enumMembers.empty());

	auto currentValue = data.get(instance);

	int iCurrentValue = 0;

	if (currentValue.allow_cast<int>())
	{
		iCurrentValue = currentValue.cast<int>();
	}

	std::string memberName = fq::reflect::GetName(data);

	auto iter = enumMembers.find(iCurrentValue);
	assert(iter != enumMembers.end());
	std::string currentEnumName = fq::reflect::GetName(iter->second);

	if (ImGui::BeginCombo(memberName.c_str(), currentEnumName.c_str()))
	{
		for (const auto& [val, metaData] : enumMembers)
		{
			std::string memberName = fq::reflect::GetName(metaData);

			if (ImGui::Selectable(memberName.c_str()))
			{
				auto a = enumMembers[val].get({}).cast<fq::game_module::Tag>();
				auto s = data.set(instance, val);
			}
		}
		ImGui::EndCombo();
	}
}
