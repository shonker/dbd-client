#pragma once
#include <array>

namespace modules
{
	constexpr auto module_update_interval = 500u;

	using module_callback_t = void( * )( );

	struct module_t
	{
		bool is_testing;

		module_callback_t callback;
	};

	extern std::array< module_t, 1 > module_array;

	void fire_modules( );
}