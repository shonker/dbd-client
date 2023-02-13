#pragma once

#include <mutex>
#include <unordered_map>

#include "offsets/offsets.hpp"

namespace engine::values
{
	extern std::uintptr_t process_base;
}

namespace engine::tools
{
	std::pair< std::int32_t, std::int32_t > get_screen_size( );
}

namespace engine::functions
{
	void initiate( );
}