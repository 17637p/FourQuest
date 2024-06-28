#pragma once
#include <string>

namespace fq::physics
{
	// 건재 : 리소스 타입 열거형
	enum class EResourceType
	{
		CONVEX_MESH,
		TRIANGLE_MESH,
		HEIGHT_FIELD,

		END
	};

	// 건재 : 리소스 타입의 갯수
	enum
	{
		RESOURCE_TYPE_COUNT = static_cast<unsigned int>(EResourceType::END)
	};

	class ResourceBase
	{
	public:
		ResourceBase(const EResourceType& type);
		virtual ~ResourceBase();

		inline const EResourceType& GetResourceType();

	private:
		EResourceType mType;
	};

	const EResourceType& ResourceBase::GetResourceType()
	{
		return mType;
	}
}