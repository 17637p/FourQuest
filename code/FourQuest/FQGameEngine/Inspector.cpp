#include "Inspector.h"

#include <imgui.h>
#include "imgui_stdlib.h"
#include "ImGuiColor.h"

#include "../FQReflect/FQReflect.h"
#include "../FQGraphics/IFQGraphics.h"
#include "../FQGameModule/Transform.h"
#include "GameProcess.h"
#include "EditorProcess.h"
#include "EditorEvent.h"
#include "CommandSystem.h"
#include "Command.h"
#include "RenderingSystem.h"

fq::game_engine::Inspector::Inspector()
	:mGameProcess(nullptr)
	, mEditorProcess(nullptr)
	, mSelectObject(nullptr)
	, mComponentTypes{}
	, mStateBehaviourTypes{}
	, mCurrentAddComponentIndex(0)
	, mCurrentAddBehaviourIndex(0)
	, mPrevColor{}
	, mSelectObjectHandler{}
	, mbIsOpen(true)
	, mViewType(ViewType::None)
	, mImguiID(0)
{}

fq::game_engine::Inspector::~Inspector()
{}

void fq::game_engine::Inspector::Initialize(GameProcess* game, EditorProcess* editor)
{
	mGameProcess = game;
	mEditorProcess = editor;

	getScriptTypes();

	// 이벤트 핸들 등록
	mSelectObjectHandler = mGameProcess->mEventManager->RegisterHandle<editor_event::SelectObject>
		([this](editor_event::SelectObject event) {
		mSelectObject = event.object;

		if (mSelectObject == nullptr)
			mViewType = ViewType::None;
		else
			mViewType = ViewType::GameObject;
			});

	mSelectAnimationController = mGameProcess->mEventManager->RegisterHandle<editor_event::SelectAnimationState>
		([this](editor_event::SelectAnimationState event) {
		mSelectController = event.controller;
		mSelectAnimationStateName = event.stateName;

		if (mSelectController == nullptr)
			mViewType = ViewType::None;
		else
			mViewType = ViewType::AnimationController;
			});
}

void fq::game_engine::Inspector::Render()
{
	mImguiID = 0;

	if (!mbIsOpen) return;

	if (ImGui::Begin("Inspector", &mbIsOpen))
	{
		if (mViewType == ViewType::GameObject)
			beginGameObject(mSelectObject);
		else if (mViewType == ViewType::AnimationController)
			beginAnimationController(mSelectController);

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
		beginClass(component.get(), true);
	}
	beginAddComponent();
}


void fq::game_engine::Inspector::beginClass(fq::reflect::IHandle* handle, bool bIsComponent)
{
	std::string name = fq::reflect::GetName(handle->GetHandle()->type());
	if (ImGui::TreeNode(name.c_str()))
	{
		if (bIsComponent)
		{
			beginPopupContextItem_Component(handle);
		}

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
	else if (metaType == entt::resolve<DirectX::SimpleMath::Vector2>())
	{
		beginInputFloat2_Vector2(data, handle);
	}
	else if (metaType == entt::resolve<DirectX::SimpleMath::Vector3>())
	{
		beginInputFloat3_Vector3(data, handle);
	}
	else if (metaType == entt::resolve<DirectX::SimpleMath::Vector4>())
	{
		beginInputFloat4_Vector4(data, handle);
	}
	else if (metaType == entt::resolve<DirectX::SimpleMath::Quaternion>())
	{
		beginInputFloat3_Quaternion(data, handle);
	}
	else if (metaType == entt::resolve<DirectX::SimpleMath::Color>())
	{
		beginColorEdit4_Color(data, handle);
	}
	else if (metaType == entt::resolve<fq::game_module::PrefabResource>())
	{
		beginInputText_PrefabResource(data, handle);
	}
	else if (metaType == entt::resolve<int>())
	{
		int val = data.get(handle->GetHandle()).cast<int>();
		std::string memberName = fq::reflect::GetName(data);

		ImGui::InputInt(memberName.c_str(), &val);

		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			mEditorProcess->mCommandSystem->Push<SetMetaData>(
				data, mSelectObject, handle, val);
		}

		beginIsItemHovered_Comment(data);
	}
	else if (metaType == entt::resolve<unsigned char>())
	{
		unsigned char val = data.get(handle->GetHandle()).cast<unsigned char>();

		std::string memberName = fq::reflect::GetName(data);

		ImGui::InputScalar(memberName.c_str(), ImGuiDataType_U8, &val);

		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			mEditorProcess->mCommandSystem->Push<SetMetaData>(
				data, mSelectObject, handle, val);
		}

		beginIsItemHovered_Comment(data);
	}

	else if (metaType == entt::resolve<unsigned int>())
	{
		unsigned int val = data.get(handle->GetHandle()).cast<unsigned int>();
		std::string memberName = fq::reflect::GetName(data);

		ImGui::InputScalar(memberName.c_str(), ImGuiDataType_U32, &val);

		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			mEditorProcess->mCommandSystem->Push<SetMetaData>(
				data, mSelectObject, handle, val);
		}

		beginIsItemHovered_Comment(data);
	}
	else if (metaType == entt::resolve<float>())
	{
		float val = data.get(handle->GetHandle()).cast<float>();
		std::string memberName = fq::reflect::GetName(data);

		ImGui::InputFloat(memberName.c_str(), &val);

		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			mEditorProcess->mCommandSystem->Push<SetMetaData>(
				data, mSelectObject, handle, val);
		}
		beginIsItemHovered_Comment(data);
	}
	else if (metaType == entt::resolve<double>())
	{
		double val = data.get(handle->GetHandle()).cast<double>();
		std::string memberName = fq::reflect::GetName(data);

		ImGui::InputDouble(memberName.c_str(), &val);

		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			mEditorProcess->mCommandSystem->Push<SetMetaData>(
				data, mSelectObject, handle, val);
		}

		beginIsItemHovered_Comment(data);
	}
	else if (metaType.is_sequence_container())
	{
		beginSequenceContainer(data, handle);
	}
	else if (metaType == entt::resolve<bool>())
	{
		bool val = data.get(handle->GetHandle()).cast<bool>();
		std::string memberName = fq::reflect::GetName(data);

		if (ImGui::Checkbox(memberName.c_str(), &val))
		{
			mEditorProcess->mCommandSystem->Push<SetMetaData>(
				data, mSelectObject, handle, val);
		}
		beginIsItemHovered_Comment(data);
	}
	else if (metaType.prop(fq::reflect::prop::POD))
	{
		entt::meta_any val = data.get(handle->GetHandle());
		bool isChangedData = beginPOD(val);

		if (isChangedData)
		{
			mEditorProcess->mCommandSystem->Push<SetMetaData>(
				data, mSelectObject, handle, val);
		}
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
	beginIsItemHovered_Comment(data);
}

