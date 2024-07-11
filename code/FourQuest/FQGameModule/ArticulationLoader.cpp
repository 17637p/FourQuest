#include "ArticulationLoader.h"

#include <fstream>
#include <iostream>
#include <spdlog/spdlog.h>
#include <cassert>

#include "ArticulationData.h"
#include "LinkData.h"
#include "Articulation.h"

namespace fq::game_module
{
	json MatrixToJson(const DirectX::SimpleMath::Matrix& mat)
	{
		json matJson;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				matJson[i][j] = mat(i, j);
			}
		}

		return matJson;
	}

	json Vector3ToJson(const DirectX::SimpleMath::Vector3& vec)
	{
		return json{ vec.x, vec.y, vec.z };
	}

	void saveTheLink(const std::shared_ptr<LinkData> linkData, ordered_json& json)
	{
		ordered_json JointJson;
		
		json["id"] = linkData->GetID();
		json["boneName"] = linkData->GetBoneName();
		json["parentBoneName"] = linkData->GetParentBoneName();
		json["localTransform"] = MatrixToJson(linkData->GetLocalTransform());
		json["density"] = linkData->GetDensity();
		json["ShapeType"] = linkData->GetShapeType();
		json["boxExtent"] = Vector3ToJson(linkData->GetBoxExtent());
		json["sphereRadius"] = linkData->GetSphereRadius();
		json["capsuleHalfHeight"] = linkData->GetCapsuleHalfHeight();
		json["capsuleRadius"] = linkData->GetCapsuleRadius();
		
		JointJson["localTransform"] = MatrixToJson(linkData->GetJointLocalTransform());
		JointJson["damping"] = linkData->GetJointDamping();
		JointJson["maxForce"] = linkData->GetJointMaxForce();
		JointJson["stiffness"] = linkData->GetJointStiffness();
		JointJson["swing1AxisMotion"] = linkData->GetSwing1AxisMotion();
		JointJson["swing1LimitHigh"] = linkData->GetSwing1LimitHigh();
		JointJson["swing1LimitLow"] = linkData->GetSwing1LimitLow();
		JointJson["swing2AxisMotion"] = linkData->GetSwing2AxisMotion();
		JointJson["swing2LimitHigh"] = linkData->GetSwing2LimitHigh();
		JointJson["swing2LimitLow"] = linkData->GetSwing2LimitLow();
		JointJson["TwistAxisMotion"] = linkData->GetTwistAxisMotion();
		JointJson["TwistLimitHigh"] = linkData->GetTwistLimitHigh();
		JointJson["TwistLimitLow"] = linkData->GetTwistLimitLow();

		json["joint"] = JointJson;

		// Recursively process children
		ordered_json childrenJson;
		for (const auto& [name, childLink] : linkData->GetChildrenLinkData())
		{
			ordered_json childJson;
			saveTheLink(childLink, childJson);
			json["children"][name] = childJson;
		}
	}

	void ArticulationLoader::Save(const std::shared_ptr<ArticulationData> data, const Path& path)
	{
		ordered_json articulationJson;

		articulationJson["density"] = data->GetDensity();
		articulationJson["restitution"] = data->GetRestitution();
		articulationJson["staticFriction"] = data->GetStaticFriction();
		articulationJson["dynamicFriction"] = data->GetDynamicFriction();

		ordered_json rootLinkJson;

		saveTheLink(data->GetRootLinkData().lock(), rootLinkJson);
		articulationJson["rootLinkData"] = rootLinkJson;

		std::ofstream file(path);
		if (file.is_open())
		{
			file << articulationJson.dump(6);
			file.close();
		}
		else
		{
			assert(nullptr);
		}
	}

	DirectX::SimpleMath::Matrix JsonToMatrix(const json& matJson) 
	{
		DirectX::SimpleMath::Matrix mat;
		for (int i = 0; i < 4; i++) 
		{
			for (int j = 0; j < 4; j++) 
			{
				mat(i, j) = matJson[i][j];
			}
		}
		return mat;
	}

	DirectX::SimpleMath::Vector3 JsonToVector3(const json& vecJson) 
	{
		return DirectX::SimpleMath::Vector3(vecJson[0], vecJson[1], vecJson[2]);
	}

	std::shared_ptr<LinkData> ArticulationLoader::LoadLinkData(const json& linkJson)
	{
		auto linkData = std::make_shared<LinkData>();

		linkData->SetID(linkJson["id"]);
		linkData->SetBoneName(linkJson["boneName"]);
		linkData->SetParentBoneName(linkJson["parentBoneName"]);
		linkData->SetLocalTransform(JsonToMatrix(linkJson["localTransform"]));
		linkData->SetDensity(linkJson["density"]);
		linkData->SetShapeType(linkJson["ShapeType"]);
		linkData->SetBoxExtent(JsonToVector3(linkJson["boxExtent"]));
		linkData->SetSphereRadius(linkJson["sphereRadius"]);
		linkData->SetCapsuleHalfHeight(linkJson["capsuleHalfHeight"]);
		linkData->SetCapsuleRadius(linkJson["capsuleRadius"]);

		auto jointJson = linkJson["joint"];
		linkData->SetJointLocalTransform(JsonToMatrix(jointJson["localTransform"]));
		linkData->SetJointDamping(jointJson["damping"]);
		linkData->SetJointMaxForce(jointJson["maxForce"]);
		linkData->SetJointStiffness(jointJson["stiffness"]);
		linkData->SetSwing1AxisMotion(jointJson["swing1AxisMotion"]);
		linkData->SetSwing1LimitHigh(jointJson["swing1LimitHigh"]);
		linkData->SetSwing1LimitLow(jointJson["swing1LimitLow"]);
		linkData->SetSwing2AxisMotion(jointJson["swing2AxisMotion"]);
		linkData->SetSwing2LimitHigh(jointJson["swing2LimitHigh"]);
		linkData->SetSwing2LimitLow(jointJson["swing2LimitLow"]);
		linkData->SetTwistAxisMotion(jointJson["TwistAxisMotion"]);
		linkData->SetTwistLimitHigh(jointJson["TwistLimitHigh"]);
		linkData->SetTwistLimitLow(jointJson["TwistLimitLow"]);

		if (linkJson.contains("children"))
		{
			for (const auto& [name, childLinkJson] : linkJson["children"].items())
			{
				linkData->AddChildLinkData(name, LoadLinkData(childLinkJson));
			}
		}

		return linkData;
	}

	std::shared_ptr<ArticulationData> ArticulationLoader::LoadArticulationData(const Path& path)
	{
		assert(fs::exists(path));
		assert(path.extension() == ".articulation");

		std::ifstream readData(path);
		ordered_json articulationJson;

		if (readData.is_open())
		{
			readData >> articulationJson;
			readData.close();
		}
		else
		{
			assert(!"파일 읽기 실패!");
		}

		auto articulationData = std::make_shared<ArticulationData>();

		articulationData->SetDensity(articulationJson["density"]);
		articulationData->SetRestitution(articulationJson["restitution"]);
		articulationData->SetStaticFriction(articulationJson["staticFriction"]);
		articulationData->SetDynamicFriction(articulationJson["dynamicFriction"]);

		auto rootLinkData = LoadLinkData(articulationJson["rootLinkData"]);
		articulationData->SetRootLinkData(rootLinkData);

		return articulationData;
	}
}
