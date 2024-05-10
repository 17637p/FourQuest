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
	, mSelectObjectHandler{}
	, mbIsOpen(true)
{}

fq::game_engine::Inspector::~Inspector()
{}

void fq::game_engine::Inspector::Initialize(GameProcess* game, EditorProcess* editor)
{
	mGameProcess = game;
	mEditorProcess = editor;
	mInputManager = editor->mInputManager.get();

	// 이벤트 핸들 등록
	mSelectObjectHandler = mGameProcess->mEventManager->RegisterHandle<editor_event::SelectObject>
		([this](editor_event::SelectObject event) {
		mSelectObject = event.object;
			});

	getComponentTypes();
}

void fq::game_engine::Inspector::Render()
{
	if (!mbIsOpen) return;

	if (ImGui::Begin("Inspector", &mbIsOpen))
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
		mEditorProcess->mCommandSystem->Push<SetMetaData>(
			data, mSelectObject, handle, name);
	}

	beginIsItemHovered_Comment(data);
}

void fq::game_engine::Inspector::beginInputFloat3_Vector3(entt::meta_data data, fq::reflect::IHandle* handle)
{
	DirectX::SimpleMath::Vector3 v = data.get(handle->GetHandle()).cast<DirectX::SimpleMath::Vector3>();
	std::string memberName = fq::reflect::GetName(data);

	float f[3]{ v.x,v.y,v.z };

	ImGui::InputFloat3(memberName.c_str(), f);

	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		mEditorProcess->mCommandSystem->Push<SetMetaData>(
			data, mSelectObject, handle, DirectX::SimpleMath::Vector3(f[0], f[1], f[2]));
	}

	beginIsItemHovered_Comment(data);
}

void fq::game_engine::Inspector::beginInputFloat3_Quaternion(entt::meta_data data, fq::reflect::IHandle* handle)
{
	using namespace DirectX::SimpleMath;

	Quaternion quatarnion = data.get(handle->GetHandle()).cast<DirectX::SimpleMath::Quaternion>();
	std::string memberName = fq::reflect::GetName(data);

	Vector3 euler = quatarnion.ToEuler();

	float f[3]{ euler.x,euler.y,euler.z };

	ImGui::InputFloat3(memberName.c_str(), f);

	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		euler.x = f[0];
		euler.y = f[1];
		euler.z = f[2];
		quatarnion = Quaternion::CreateFromYawPitchRoll(euler);

		mEditorProcess->mCommandSystem->Push<SetMetaData>(
			data, mSelectObject, handle, quatarnion);
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

	ImGui::SameLine();

	// 추가 버튼
	if (ImGui::Button("+##add_component_button"))
	{
		auto& type = mComponentTypes[mCurrentAddComponentIndex];
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
		std::string addText = "Add##AddButton";
		if (ImGui::Button(addText.c_str()))
		{
			auto baseValue = view.value_type().construct();
			auto last = view.begin();
			view.insert(last, baseValue);
			mEditorProcess->mCommandSystem->Push<SetMetaData>(
				data, mSelectObject, handle, any);
		}

		ImGui::SameLine();

		// Delete element
		std::string deleteText = "Delete##DelteButton";
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

	// string 특수화 
	if (valueType == entt::resolve<std::string>())
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