void fq::game_engine::Inspector::beginInputText_String(entt::meta_data data, fq::reflect::IHandle* handle)
{
	std::string name = data.get(handle->GetHandle()).cast<std::string>();
	std::string memberName = fq::reflect::GetName(data);

	ImGui::InputText(memberName.c_str(), &name);

	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		if (!data.prop(fq::reflect::prop::DragDrop))
		{
			mEditorProcess->mCommandSystem->Push<SetMetaData>(
				data, mSelectObject, handle, name);
		}
		else if (mEditorProcess->mSettingWindow->CanEditPath())
		{
			mEditorProcess->mCommandSystem->Push<SetMetaData>(
				data, mSelectObject, handle, name);
		}
	}

	// DragDrop 받기
	if (data.prop(fq::reflect::prop::DragDrop) && ImGui::BeginDragDropTarget())
	{
		const ImGuiPayload* pathPayLoad = ImGui::AcceptDragDropPayload("Path");

		if (pathPayLoad)
		{
			std::filesystem::path* dropPath
				= static_cast<std::filesystem::path*>(pathPayLoad->Data);

			auto extensions = fq::reflect::GetDragDropExtension(data);

			for (auto& extension : extensions)
			{
				// 폴더 경로를 받는 경우 입력 
				if (std::filesystem::is_directory(*dropPath) && extension == ".dir")
				{
					name = dropPath->string();
					mEditorProcess->mCommandSystem->Push<SetMetaData>(
						data, mSelectObject, handle, name);
				}
				// 확장자명이 같은 경우 입력 
				else if (dropPath->extension() == extension)
				{
					name = dropPath->string();
					mEditorProcess->mCommandSystem->Push<SetMetaData>(
						data, mSelectObject, handle, name);
				}
			}
		}
	}

	beginIsItemHovered_Comment(data);
}

void fq::game_engine::Inspector::beginInputFloat3_Vector3(entt::meta_data data, fq::reflect::IHandle* handle)
{
	DirectX::SimpleMath::Vector3 v = data.get(handle->GetHandle()).cast<DirectX::SimpleMath::Vector3>();
	std::string memberName = fq::reflect::GetName(data);

	float f[3]{ v.x,v.y,v.z };

	if (ImGui::DragFloat3(memberName.c_str(), f))
	{
		data.set(handle->GetHandle(), DirectX::SimpleMath::Vector3(f[0], f[1], f[2]));
	}

	if (ImGui::IsItemActivated())
	{
		mPrevVector3 = DirectX::SimpleMath::Vector3(f[0], f[1], f[2]);
	}

	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		data.set(handle->GetHandle(), mPrevVector3);
		mEditorProcess->mCommandSystem->Push<SetMetaData>(
			data, mSelectObject, handle, DirectX::SimpleMath::Vector3(f[0], f[1], f[2]));
	}

	beginIsItemHovered_Comment(data);
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
	beginIsItemHovered_Comment(data);

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

void fq::game_engine::Inspector::beginInputFloat3_Quaternion(entt::meta_data data, fq::reflect::IHandle* handle)
{
	using namespace DirectX::SimpleMath;

	Quaternion quatarnion = data.get(handle->GetHandle()).cast<DirectX::SimpleMath::Quaternion>();
	std::string memberName = fq::reflect::GetName(data);

	Vector3 euler = quatarnion.ToEuler();

	float f[3]{ DirectX::XMConvertToDegrees(euler.x)
		,DirectX::XMConvertToDegrees(euler.y)
		,DirectX::XMConvertToDegrees(euler.z) };

	ImGui::InputFloat3(memberName.c_str(), f);

	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		euler.x = DirectX::XMConvertToRadians(f[0]);
		euler.y = DirectX::XMConvertToRadians(f[1]);
		euler.z = DirectX::XMConvertToRadians(f[2]);
		quatarnion = Quaternion::CreateFromYawPitchRoll(euler);

		mEditorProcess->mCommandSystem->Push<SetMetaData>(
			data, mSelectObject, handle, quatarnion);
	}

	beginIsItemHovered_Comment(data);
}



