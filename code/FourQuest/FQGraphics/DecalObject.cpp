#include "DecalObject.h"

namespace fq::graphics
{
	DecalObject::DecalObject(std::shared_ptr<IDecalMaterial> iDecalMaterial, const DecalInfo& decalInfo, const DirectX::SimpleMath::Matrix& transform)
		: mIDecalMaterial(iDecalMaterial)
		, mDecalInfo(decalInfo)
		, mTransform(transform)
	{
	}
}