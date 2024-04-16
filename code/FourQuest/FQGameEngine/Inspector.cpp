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
	, mInputManager(nullptr)
	, mComponentTypes{}
	, mCurrentAddComponentIndex(0)
	, mPrevColor{}
{}

fq::game_engine::Inspector::~Inspector()
{}

void fq::game_engine::Inspector::Initialize(GameProcess* game, EditorProcess* editor)
{
	mGameProcess = game;
	mEditorProcess = editor;
	mInputManager = editor->mInputManager.get();


	// 이벤트 핸들 등록
	mSelectObjectHandle = mGameProcess->mEventManager->RegisterHandle<editor_event::SelectObject>
		([this](editor_event::SelectObject event) {
		mSelectObject = event.object;
			});

	getComponentTypes();
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
	// GameObject 클래스 정보 표시
	beginClass(object.get());

	// 컴포넌트 정보 표시
	for (const auto& [id, component] : object->GetComponentContainer())
	{
		beginClass(component.get());
	}

	beginAddComponent();
}


void fq::game_engine::Inspector::beginClass(fq::reflect::IHandle* handle)
{
	std::string name = fq::reflect::GetName(handle->GetHandle()->type());
	if (ImGui::TreeNode(name.c_str()))
	{
		for (auto [id, metaData] : handle->GetHandle()->type().data())
		{
			beginMember(metaData, handle);
		}
		ImGui::TreePop();
	}

	ImGui::Separator();
}

void fq::game_engine::Inspector::beginMember(entt::meta_data data, fq::reflect::IHandle* handle)
{
	auto metaType = data.type();

	if (metaType.is_enum())
	{
		beginCombo_EnumClass(data, handle);
	}
	else if (metaType == entt::resolve<std::string>())
	{
		beginInputText_String(data, handle);
	}
	else if (metaType == entt::resolve<DirectX::SimpleMath::Vector3>())
	{
		beginInputFloat3_Vector3(data, handle);
	}
	else if (metaType == entt::resolve<DirectX::SimpleMath::Quaternion>())
	{
		beginInputFloat3_Quaternion(data, handle);
	}
	else if (metaType == entt::resolve<DirectX::SimpleMath::Color>())
	{
		beginColorEdit4_Color(data, handle);
	}
}

void fq::game_engine::Inspector::beginCombo_EnumClass(entt::meta_data data, fq::reflect::IHandle* handle)
{
	std::map<int, entt::meta_data> enumMembers;

	// eunmMember string table 생성
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

	// 현재 enum 값 int로 가져오기
	auto currentValue = data.get(handle->GetHandle());
	int iCurrentValue = 0;

	if (currentValue.allow_cast<int>())
	{
		iCurrentValue = currentValue.cast<int>();
	}

	std::string memberName = fq::reflect::GetName(data);
	auto iter = enumMembers.find(iCurrentValue);
	assert(iter != enumMembers.end());
	std::string currentEnumName = fq::reflect::GetName(iter->second);

	// Combo 창
	if (ImGui::BeginCombo(memberName.c_str(), currentEnumName.c_str()))
	{
		for (const auto& [val, metaData] : enumMembers)
		{
			std::string memberName = fq::reflect::GetName(metaData);

			const bool bIsSelected = val == iCurrentValue;

			if (ImGui::Selectable(memberName.c_str(), bIsSelected))
			{
				mEditorProcess->mCommandSystem->Push<SetMetaData>(
					data, mSelectObject, handle, val);
			}
		}
		ImGui::EndCombo();
	}
}

void fq::game_engine::Inspector::beginInputText_String(entt::meta_data data, fq::reflect::IHandle* handle)
{
	std::string name = data.get(handle->GetHandle()).cast<std::string>();
	std::string memberName = fq::reflect::GetName(data);

	if (ImGui::InputText(memberName.c_str(), &name)
		&& mInputManager->IsKeyState(Key::Enter, KeyState::Tap))
	{
		mEditorProcess->mCommandSystem->Push<SetMetaData>(
			data, mSelectObject, handle, name);
	}
}

