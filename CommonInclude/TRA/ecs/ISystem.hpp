#ifndef TRA_ECS_I_SYSTEM_HPP
#define TRA_ECS_I_SYSTEM_HPP

namespace tra::ecs
{
	class World;

	struct ISystem
	{
		~ISystem() = default;
		virtual void update(World* _world) = 0;
	};
}

#endif
