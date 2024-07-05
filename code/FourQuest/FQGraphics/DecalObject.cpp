#include "DecalObject.h"

namespace fq::graphics
{
	DecalObject::DecalObject(const DirectX::SimpleMath::Matrix& transform, const DecalInfo& decalInfo, std::shared_ptr<IDecalMaterial> iDecalMaterial)
		: mTransform(transform)
		, mDecalInfo(decalInfo)
		, mIDecalMaterial(iDecalMaterial)
	{
	}
}