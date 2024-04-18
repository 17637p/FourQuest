#include "../FQLoader/pch.h"
#include "../FQLoader/ModelConverter.h"
#include "../FQLoader/ModelLoader.h"

int main(void)
{
	using namespace fq::loader;

	{
		ModelConverter converter;
		converter.ReadFBXFile("SkinningTest.fbx");
		converter.WriteMesh("SkinningTest/SkinningTest.mesh");
		converter.WriteMaterial("SkinningTest/SkinningTest.material");
		converter.WriteAnimation("SkinningTest/SkinningTest.animation");
		converter.WriteModel("Skinning/Skinning");
	}
	{
		auto mesh = ModelLoader::ReadMesh("SkinningTest/SkinningTest.mesh");
		auto material = ModelLoader::ReadMaterial("SkinningTest/SkinningTest.material");
		auto animation = ModelLoader::ReadAnimation("SkinningTest/SkinningTest.animation");
		auto model = ModelLoader::ReadModel("Skinning/Skinning");
	}

	return 0;
}