void fq::game_engine::Inspector::getScriptTypes()
{
	for (const auto& [id, type] : entt::resolve())
	{
		for (auto [baseID, baseType] : type.base())
		{
			if (baseType == entt::resolve<fq::game_module::Component>())
			{
				std::string label = "Scripts"; // Default
				if (type.prop(fq::reflect::prop::Label))
				{
					label = type.prop(fq::reflect::prop::Label).value().cast<const char*>();
				}

				mComponentTypes.insert({ label, type });
			}

			if (baseType == entt::resolve<fq::game_module::IStateBehaviour>())
			{
				mStateBehaviourTypes.push_back(type);
			}
		}
	}
}

void fq::game_engine::Inspector::beginAddComponent()
{
	auto addComponentiter = mComponentTypes.end();

	// 선택 버튼
	if (ImGui::BeginCombo("##add_comnpoent_combo", "AddComponent"))
	{
		std::set<std::string> checkKey{};

		for (auto& [key, value] : mComponentTypes)
		{
			checkKey.insert(key);
		}

		for (auto& key : checkKey)
		{
			if (ImGui::CollapsingHeader(key.c_str()))
			{
				for (auto [beginIter, endIter] = mComponentTypes.equal_range(key);
					beginIter != endIter; ++beginIter)
				{
					auto typeName = fq::reflect::GetName(beginIter->second);
					typeName += "##Component";

					if (ImGui::Selectable(typeName.c_str()))
					{
						addComponentiter = beginIter;
					}
				}
			}
		}

		ImGui::EndCombo();
	}

	ImGui::SameLine();

	if (mComponentTypes.end() == addComponentiter)
		return;

	// 추가 버튼
	auto& type = addComponentiter->second;
	auto id = type.id();

	// 이미 가지고있는지 확인
	auto check = mSelectObject->GetComponent(id);
	if (check != nullptr)
	{
		return;
	}

	auto anyComponent = type.construct();

	fq::game_module::Component* component =
		anyComponent.try_cast<fq::game_module::Component>();

	auto clone = std::shared_ptr<fq::game_module::Component>(component->Clone(nullptr));

	auto remove = [selectObject = mSelectObject, id]()
		{
			selectObject->RemoveComponent(id);
		};

	auto add = [selectObject = mSelectObject, id, clone]
		{
			selectObject->AddComponent(id, clone);
		};

	mEditorProcess->mCommandSystem->Push<BindFunctionCommand>(
		BindFunctionCommand{
			add, remove
		});
}

void fq::game_engine::Inspector::beginInputFloat2_Vector2(entt::meta_data data, fq::reflect::IHandle* handle)
{
	DirectX::SimpleMath::Vector2 v = data.get(handle->GetHandle()).cast<DirectX::SimpleMath::Vector2>();
	std::string memberName = fq::reflect::GetName(data);

	float f[2]{ v.x,v.y };

	ImGui::InputFloat2(memberName.c_str(), f);
	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		mEditorProcess->mCommandSystem->Push<SetMetaData>(
			data, mSelectObject, handle, DirectX::SimpleMath::Vector2(f[0], f[1]));
	}

	beginIsItemHovered_Comment(data);
}

void fq::game_engine::Inspector::beginInputFloat4_Vector4(entt::meta_data data, fq::reflect::IHandle* handle)
{
	DirectX::SimpleMath::Vector4 v = data.get(handle->GetHandle()).cast<DirectX::SimpleMath::Vector4>();
	std::string memberName = fq::reflect::GetName(data);

	float f[4]{ v.x,v.y,v.z,v.w };

	ImGui::InputFloat4(memberName.c_str(), f);

	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		mEditorProcess->mCommandSystem->Push<SetMetaData>(
			data, mSelectObject, handle, DirectX::SimpleMath::Vector4(f[0], f[1], f[2], f[3]));
	}
	beginIsItemHovered_Comment(data);
}

void fq::game_engine::Inspector::beginIsItemHovered_Comment(entt::meta_data data)
{
	auto prop = data.prop(fq::reflect::prop::Comment);

	if (!prop)
	{
		return;
	}

	if (prop.value().type() == entt::resolve<const char*>())
	{
		const char* comment = prop.value().cast<const char*>();

		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip(comment);
		}
	}
	else  if (prop.value().type() == entt::resolve<const char8_t*>())
	{
		const char8_t* comment = prop.value().cast<const char8_t*>();

		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip(reinterpret_cast<const char*>(comment));
		}
	}

}

