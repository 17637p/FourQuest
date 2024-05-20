#pragma once
#include <string>

namespace fq::physics
{
	enum class EResourceType
	{
		NONE = 0,
		MATERIAL,
		CONVEX_MESH,
		ARTICULATION,

		END
	};

	class ResourceBase
	{
	public:
		ResourceBase(const EResourceType& type);
		~ResourceBase();

		inline const EResourceType& GetResourceType();

	private:
		EResourceType mType;
	};

	const EResourceType& ResourceBase::GetResourceType()
	{
		return mType;
	}
}