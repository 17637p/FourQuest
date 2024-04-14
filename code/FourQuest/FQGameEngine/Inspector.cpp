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
{

}

void fq::game_engine::Inspector::Initialize(GameProcess* game, EditorProcess* editor)
{
	mGameProcess = game;
	mEditorProcess = editor;

	// �̺�Ʈ �ڵ� ���
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
	const auto& components = object->GetComponentContainer();
	// ���� ������Ʈ �̸� ǥ��
	std::string objectName = object->GetName();

	if (ImGui::InputText("##GameObjectName", &objectName)
		&& mEditorProcess->mInputManager->IsKeyState(Key::Enter, KeyState::Tap))
	{
		mEditorProcess->mCommandSystem->Push<SetValueCommand<std::string>>(
			SetValueCommand<std::string>{ [object](std::string name)
			{
				object->SetName(name);
			}, object->GetName(), objectName }
		);
	}

	// Tag , Layer
	entt::meta_any tag = object->GetTag();

	beginCombo_EnumClass(entt::resolve<fq::game_module::Tag>(), tag);

	// ������Ʈ ���� ǥ��
	for (const auto& [id, component] : components)
	{
		beginComponent(id, component.get());
	}
}

void fq::game_engine::Inspector::beginComponent(entt::id_type id, fq::game_module::Component* component)
{
	auto metaType = entt::resolve(id);

	entt::meta_any anyComponent = metaType.from_void(component);

	beginClass(metaType, anyComponent);
}

void fq::game_engine::Inspector::beginClass(entt::meta_type type, entt::meta_any& object)
{
	for (auto [id, metaData] : type.data())
	{
		auto memberAny = metaData.get(object);
		beginMember(metaData, memberAny);
	}
}

void fq::game_engine::Inspector::beginMember(entt::meta_data data, entt::meta_any& member)
{
	auto metaType = data.type();

	if (metaType.is_enum())
	{
		beginCombo_EnumClass(metaType, member);
	}
}

void fq::game_engine::Inspector::beginCombo_EnumClass(entt::meta_type type, entt::meta_any& enumToAny)
{
	std::map<std::string, entt::meta_data> enumMembers;

	int* eunmValue = nullptr;

	if (enumToAny.allow_cast<int>())
	{
		eunmValue = enumToAny.try_cast<int>();
	}

	for (auto [id, metaData] : type.data())
	{
		std::string name = metaData.prop(fq::reflect::tag::name).value().cast<const char*>();

		enumMembers.insert({ name,metaData });
	}

	const char* enumClassName = type.prop(fq::reflect::tag::name).value().cast<const char*>();

	if (ImGui::BeginCombo(enumClassName, enumClassName))
	{
		for (const auto& [name, metaData] : enumMembers)
		{


		}
		ImGui::EndCombo();
	}

}