void fq::game_engine::Inspector::beginSequenceContainer(entt::meta_data data, fq::reflect::IHandle* handle)
{
	// 컨테이너의 복사복은 가져옵니다.
	auto any = data.get(handle->GetHandle());
	auto view = any.as_sequence_container();
	assert(view);

	ImGui::Text(fq::reflect::GetName(data).c_str());
	beginIsItemHovered_Comment(data);

	bool bIsReadOnly = fq::reflect::IsReadOnly(data);

	if (!bIsReadOnly)
	{
		ImGui::SameLine();
		// Add element
		std::string addText = "Add##AddButton" + std::to_string(mImguiID++);
		if (ImGui::Button(addText.c_str()))
		{
			auto baseValue = view.value_type().construct();
			auto last = view.end();
			view.insert(last, baseValue);
			mEditorProcess->mCommandSystem->Push<SetMetaData>(
				data, mSelectObject, handle, any);
		}

		ImGui::SameLine();

		// Delete element
		std::string deleteText = "Delete##DelteButton" + std::to_string(mImguiID++);
		if (ImGui::Button(deleteText.c_str()))
		{
			auto last = view.begin();

			if (last != view.end())
			{
				view.erase(last);
				mEditorProcess->mCommandSystem->Push<SetMetaData>(
					data, mSelectObject, handle, any);
			}
		}
	}
	auto valueType = view.value_type();

	// POD Data 
	if (valueType.prop(fq::reflect::prop::POD))
	{
		unsigned int index = 0;
		for (auto element : view)
		{
			entt::meta_any val = element.as_ref();
			bool isChanged = beginPOD(val, index++);

			if (isChanged)
			{
				mEditorProcess->mCommandSystem->Push<SetMetaData>(
					data, mSelectObject, handle, any);
			}
		}
	}
	// string 특수화 
	else if (valueType == entt::resolve<std::string>())
	{
		size_t index = 0;
		for (auto element : view)
		{
			std::string val = element.cast<std::string>();
			std::string sIndex = "  [" + std::to_string(index) + "]";

			ImGui::Text(sIndex.c_str());
			ImGui::SameLine();
			if (bIsReadOnly)
			{
				ImGui::Text(val.c_str());
			}
			else // not read_only
			{
				std::string textName = "##" + sIndex + val;
				ImGui::InputText(textName.c_str(), &val);

				if (ImGui::IsItemDeactivatedAfterEdit())
				{
					view[index].cast<std::string&>() = val;
					mEditorProcess->mCommandSystem->Push<SetMetaData>(
						data, mSelectObject, handle, any);
				}

				// DragDrop 받기
				if (data.prop(fq::reflect::prop::DragDrop) && ImGui::BeginDragDropTarget())
				{
					const ImGuiPayload* pathPayLoad = ImGui::AcceptDragDropPayload("Path");

					if (pathPayLoad)
					{
						std::filesystem::path* dropPath
							= static_cast<std::filesystem::path*>(pathPayLoad->Data);

						auto extensions = fq::reflect::GetDragDropExtension(data);

						for (auto& extension : extensions)
						{
							if (dropPath->extension() == extension)
							{
								view[index].cast<std::string&>() = dropPath->string();
								mEditorProcess->mCommandSystem->Push<SetMetaData>(
									data, mSelectObject, handle, any);
							}
						}
					}
				}
			}
			++index;
		}
	}
}

void fq::game_engine::Inspector::beginPopupContextItem_Component(fq::reflect::IHandle* handle)
{
	auto type = handle->GetHandle()->type();
	auto id = type.id();

	if (ImGui::BeginPopupContextItem())
	{
		if (ImGui::MenuItem("Remove Component")
			&& type != entt::resolve<fq::game_module::Transform>())
		{

			assert(mSelectObject->GetComponent(id));

			auto component = mSelectObject->GetComponent(id)->shared_from_this();


			auto remove = [selectObject = mSelectObject, id]()
				{
					selectObject->RemoveComponent(id);
				};

			auto add = [selectObject = mSelectObject, id, component]
				{
					selectObject->AddComponent(id, component);
				};

			mEditorProcess->mCommandSystem->Push<BindFunctionCommand>(
				BindFunctionCommand{
					remove, add
				});
		}

		if (type == entt::resolve<fq::game_module::Transform>() && ImGui::MenuItem("Reset"))
		{
			auto transform = mSelectObject->GetComponent<fq::game_module::Transform>();
			auto prevMatrix = transform->GetLocalMatrix();

			auto excute = [transform, object = mSelectObject]()
				{
					transform->SetLocalMatrix(DirectX::SimpleMath::Matrix::Identity);
				};
			auto undo = [transform, object = mSelectObject, prevMatrix]()
				{
					transform->SetLocalMatrix(prevMatrix);
				};

			mEditorProcess->mCommandSystem->Push<BindFunctionCommand>(
				BindFunctionCommand{ excute, undo }
			);
		}

		ImGui::EndPopup();
	}
}

void fq::game_engine::Inspector::beginInputText_PrefabResource(entt::meta_data data, fq::reflect::IHandle* handle)
{
	auto prefabRes = data.get(handle->GetHandle()).cast<fq::game_module::PrefabResource>();

	auto name = fq::reflect::GetName(data);
	std::string prefabPath = prefabRes.GetPrefabPath();

	ImGui::InputText(name.c_str(), &prefabPath);

	// DragDrop 받기
	if (ImGui::BeginDragDropTarget())
	{
		const ImGuiPayload* pathPayLoad = ImGui::AcceptDragDropPayload("Path");

		if (pathPayLoad)
		{
			std::filesystem::path* dropPath
				= static_cast<std::filesystem::path*>(pathPayLoad->Data);

			if (dropPath->extension() == ".prefab")
			{
				prefabPath = dropPath->string();
				prefabRes.SetPrefabPath(prefabPath);

				data.set(handle->GetHandle(), prefabRes);
			}
		}
	}

	beginIsItemHovered_Comment(data);
}

