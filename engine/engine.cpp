#include "engine.hpp"

#include "types/types.hpp"

#include "../settings/settings.hpp"
#include "../renderer/menu/config/config.hpp"

std::uintptr_t engine::values::process_base = 0;

std::once_flag setup_bases;

static HWND game;

std::pair< std::int32_t, std::int32_t > engine::tools::get_screen_size( )
{
	if ( !game )
		game = lf( FindWindowA )( xs( "UnrealWindow" ), nullptr );

	RECT screen_dimensions{ };

	if ( lf( GetClientRect )( game, &screen_dimensions ) )
	{
		return { screen_dimensions.right - screen_dimensions.left, screen_dimensions.bottom - screen_dimensions.top };
	}

	return { 0, 0 };
}

void engine::functions::initiate( )
{
	std::call_once( setup_bases, [=]()
		{
			engine::values::process_base = driver.base( );
		});
}