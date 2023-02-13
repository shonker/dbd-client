#pragma once

namespace engine::offsets::core
{
	constexpr auto names = 0xB7A87C0;

	constexpr auto objects = 0xB800E10;

	constexpr auto world = 0xB987260;
}

namespace engine::offsets::vtables
{
	constexpr auto generator_vtable = 0x91F33B8;
	constexpr auto hatch_vtable = 0x91F7E58;
}

namespace engine::offsets::world
{
	constexpr auto persistent_level = 0x38;
	constexpr auto owning_game_instance = 0x190;
	constexpr auto game_state = 0x130;

	constexpr auto actors = 0xA0;
	constexpr auto local_players = 0x40;
}

namespace engine::offsets::game_state
{
	constexpr auto slasher = 0x4A8;
	constexpr auto players = 0x248;
	constexpr auto hatch_open = 0x5CA;
	constexpr auto searchables = 0x5E8;
	constexpr auto generators = 0x5F8;
	constexpr auto hatches = 0x618;
	constexpr auto pallets = 0x648;
}

namespace engine::offsets::actor
{
	constexpr auto root_component = 0x140;
}

namespace engine::offsets::player_state
{
	constexpr auto pawn_private = 0x298;
	constexpr auto name_private = 0x318;
}

namespace engine::offsets::health_component
{
	constexpr auto health_state = 0x1E0;
	constexpr auto is_dead = 0x1E4;
}

namespace engine::offsets::camper_player
{
	constexpr auto health_component = 0x13F8;
}

namespace engine::offsets::dbd_player
{
	constexpr auto interaction_handler = 0x8F8;
	constexpr auto inventory_component = 0x858;
}

namespace engine::offsets::character
{
	constexpr auto mesh = 0x290;
}

namespace engine::offsets::skeletal_mesh_component
{
	constexpr auto bone_space = 0x510;
	constexpr auto double_buffer = 0x554;
	constexpr auto skeletal_mesh = 0x4D8;
	constexpr auto transform = 0x1E0;

	constexpr auto bone_names_array = 0x1B8;
}

namespace engine::offsets::local_player
{
	constexpr auto player_controller = 0x38;
}

namespace engine::offsets::acontroller
{
	constexpr auto pawn = 0x268;
}

namespace engine::offsets::agenerator
{
	constexpr auto is_gen_repaired = 0x339;
	constexpr auto percent_complete = 0x348;
}

namespace engine::offsets::player_controller
{
	constexpr auto camera_manager = 0x2D0;

	constexpr auto pawn = 0x268;
}

namespace engine::offsets::inventory_component
{
	constexpr auto item_address = 0xB801EA4; //48 83 EC 08 45 33 C0
}

namespace engine::offsets::camera_manager
{
	constexpr auto camera_cache = 0x1AF0;

	constexpr auto pov_viewinfo = 0x10;

	constexpr auto camera_rotation = 0xC;
	constexpr auto camera_fov = 0x18;
}

namespace engine::offsets::scene_component
{
	constexpr auto root_position = 0x134;
	constexpr auto root_rotation = 0x140;
	constexpr auto root_scale = 0x14C;
}

namespace engine::offsets::skill_check
{
	constexpr auto self = 0x2E8; // UPlayerInteractionHandler

	constexpr auto current_progress = 0x14C;
	constexpr auto success_zone = 0x19C;
	constexpr auto rotation = 0x1A0;
	constexpr auto displayed = 0x1B4;
}