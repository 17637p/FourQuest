#include "DecalObject.h"

namespace fq::graphics
{
	DecalObject::DecalObject(std::shared_ptr<IDecalMaterial> iDecalMaterial, const DecalInfo& decalInfo, const DirectX::SimpleMath::Matrix& transform)
		: mIDecalMaterial(iDecalMaterial)
		, mDecalInfo(decalInfo)
		, mTransform(transform)
		, mbIsDirtyLayer(false)
		, mLayer(0u)
		, mPrevLayer(0u)
	{
	}

	void DecalObject::SetLayer(unsigned int layer)
	{
		if (!mbIsDirtyLayer)
		{
			mPrevLayer = mLayer;
			mbIsDirtyLayer = true;
		}

		mLayer = std::min<unsigned int>(layer, DecalObject::MAX_LAYER);
	}
}