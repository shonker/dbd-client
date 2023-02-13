#include "modules.hpp"

#include <thread>

#include "../settings/settings.hpp"

std::array< modules::module_t, 1 > modules::module_array = 
{
	modules::module_t{ false, 0 },
};

void modules::fire_modules( )
{
	while (true)
	{
		for (const auto& [is_testing, callback] : module_array)
		{
			if ( !is_testing || settings::values::is_testing )
				callback( );
		}

		std::this_thread::sleep_for( std::chrono::milliseconds( module_update_interval ) );
	}
}
