#include "visuals.hpp"

#include <mutex>
#include <cmath>

#include <array>
#include <vector>
#include <format>
#include <map>

#include <unordered_map>
#include <optional>

#include "../../engine/engine.hpp"
#include "../../engine/types/types.hpp"

#include "../../settings/settings.hpp"

#include "../../renderer/renderer.hpp"
#include "../../renderer/menu/config/config.hpp"

#include "../../engine/structures/structures.hpp"

auto position_map = std::map<int, engine::structures::bone_ids>
{
	{ 0,  engine::structures::bone_ids::character },
	{ 1,  engine::structures::bone_ids::head }
};

std::mutex local_mutex;

std::optional< engine::types::actor > cached_hatch;
std::vector< engine::types::generator > cached_gens;

std::unordered_map< std::uintptr_t, std::string > cached_names;
std::unordered_map< std::uintptr_t, std::string > cached_item_names;

void modules::visuals::cache::cache_data( )
{
	while ( true )
	{
		std::this_thread::sleep_for( std::chrono::seconds( 5 ) );

		std::lock_guard<std::mutex> lock( local_mutex );

		cached_item_names.clear();
		cached_names.clear( );
		cached_gens.clear( );
		cached_hatch.reset( );

		auto world = engine::types::uworld::get( );

		auto actors = world.get_persistent_level( ).get_actors( ).get_objects( );
		auto players = world.get_game_state( ).get_players( ).get_objects( );

		for ( auto& actor : actors )
		{
			if ( actor.get_vtable( ) == engine::values::process_base + engine::offsets::vtables::generator_vtable )
				cached_gens.emplace_back( actor.instance );

			if ( actor.get_vtable( ) == engine::values::process_base + engine::offsets::vtables::hatch_vtable )
				cached_hatch = { actor.instance };
		}

		for (auto& player : players)
		{
			cached_names[player.instance] = player.get_player_name();

			const auto player_item = player.get_pawn().get_inventory_component();

			if (player_item.instance && player_item.get_item().instance)
			{
				const auto item_name = player_item.get_item().get_name();

				cached_item_names[player.instance] = item_name;
			}
		}
	}
}

