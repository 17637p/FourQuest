
#include "ClientRegistration.h"

#include "../FQReflect/FQReflect.h"
#include "PlayerMovement.h"

void fq::client::RegisterMetaData()
{
	using namespace entt::literals;

	entt::meta<PlayerMovement>()
		.type("PlayerMovement"_hs)
		.prop(reflect::prop::Name, "PlayerMovement")
		.base<game_module::Component>();


}

