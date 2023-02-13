#pragma once

#include <windows.h>
#include <string_view>
#include <unordered_map>

#include <variant>
#include <fstream>

#include "../../../engine/structures/structures.hpp"

enum features : int32_t
{
	enemy_tracers,
	auto_skill_check,
	player_names,
	player_distance_esp,
	generator_esp,
	generator_distance_esp,
	generator_ignore_repaired,
	generator_percent_esp,
	hatch_esp,
	camper_health_esp,
	hatch_distance_esp,
	visuals_render_bone,
	player_box_esp,
	instant_skill_check,
	max_distance,
	player_item_esp,



	player_names_color,
	player_distance_color,
	player_box_color,
	player_item_color,
	generator_color,
	generator_distance_color,
	generator_percent_color,
	hatch_color,
	hatch_distance_color,
	camper_health_color,
	enemy_tracers_color
};

namespace menu::config
{
	extern std::variant< bool, int, float, engine::structures::vector4_t > config_map[];
}