void fq::game_engine::Inspector::beginAnimationController(const std::shared_ptr<fq::game_module::AnimatorController>& controller)
{
	auto& stateMap = controller->GetStateMap();
	auto iter = stateMap.find(mSelectAnimationStateName);

	// 삭제 예외처리
	if (iter == stateMap.end())
	{
		mSelectAnimationStateName = {};
		mSelectController = nullptr;
		mViewType = ViewType::None;
		return;
	}

	// State GUI를 표시합니다 
	{
		auto& state = iter->second;

		std::string StateName = state.GetAnimationKey();

		// Animation State 표시
		ImGui::InputText("StateName##StateName", &StateName);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			if (controller->ChangeStateName(state.GetAnimationKey(), StateName))
			{
				mSelectAnimationStateName = StateName;
				iter = stateMap.find(mSelectAnimationStateName);
			}
		}
	}

	auto& state = iter->second;

	// Animation 정보 표시
	beginAnimationStateNode(state);

	ImGui::Dummy(ImVec2{ 0.f,10.f });
	ImGui::Text("Transition");

	// Transition GUI를 표시합니다 
	int conditionIndex = 0;
	int transitionIndex = 0;
	auto& transitions = controller->GetTransitionMap();
	auto range = transitions.equal_range(state.GetAnimationKey());
	auto eraseTransition = transitions.end();

	for (auto& iter = range.first; iter != range.second; ++iter)
	{
		auto& transition = iter->second;
		++conditionIndex;
		++transitionIndex;
		auto& conditions = transition.GetConditions();
		auto enterState = transition.GetEnterState();
		auto exitState = transition.GetExitState();

		std::string transitionName = "[" + std::to_string(transitionIndex) + "]" + exitState + " -> " + enterState;
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4{ 0.44f, 0.37f, 0.61f, 1.0f });

		if (ImGui::CollapsingHeader(transitionName.c_str()))
		{
			if (ImGui::BeginPopupContextItem())
			{
				if (ImGui::MenuItem("Remove Transition"))
				{
					eraseTransition = iter;
				}
				ImGui::EndPopup();
			}

			if (ImGui::BeginChild(transitionName.c_str(), ImVec2(0, 0), ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeY))
			{
				bool hasExitTime = transition.HasExitTime();

				if (ImGui::Checkbox("HasExitTime", &hasExitTime))
				{
					if (hasExitTime)
						transition.SetExitTime(0.f);
					else
						transition.SetExitTime(game_module::AnimationTransition::NoExitTime);

					hasExitTime = transition.HasExitTime();
				}

				if (hasExitTime)
				{
					float exitTime = transition.GetExitTime();
					if (ImGui::InputFloat("ExitTime", &exitTime))
					{
						transition.SetExitTime(exitTime);
					}
				}

				float transitionDuration = transition.GetTransitionDuration();
				if (ImGui::InputFloat("TransitionDuration", &transitionDuration))
				{
					transition.SetTransitionDuration(transitionDuration);
				}

				int source = static_cast<int>(transition.GetInterruptionSource());

				constexpr const char* interruptionSourece[] = { "None", "CurrentState", "NextState"
				, "CurrentState Then NextState", "NextState Then CurrentState" };

				if (ImGui::BeginCombo("InterruptionSource", interruptionSourece[source]))
				{
					for (int i = 0; i < 5; ++i)
					{
						if (ImGui::Selectable(interruptionSourece[i]))
						{
							transition.SetInterruptionSource(static_cast<game_module::AnimationTransition::InterruptionSource>(i));
						}
					}
					ImGui::EndCombo();
				}

				// AnyState 특별 처리 
				if (transition.GetExitState() == "AnyState")
				{
					bool canTransitionToSelf = transition.CanTrasitionToSelf();
					if (ImGui::Checkbox("CanTranstionToSelf", &canTransitionToSelf))
					{
						transition.SetCanTrasitionToSelf(canTransitionToSelf);
					}
				}

				ImGui::Separator();
				ImGui::Text("Conditions");

				// Condtion GUI
				for (auto& condition : conditions)
				{
					beginTransitionCondition(condition, ++conditionIndex);
				}

				// Add Delete Button
				std::string addButtonNaem = "+##PushBackCondition" + transitionName;
				std::string deleteButtonName = "-##PopBackCondition" + transitionName;

				if (ImGui::Button(addButtonNaem.c_str()))
					transition.PushBackCondition(game_module::TransitionCondition::CheckType::Equals, "", 0);
				ImGui::SameLine();
				if (ImGui::Button(deleteButtonName.c_str()))
					transition.PopBackCondition();
			}
			ImGui::EndChild();


		}
		ImGui::PopStyleColor(1);
	}

	// 트랜지션 삭제처리 
	if (eraseTransition != transitions.end())
	{
		transitions.erase(eraseTransition);
	}

	beginStateBehaviour(state);
}