void modules::visuals::callback( )
{
	static std::once_flag flag;

	std::call_once( flag, []( ) { std::thread{ cache::cache_data }.detach( ); } );

	static auto lp = engine::types::uworld::get().get_local_player();

	auto dl = ImGui::GetBackgroundDrawList();
	const auto [width, height] = engine::tools::get_screen_size();

	auto world = engine::types::uworld::get();
	auto game_state = world.get_game_state();

	auto killer = game_state.get_slasher();

	auto lp_pawn = lp.get_pawn();

	if (!lp_pawn.instance)
		return;

	const auto target_render_bone = position_map[std::get<int>(menu::config::config_map[features::visuals_render_bone])];

	auto camera_manager = lp.get_camera_manager();

	const auto lp_origin = lp_pawn.get_origin();

	if (killer.instance)
	{
		auto killer_pos = killer.get_mesh().get_bone_position(target_render_bone);

		engine::structures::vector3_t killer_screen_pos;

		auto w2s = camera_manager.world_to_screen(killer_pos, killer_screen_pos);

		if (lp_pawn.instance != killer.instance && w2s && std::get<bool>(menu::config::config_map[features::enemy_tracers]))
		{
			const auto& col = std::get< engine::structures::vector4_t >(menu::config::config_map[features::enemy_tracers_color]);

			dl->AddLine({ static_cast<float>(width) / 2, static_cast<float>(height) }, { killer_screen_pos.x, killer_screen_pos.y }, ImColor{ col.color[0], col.color[1], col.color[2], col.color[3] });
		}
	}

	auto players = game_state.get_players().get_objects();

	std::lock_guard<std::mutex> lock(local_mutex);

	for (auto& player : players)
	{
		auto player_pawn = player.get_pawn();

		if (!player_pawn.instance || player_pawn.instance == lp_pawn.instance)
			continue;

		auto player_mesh = player_pawn.get_mesh();

		auto player_pos = player_mesh.get_bone_position(target_render_bone);

		engine::structures::vector3_t player_screen_pos;

		auto w2s = camera_manager.world_to_screen(player_pos, player_screen_pos);

		if (!w2s)
			continue;

		if (std::get<bool>(menu::config::config_map[features::player_box_esp]) && target_render_bone == engine::structures::bone_ids::character)
			player_screen_pos.y += 15.f;

		if (lp_pawn.instance == killer.instance && std::get<bool>(menu::config::config_map[features::enemy_tracers]))
		{
			const auto& col = std::get< engine::structures::vector4_t >(menu::config::config_map[features::enemy_tracers_color]);

			dl->AddLine({ static_cast<float>(width) / 2, static_cast<float>(height) }, { player_screen_pos.x, player_screen_pos.y }, ImColor{ col.color[0], col.color[1], col.color[2], col.color[3] });
		}

		std::uint32_t offset = 1;

		if (std::get<bool>(menu::config::config_map[features::player_names]) && cached_names.find(player.instance) != cached_names.end())
		{
			const auto& col = std::get< engine::structures::vector4_t >(menu::config::config_map[features::player_names_color]);

			const auto& name = cached_names[player.instance];

			dl->AddText(ImGui::GetDefaultFont(), 14.f, ImVec2(player_screen_pos.x - ImGui::CalcTextSize(name.c_str()).x / 2, player_screen_pos.y), ImColor{ col.color[0], col.color[1], col.color[2], col.color[3] }, name.c_str());
		}

		if (std::get<bool>(menu::config::config_map[features::player_item_esp]) && killer.instance != player_pawn.instance && cached_item_names.find(player.instance) != cached_item_names.end())
		{
			const auto& col = std::get< engine::structures::vector4_t >(menu::config::config_map[features::player_item_color]);

			const auto& name = cached_item_names[player.instance];

			dl->AddText(ImGui::GetDefaultFont(), 14.f, ImVec2(player_screen_pos.x - ImGui::CalcTextSize(name.c_str()).x / 2, player_screen_pos.y + offset++ * 15.f), ImColor{ col.color[0], col.color[1], col.color[2], col.color[3] }, name.c_str());
		}

		if (std::get<bool>(menu::config::config_map[features::player_distance_esp]))
		{
			const auto& col = std::get< engine::structures::vector4_t >(menu::config::config_map[features::player_distance_color]);

			const auto dist = std::to_string(static_cast<int>(player_pos.distance(lp_origin)));

			dl->AddText(ImGui::GetDefaultFont(), 14.f, ImVec2(player_screen_pos.x - ImGui::CalcTextSize(dist.c_str()).x / 2, player_screen_pos.y + offset++ * 15.f), ImColor{ col.color[0], col.color[1], col.color[2], col.color[3] }, dist.c_str());
		}

		if (std::get<bool>(menu::config::config_map[features::camper_health_esp]) && killer.instance != player_pawn.instance)
		{
			const auto& col = std::get< engine::structures::vector4_t >(menu::config::config_map[features::camper_health_color]);

			auto camper = engine::types::camper_player{ player_pawn.instance };

			const auto health_state = camper.get_health_component().get_health_state_str();

			dl->AddText(ImGui::GetDefaultFont(), 14.f, ImVec2(player_screen_pos.x - ImGui::CalcTextSize(health_state).x / 2, player_screen_pos.y + offset++ * 15.f), ImColor{ col.color[0], col.color[1], col.color[2], col.color[3] }, health_state);
		}

		if (std::get<bool>(menu::config::config_map[features::player_box_esp]))
		{
			if (killer.instance != player_pawn.instance)
			{
				auto health_component = engine::types::camper_player{ player_pawn.instance }.get_health_component();

				if (health_component.get_health_state() == engine::structures::health_states::fallen || health_component.is_dead())
					continue;
			}

			auto head_position = player_mesh.get_bone_position(engine::structures::bone_ids::head);
			auto character_position = player_mesh.get_bone_position(engine::structures::bone_ids::character);

			engine::structures::vector3_t head_screen_pos, character_screen_pos;

			if (camera_manager.world_to_screen(head_position, head_screen_pos) && camera_manager.world_to_screen(character_position, character_screen_pos))
			{
				character_screen_pos.y += 15.f;

				const auto width = std::fabs(head_screen_pos.y - character_screen_pos.y) / 4.0f;
				const auto height = std::fabs(head_screen_pos.y - character_screen_pos.y) * 1.25f;

				const auto line_len_width = width / 2.5f;

				const auto& col = std::get< engine::structures::vector4_t >(menu::config::config_map[features::player_box_color]);

				dl->AddLine({ character_screen_pos.x - width, character_screen_pos.y - height }, { character_screen_pos.x - width + line_len_width, character_screen_pos.y - height }, ImColor{ col.color[0], col.color[1], col.color[2], col.color[3] });
				dl->AddLine({ character_screen_pos.x - width, character_screen_pos.y - height }, { character_screen_pos.x - width, (character_screen_pos.y - height) + line_len_width }, ImColor{ col.color[0], col.color[1], col.color[2], col.color[3] });

				dl->AddLine({ character_screen_pos.x + width, character_screen_pos.y - height }, { character_screen_pos.x + width - line_len_width, character_screen_pos.y - height }, ImColor{ col.color[0], col.color[1], col.color[2], col.color[3] });
				dl->AddLine({ character_screen_pos.x + width, character_screen_pos.y - height }, { character_screen_pos.x + width, (character_screen_pos.y - height) + line_len_width }, ImColor{ col.color[0], col.color[1], col.color[2], col.color[3] });

				dl->AddLine({ character_screen_pos.x - width, character_screen_pos.y }, { character_screen_pos.x - width + line_len_width, character_screen_pos.y }, ImColor{ col.color[0], col.color[1], col.color[2], col.color[3] });
				dl->AddLine({ character_screen_pos.x - width, character_screen_pos.y }, { character_screen_pos.x - width, character_screen_pos.y - line_len_width }, ImColor{ col.color[0], col.color[1], col.color[2], col.color[3] });

				dl->AddLine({ character_screen_pos.x + width, character_screen_pos.y }, { character_screen_pos.x + width - line_len_width, character_screen_pos.y }, ImColor{ col.color[0], col.color[1], col.color[2], col.color[3] });
				dl->AddLine({ character_screen_pos.x + width, character_screen_pos.y }, { character_screen_pos.x + width, character_screen_pos.y - line_len_width }, ImColor{ col.color[0], col.color[1], col.color[2], col.color[3] });

				dl->AddLine({ character_screen_pos.x - width + 1, character_screen_pos.y - height + 1 }, { character_screen_pos.x - width + line_len_width, character_screen_pos.y - height + 1 }, ImColor{ 0, 0, 0 });
				dl->AddLine({ character_screen_pos.x - width + 1, character_screen_pos.y - height + 1 }, { character_screen_pos.x - width + 1, (character_screen_pos.y - height) + line_len_width }, ImColor{ 0, 0, 0 });

				dl->AddLine({ character_screen_pos.x + width - 1, character_screen_pos.y - height + 1 }, { character_screen_pos.x + width - line_len_width, character_screen_pos.y - height + 1 }, ImColor{ 0, 0, 0 });
				dl->AddLine({ character_screen_pos.x + width - 1, character_screen_pos.y - height + 1 }, { character_screen_pos.x + width - 1, (character_screen_pos.y - height) + line_len_width }, ImColor{ 0, 0, 0 });

				dl->AddLine({ character_screen_pos.x - width + 1, character_screen_pos.y - 1 }, { character_screen_pos.x - width + line_len_width, character_screen_pos.y - 1 }, ImColor{ 0, 0, 0 });
				dl->AddLine({ character_screen_pos.x - width + 1, character_screen_pos.y - 1 }, { character_screen_pos.x - width + 1, character_screen_pos.y - line_len_width - 1 }, ImColor{ 0, 0, 0 });

				dl->AddLine({ character_screen_pos.x + width - 1, character_screen_pos.y - 1 }, { character_screen_pos.x + width - line_len_width, character_screen_pos.y - 1 }, ImColor{ 0, 0, 0 });
				dl->AddLine({ character_screen_pos.x + width - 1, character_screen_pos.y - 1 }, { character_screen_pos.x + width - 1, character_screen_pos.y - line_len_width - 1 }, ImColor{ 0, 0, 0 });
			}
		}
	}

	if (std::get<bool>(menu::config::config_map[features::generator_esp]))
	{
		for (auto& gen : cached_gens)
		{
			if (std::get<bool>(menu::config::config_map[features::generator_ignore_repaired]) && gen.is_repaired())
				continue;

			auto gen_pos = gen.get_origin();

			engine::structures::vector3_t gen_screen_pos;

			auto w2s = camera_manager.world_to_screen(gen_pos, gen_screen_pos);

			if (!w2s)
				continue;

			const auto& col = std::get< engine::structures::vector4_t >(menu::config::config_map[features::generator_color]);

			dl->AddText(ImGui::GetDefaultFont(), 14.f, ImVec2(gen_screen_pos.x - ImGui::CalcTextSize("Generator").x / 2, gen_screen_pos.y), ImColor{ col.color[0], col.color[1], col.color[2], col.color[3] }, "Generator");

			std::uint32_t offset = 1;

			if (std::get<bool>(menu::config::config_map[features::generator_distance_esp]))
			{
				const auto& col = std::get< engine::structures::vector4_t >(menu::config::config_map[features::generator_distance_color]);

				const auto dist = std::to_string(static_cast<int>(gen_pos.distance(lp_origin)));

				dl->AddText(ImGui::GetDefaultFont(), 14.f, ImVec2(gen_screen_pos.x - ImGui::CalcTextSize(dist.c_str()).x / 2, gen_screen_pos.y + offset++ * 15.f), ImColor{ col.color[0], col.color[1], col.color[2], col.color[3] }, dist.c_str());
			}

			if (std::get<bool>(menu::config::config_map[features::generator_percent_esp]))
			{
				const auto& col = std::get< engine::structures::vector4_t >(menu::config::config_map[features::generator_percent_color]);

				auto percent = std::to_string(gen.get_percent_complete()).substr(0, 4); percent.append("/1.00");

				dl->AddText(ImGui::GetDefaultFont(), 14.f, ImVec2(gen_screen_pos.x - ImGui::CalcTextSize(percent.c_str()).x / 2, gen_screen_pos.y + offset++ * 15.f), ImColor{ col.color[0], col.color[1], col.color[2], col.color[3] }, percent.c_str());
			}
		}
	}

	if (std::get<bool>(menu::config::config_map[features::hatch_esp]) && cached_hatch.has_value())
	{
		auto& hatch = cached_hatch.value();

		auto hatch_pos = hatch.get_origin();

		engine::structures::vector3_t hatch_screen_pos;

		auto w2s = camera_manager.world_to_screen(hatch_pos, hatch_screen_pos);

		std::string hatch_text = "Hatch: ";

		hatch_text.append(game_state.is_hatchet_open() ? "Open" : "Closed");

		if (w2s)
		{
			const auto& col = std::get< engine::structures::vector4_t >(menu::config::config_map[features::hatch_color]);

			dl->AddText(ImGui::GetDefaultFont(), 14.f, ImVec2(hatch_screen_pos.x - ImGui::CalcTextSize(hatch_text.c_str()).x / 2, hatch_screen_pos.y), ImColor{ col.color[0], col.color[1], col.color[2], col.color[3] }, hatch_text.c_str());
		}

		std::uint32_t offset = 1;

		if (w2s && std::get<bool>(menu::config::config_map[features::hatch_distance_esp]))
		{
			const auto& col = std::get< engine::structures::vector4_t >(menu::config::config_map[features::hatch_distance_color]);

			const auto dist = std::to_string(static_cast<int>(hatch_pos.distance(lp_origin)));

			dl->AddText(ImGui::GetDefaultFont(), 14.f, ImVec2(hatch_screen_pos.x - ImGui::CalcTextSize(dist.c_str()).x / 2, hatch_screen_pos.y + offset++ * 15.f), ImColor{ col.color[0], col.color[1], col.color[2], col.color[3] }, dist.c_str());
		}
	}
}