void fq::game_engine::Inspector::beginInputFloat3_Vector3(entt::meta_data data, fq::reflect::IHandle* handle)
{
	DirectX::SimpleMath::Vector3 v = data.get(handle->GetHandle()).cast<DirectX::SimpleMath::Vector3>();
	std::string memberName = fq::reflect::GetName(data);

	float f[3]{ v.x,v.y,v.z };

	if (ImGui::InputFloat3(memberName.c_str(), f)
		&& mInputManager->IsKeyState(Key::Enter, KeyState::Tap))
	{
		mEditorProcess->mCommandSystem->Push<SetMetaData>(
			data, mSelectObject, handle, DirectX::SimpleMath::Vector3(f[0], f[1], f[2]));
	}
}

void fq::game_engine::Inspector::beginInputFloat3_Quaternion(entt::meta_data data, fq::reflect::IHandle* handle)
{
	using namespace DirectX::SimpleMath;

	Quaternion quatarnion = data.get(handle->GetHandle()).cast<DirectX::SimpleMath::Quaternion>();
	std::string memberName = fq::reflect::GetName(data);

	Vector3 euler = quatarnion.ToEuler();

	float f[3]{ euler.x,euler.y,euler.z };

	if (ImGui::InputFloat3(memberName.c_str(), f)
		&& mInputManager->IsKeyState(Key::Enter, KeyState::Tap))
	{
		euler.x = f[0];
		euler.y = f[1];
		euler.z = f[2];
		quatarnion = Quaternion::CreateFromYawPitchRoll(euler);

		mEditorProcess->mCommandSystem->Push<SetMetaData>(
			data, mSelectObject, handle, quatarnion);
	}
}

void fq::game_engine::Inspector::beginColorEdit4_Color(entt::meta_data data, fq::reflect::IHandle* handle)
{
	using namespace DirectX::SimpleMath;

	Color color = data.get(handle->GetHandle()).cast<DirectX::SimpleMath::Color>();
	std::string memberName = fq::reflect::GetName(data);

	float f[4] = { color.x, color.y ,color.z, color.w };

	if (ImGui::ColorEdit4(memberName.c_str(), f))
	{
		color.x = f[0];
		color.y = f[1];
		color.z = f[2];
		color.w = f[3];

		data.set(handle->GetHandle(), color);
	}

	if (ImGui::IsItemActivated())
	{
		mPrevColor = color;
	}

	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		data.set(handle->GetHandle(), mPrevColor);
		mEditorProcess->mCommandSystem->Push<SetMetaData>(
			data, mSelectObject, handle, color);
	}
}


void fq::game_engine::Inspector::getComponentTypes()
{
	for (const auto& [id, type] : entt::resolve())
	{
		for (auto [baseID, baseType] : type.base())
		{
			if (baseType == entt::resolve<fq::game_module::Component>())
			{
				mComponentTypes.push_back(type);
			}
		}
	}
}

void fq::game_engine::Inspector::beginAddComponent()
{
	auto currentName = fq::reflect::GetName(mComponentTypes[mCurrentAddComponentIndex]);

	// 선택 버튼
	if (ImGui::BeginCombo("##add_comnpoent_combo", currentName.c_str()))
	{
		for (int i = 0; i < mComponentTypes.size(); ++i)
		{
			auto componentName = fq::reflect::GetName(mComponentTypes[i]);

			bool bIsSelected = mCurrentAddComponentIndex == i;

			if (ImGui::Selectable(componentName.c_str(), bIsSelected))
			{
				mCurrentAddComponentIndex = i;
			}
		}
		ImGui::EndCombo();
	}

	// 추가 버튼
	if (ImGui::Button("+##add_component_button"))
	{
		auto& type = mComponentTypes[mCurrentAddComponentIndex];

		auto component = type.construct();

		mSelectObject->AddComponent(component);
	}
}