void fq::game_engine::Inspector::beginTransitionCondition(fq::game_module::TransitionCondition& condition, int index)
{
	auto parameterPack = mSelectController->GetParameterPack();

	// 파라미터 선택 콤보창
	ImGui::SetNextItemWidth(150.f);
	std::string parameterComboName = "##Parameters" + std::to_string(index);
	if (ImGui::BeginCombo(parameterComboName.c_str(), condition.GetParameterID().c_str()))
	{
		for (auto& [id, parameter] : parameterPack)
		{
			std::string idName = id + "##" + std::to_string(index);
			// 파라미터 선택지
			if (ImGui::Selectable(idName.c_str()))
			{
				condition.SetParameterID(id);
				condition.SetCompareParameter(parameter);
			}
		}
		ImGui::EndCombo();
	}

	// 비교 연산자 콤보창
	auto compareParam = condition.GetCompareParameter();

	bool isInt = compareParam.type() == entt::resolve<int>();
	bool isFloat = compareParam.type() == entt::resolve<float>();
	bool isBool = compareParam.type() == entt::resolve<bool>();
	bool isChar = compareParam.type() == entt::resolve<char>();

	if (!isInt && !isFloat && !isBool) return;
	ImGui::SameLine();

	constexpr const char* type[4] = { "Greater","Less" ,"Equals", "NotEqual" };
	int checkType = static_cast<int>(condition.GetCheckType());

	ImGui::SetNextItemWidth(100.f);
	std::string OperatorComboName = "##Operator" + std::to_string(index);

	if (ImGui::BeginCombo(OperatorComboName.c_str(), type[checkType]))
	{
		if (!isBool)
		{
			std::string type0 = std::string(type[0]) + "##" + std::to_string(index);
			std::string type1 = std::string(type[1]) + "##" + std::to_string(index);

			if (ImGui::Selectable(type0.c_str()))
			{
				condition.SetCheckType(game_module::TransitionCondition::CheckType::Greater);
			}
			if (ImGui::Selectable(type1.c_str()))
			{
				condition.SetCheckType(game_module::TransitionCondition::CheckType::Less);
			}
		}
		if (!isFloat)
		{
			std::string type2 = std::string(type[2]) + "##" + std::to_string(index);
			std::string type3 = std::string(type[3]) + "##" + std::to_string(index);

			if (ImGui::Selectable(type2.c_str()))
			{
				condition.SetCheckType(game_module::TransitionCondition::CheckType::Equals);
			}
			if (ImGui::Selectable(type3.c_str()))
			{
				condition.SetCheckType(game_module::TransitionCondition::CheckType::NotEqual);
			}
		}

		ImGui::EndCombo();
	}

	ImGui::SameLine();
	std::string compareParameterName = "##compareParameter" + std::to_string(index);
	// 비교 파라미터 입력창
	if (isBool)
	{
		bool val = compareParam.cast<bool>();

		if (ImGui::Checkbox(compareParameterName.c_str(), &val))
		{
			condition.SetCompareParameter(val);
		}
	}
	else if (isInt)
	{
		int val = compareParam.cast<int>();
		ImGui::SetNextItemWidth(100.f);
		if (ImGui::DragInt(compareParameterName.c_str(), &val))
		{
			condition.SetCompareParameter(val);
		}
	}
	else if (isFloat)
	{
		float val = compareParam.cast<float>();
		ImGui::SetNextItemWidth(100.f);
		if (ImGui::InputFloat(compareParameterName.c_str(), &val))
		{
			condition.SetCompareParameter(val);
		}
	}
}

void fq::game_engine::Inspector::beginAnimationStateNode(fq::game_module::AnimationStateNode& stateNode)
{
	// 도움 필요
	if (stateNode.GetType() != game_module::AnimationStateNode::Type::State)
		return;

	// ModelPath GUI
	std::string animationPath = stateNode.GetAnimationPath();

	bool isModelExist = std::filesystem::exists(animationPath);

	if (!isModelExist)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, ImGuiColor::RED);
	}

	ImGui::InputText("AnimationPath", &animationPath);

	if (!isModelExist)
	{
		ImGui::PopStyleColor(1);
	}

	// DragDrop 받기
	if (ImGui::BeginDragDropTarget())
	{
		const ImGuiPayload* pathPayLoad = ImGui::AcceptDragDropPayload("Path");

		if (pathPayLoad)
		{
			std::filesystem::path* dropPath
				= static_cast<std::filesystem::path*>(pathPayLoad->Data);

			if (dropPath->extension() == ".animation")
			{
				stateNode.SetAnimationPath(dropPath->string());
			}
		}
	}

	animationPath = stateNode.GetAnimationPath();
	auto animationInterfaceOrNull = mGameProcess->mGraphics->GetAnimationOrNull(animationPath);

	if (animationInterfaceOrNull == nullptr && std::filesystem::exists(animationPath))
	{
		const auto animationData = mGameProcess->mGraphics->ReadAnimation(animationPath);
		animationInterfaceOrNull = mGameProcess->mGraphics->CreateAnimation(animationPath, animationData);
	}

	if (animationPath.empty() || animationInterfaceOrNull == nullptr) return;

	// PlayBackSpeed
	float playBackSpeed = stateNode.GetPlayBackSpeed();

	if (ImGui::InputFloat("PlayBackSpeed", &playBackSpeed))
	{
		stateNode.SetPlayBackSpeed(playBackSpeed);
	}

	float maxDuration = animationInterfaceOrNull->GetAnimationClip().Duration;

	float startTimePos = stateNode.GetStartTimePos();
	float duration = stateNode.GetDuration();

	if (ImGui::SliderFloat("StartTimePos", &startTimePos, 0.f, duration))
	{
		stateNode.SetStartTimePos(startTimePos);
	}

	if (ImGui::SliderFloat("Duration", &duration, 0.f, maxDuration))
	{
		stateNode.SetDuration(duration);
	}
	
	bool IsLoof = stateNode.IsLoof();
	if (ImGui::Checkbox("IsLoof", &IsLoof))
	{
		stateNode.SetLoof(IsLoof);
	}
}

