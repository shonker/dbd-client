#include "config.hpp"

#include "../menu.hpp"

/*
	enemy_tracers,
	auto_skill_check,
	player_names,
	distance_esp,
	generator_esp,
	generator_distance_esp,
	generator_ignore_repaired,
	generator_percent_esp,
	hatch_esp,
	camper_health_esp,
	hatch_distance_esp,
	visuals_render_bone,
	player_box_esp,
	instant_skill_check
*/

std::variant< bool, int, float, engine::structures::vector4_t > menu::config::config_map[]
{
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	0,
	false,
	false,
	1000,
	false,



	engine::structures::vector4_t { 1.f, 1.f, 1.f, 1.f },
	engine::structures::vector4_t { 1.f, 1.f, 1.f, 1.f },
	engine::structures::vector4_t { 1.f, 1.f, 1.f, 1.f },
	engine::structures::vector4_t { 1.f, 1.f, 1.f, 1.f },
	engine::structures::vector4_t { 1.f, 1.f, 1.f, 1.f },
	engine::structures::vector4_t { 1.f, 1.f, 1.f, 1.f },
	engine::structures::vector4_t { 1.f, 1.f, 1.f, 1.f },
	engine::structures::vector4_t { 1.f, 1.f, 1.f, 1.f },
	engine::structures::vector4_t { 1.f, 1.f, 1.f, 1.f },
	engine::structures::vector4_t { 1.f, 1.f, 1.f, 1.f },
	engine::structures::vector4_t { 1.f, 1.f, 1.f, 1.f }
};