bool fq::game_engine::Inspector::beginPOD(entt::meta_any& pod, unsigned int index)
{
	bool changedData = false;
	auto metaType = pod.type();

	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4{ 0.44f, 0.37f, 0.61f, 1.0f });
	// POD 이름 표시 

	std::string podName = fq::reflect::GetName(metaType);

	if (index != -1)
	{
		podName += "[" + std::to_string(index) + "]" + "##" + std::to_string(mImguiID++);
	}

	if (ImGui::CollapsingHeader(podName.c_str()))
	{
		if (ImGui::BeginChild(podName.c_str(), ImVec2(0, 0), ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeY))
		{
			for (auto [id, data] : metaType.data())
			{
				std::string memberName = fq::reflect::GetName(data);

				if (data.type() == entt::resolve<int>())
				{
					int val = data.get(pod).cast<int>();
					ImGui::InputInt(memberName.c_str(), &val);

					if (ImGui::IsItemDeactivatedAfterEdit())
					{
						data.set(pod, val);
						changedData = true;
					}
					beginIsItemHovered_Comment(data);
				}
				else if (data.type() == entt::resolve<size_t>())
				{
					size_t val = data.get(pod).cast<size_t>();
					ImGui::InputScalar(memberName.c_str(), ImGuiDataType_U64, &val);

					if (ImGui::IsItemDeactivatedAfterEdit())
					{
						data.set(pod, val);
						changedData = true;
					}
					beginIsItemHovered_Comment(data);
				}
				else if (data.type() == entt::resolve<unsigned int>())
				{
					unsigned int val = data.get(pod).cast<unsigned int>();
					ImGui::InputScalar(memberName.c_str(), ImGuiDataType_U32, &val);

					if (ImGui::IsItemDeactivatedAfterEdit())
					{
						data.set(pod, val);
						changedData = true;
					}
					beginIsItemHovered_Comment(data);
				}
				else if (data.type() == entt::resolve<float>())
				{
					float val = data.get(pod).cast<float>();

					ImGui::InputFloat(memberName.c_str(), &val);

					if (ImGui::IsItemDeactivatedAfterEdit())
					{
						data.set(pod, val);
						changedData = true;
					}
					beginIsItemHovered_Comment(data);
				}
				else if (data.type() == entt::resolve<double>())
				{
					double val = data.get(pod).cast<double>();

					ImGui::InputDouble(memberName.c_str(), &val);

					if (ImGui::IsItemDeactivatedAfterEdit())
					{
						data.set(pod, val);
						changedData = true;
					}
					beginIsItemHovered_Comment(data);
				}
				else if (data.type() == entt::resolve<bool>())
				{
					bool val = data.get(pod).cast<bool>();

					if (ImGui::Checkbox(memberName.c_str(), &val))
					{
						data.set(pod, val);
						changedData = true;
					}
					beginIsItemHovered_Comment(data);
				}
				else if (data.type() == entt::resolve<DirectX::SimpleMath::Vector2>())
				{
					DirectX::SimpleMath::Vector2 v = data.get(pod).cast<DirectX::SimpleMath::Vector2>();
					std::string memberName = fq::reflect::GetName(data);

					float f[2]{ v.x,v.y };

					ImGui::InputFloat2(memberName.c_str(), f);

					if (ImGui::IsItemDeactivatedAfterEdit())
					{
						v.x = f[0];
						v.y = f[1];
						data.set(pod, v);
						changedData = true;
					}

					beginIsItemHovered_Comment(data);

				}
				else if (data.type() == entt::resolve<DirectX::SimpleMath::Vector3>())
				{
					DirectX::SimpleMath::Vector3 v = data.get(pod).cast<DirectX::SimpleMath::Vector3>();
					std::string memberName = fq::reflect::GetName(data);

					float f[3]{ v.x,v.y, v.z };

					ImGui::InputFloat3(memberName.c_str(), f);

					if (ImGui::IsItemDeactivatedAfterEdit())
					{
						v.x = f[0];
						v.y = f[1];
						v.z = f[2];
						data.set(pod, v);
						changedData = true;
					}

					beginIsItemHovered_Comment(data);

				}
				else if (data.type() == entt::resolve<DirectX::SimpleMath::Color>())
				{
					using namespace DirectX::SimpleMath;

					Color color = data.get(pod).cast<DirectX::SimpleMath::Color>();
					std::string memberName = fq::reflect::GetName(data);

					float f[4] = { color.x, color.y ,color.z, color.w };

					if (ImGui::ColorEdit4(memberName.c_str(), f))
					{
						color.x = f[0];
						color.y = f[1];
						color.z = f[2];
						color.w = f[3];

						data.set(pod, color);
						changedData = true;
					}
					beginIsItemHovered_Comment(data);
				}
				else if (data.type().is_enum())
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
					auto currentValue = data.get(pod);
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
								data.set(pod, val);
								changedData = true;
							}
						}
						ImGui::EndCombo();
					}
					beginIsItemHovered_Comment(data);
				}
				else if (data.type() == entt::resolve<std::string>())
				{
					std::string val = data.get(pod).cast<std::string>();

					ImGui::InputText(memberName.c_str(), &val);
					if (ImGui::IsItemDeactivatedAfterEdit())
					{
						if (!data.prop(fq::reflect::prop::DragDrop))
						{
							data.set(pod, val);
							changedData = true;
						}
						else if (mEditorProcess->mSettingWindow->CanEditPath())
						{
							data.set(pod, val);
							changedData = true;
						}
					}

					// DragDrop 받기
					if (data.prop(fq::reflect::prop::DragDrop) && ImGui::BeginDragDropTarget())
					{
						const ImGuiPayload* pathPayLoad = ImGui::AcceptDragDropPayload("Path");

						if (pathPayLoad)
						{
							std::filesystem::path* dropPath
								= static_cast<std::filesystem::path*>(pathPayLoad->Data);

							auto extensions = fq::reflect::GetDragDropExtension(data);

							for (auto& extension : extensions)
							{
								if (dropPath->extension() == extension)
								{
									val = dropPath->string();
									data.set(pod, val);
									changedData = true;
								}
							}
						}
					}
					beginIsItemHovered_Comment(data);
				}
				else if (data.type().prop(fq::reflect::prop::POD))
				{
					entt::meta_any val = data.get(pod);
					bool isChangedData = beginPOD(val);

					if (isChangedData)
					{
						changedData = isChangedData;
						data.set(pod, val);
					}
				}
				else if (data.type() == entt::resolve<std::wstring>())
				{
					std::wstring val = data.get(pod).cast<std::wstring>();

					std::string sVal = std::filesystem::path(val).string();

					ImGui::InputText(memberName.c_str(), &sVal);

					if (ImGui::IsItemDeactivatedAfterEdit())
					{
						if (!data.prop(fq::reflect::prop::DragDrop))
						{
							val = std::filesystem::path(sVal).wstring();
							data.set(pod, val);
							changedData = true;
						}
						else if (mEditorProcess->mSettingWindow->CanEditPath())
						{
							val = std::filesystem::path(sVal).wstring();
							data.set(pod, val);
							changedData = true;
						}
					}

					// DragDrop 받기
					if (data.prop(fq::reflect::prop::DragDrop) && ImGui::BeginDragDropTarget())
					{
						const ImGuiPayload* pathPayLoad = ImGui::AcceptDragDropPayload("Path");

						if (pathPayLoad)
						{
							std::filesystem::path* dropPath
								= static_cast<std::filesystem::path*>(pathPayLoad->Data);

							auto extensions = fq::reflect::GetDragDropExtension(data);

							for (auto& extension : extensions)
							{
								if (dropPath->extension() == extension)
								{
									val = dropPath->wstring();
									data.set(pod, val);
									changedData = true;
								}
							}
						}
					}
					beginIsItemHovered_Comment(data);
				}
			}
		}
		ImGui::EndChild();
	}
	ImGui::PopStyleColor(1);

	return changedData;
}

void fq::game_engine::Inspector::beginStateBehaviour(fq::game_module::AnimationStateNode& stateNode)
{
	ImGui::Dummy({ 0,10 });
	auto& behaviourMap = stateNode.GetStateBehaviourMap();

	entt::id_type removeBehavour = 0;

	for (auto& [id, behaviour] : behaviourMap)
	{
		std::string number = " " + std::string{ "##" } + std::to_string(id);
		ImGui::Button(number.c_str());

		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Remove Behaviour"))
			{
				removeBehavour = id;
			}
			ImGui::EndPopup();
		}

		ImGui::SameLine();
		beginClass(behaviour.get(), false);
	}

	if (removeBehavour != 0)
	{
		behaviourMap.erase(removeBehavour);
	}

	ImGui::Dummy({ 0,10 });

	// AddScript
	auto currentName = fq::reflect::GetName(mStateBehaviourTypes[mCurrentAddBehaviourIndex]);

	// 선택 버튼
	if (ImGui::BeginCombo("##add_behaviour_combo", currentName.c_str()))
	{
		for (int i = 0; i < mStateBehaviourTypes.size(); ++i)
		{
			auto componentName = fq::reflect::GetName(mStateBehaviourTypes[i]);

			bool bIsSelected = mCurrentAddBehaviourIndex == i;

			if (ImGui::Selectable(componentName.c_str(), bIsSelected))
			{
				mCurrentAddBehaviourIndex = i;
			}
		}
		ImGui::EndCombo();
	}

	ImGui::SameLine();

	// 추가 버튼
	if (ImGui::Button("+##add_behaviour_button"))
	{
		auto& type = mStateBehaviourTypes[mCurrentAddBehaviourIndex];
		auto id = type.id();

		// 이미 가지고있는지 확인
		auto check = behaviourMap.find(id);

		if (check != behaviourMap.end())
		{
			return;
		}

		auto anyComponent = type.construct();

		fq::game_module::IStateBehaviour* stateBehaviour =
			anyComponent.try_cast<fq::game_module::IStateBehaviour>();

		auto clone = std::shared_ptr<fq::game_module::IStateBehaviour>(stateBehaviour->Clone());

		behaviourMap.insert({ id, clone });
	}
}

