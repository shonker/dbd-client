#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <vector>

#include "../engine.hpp"
#include "../offsets/offsets.hpp"
#include "../structures/structures.hpp"

#include "../../driver/driver.hpp"

namespace engine::types
{
	struct uobject
	{
		std::uintptr_t instance;

		uobject( ) = default;

		uobject( std::uintptr_t addr ) : instance{ addr } { }

		std::string get_name( bool class_name = false ) const;

		std::uintptr_t get_vtable( ) const { return driver.read< std::uintptr_t >( instance ); }
	};

	struct tarray
	{
		std::uintptr_t objects;

		std::uint32_t size;
	};

	template<class t>
	struct tobj_array : private tarray
	{
		static_assert( std::is_base_of_v<uobject, t>, "T must derive from UObject" );

		std::vector<t> get_objects( ) const
		{
			std::vector<t> ret;

			for ( auto i = 0u; i < size; ++i )
			{
				t actor{ driver.read<std::uintptr_t>( objects + i * sizeof( std::uintptr_t ) ) };

				if ( !actor.instance )
					continue;

				ret.push_back( actor );
			}

			return ret;
		}

		std::uint32_t const get_size( ) { return size; }
	};

	struct fstring
	{
		tarray str;

		fstring( std::uintptr_t addr ) : str{ driver.read<tarray>( addr ) } {}

		std::string get_string( ) const;
	};

	struct camera_manager : uobject
	{
		camera_manager( std::uintptr_t addr ) : uobject{ addr } { }

		engine::structures::vector3_t get_rotation( ) const { return driver.read<engine::structures::vector3_t>( this->instance + engine::offsets::camera_manager::camera_cache + engine::offsets::camera_manager::pov_viewinfo + engine::offsets::camera_manager::camera_rotation ); }

		engine::structures::vector3_t get_position( ) const { return driver.read<engine::structures::vector3_t>( this->instance + engine::offsets::camera_manager::camera_cache + engine::offsets::camera_manager::pov_viewinfo ); }

		float get_fov( ) const { return driver.read<float>( this->instance + engine::offsets::camera_manager::camera_cache + engine::offsets::camera_manager::pov_viewinfo + engine::offsets::camera_manager::camera_fov ); }

		engine::structures::matrix3x4_t get_rotation_matrix( ) const;

		bool world_to_screen( engine::structures::vector3_t& in, engine::structures::vector3_t& out ) const;
	};

	struct skill_check : uobject
	{
		skill_check( std::uintptr_t addr ) : uobject{ addr } { }

		float get_current_progress( ) const { return driver.read<float>( this->instance + engine::offsets::skill_check::current_progress ); }

		float get_success_zone( ) const { return driver.read<float>( this->instance + engine::offsets::skill_check::success_zone ); }

		float get_rotation( ) const { return driver.read<float>( this->instance + engine::offsets::skill_check::rotation ); }

		bool is_displayed( ) const { return driver.read<bool>( this->instance + engine::offsets::skill_check::displayed ); }
	};

	struct mesh_component : uobject
	{
		mesh_component( std::uintptr_t addr ) : uobject{ addr } { }

		engine::structures::transform_t get_transform( ) const { return driver.read<engine::structures::transform_t>( this->instance + engine::offsets::skeletal_mesh_component::transform ); };

		engine::structures::vector3_t get_bone_position( engine::structures::bone_ids bone_id ) const;

		void print_bones( ) const;
	};

	struct actor : uobject
	{
		actor( std::uintptr_t addr ) : uobject{ addr } { }

		engine::structures::vector3_t get_origin( ) const;
	};

	struct camper_health_component : uobject
	{
		camper_health_component( std::uintptr_t addr ) : uobject{ addr } { }

		const char* get_health_state_str( ) const;

		engine::structures::health_states get_health_state( ) const { return driver.read<engine::structures::health_states>( this->instance + engine::offsets::health_component::health_state ); }

		bool is_dead( ) const { return driver.read<bool>( this->instance + engine::offsets::health_component::is_dead ); }
	};

	struct collectable : uobject
	{
		collectable(std::uintptr_t addr) : uobject{ addr } { }
	};

	struct inventory_component : uobject
	{
		inventory_component(std::uintptr_t addr) : uobject{ addr } { }

		collectable get_item() const;

	private:
		std::uintptr_t get_inv_address() const;
	};

	struct dbd_player : actor
	{
		dbd_player( std::uintptr_t addr ) : actor{ addr } { }

		mesh_component get_mesh( ) const { return { driver.read<std::uintptr_t>( this->instance + engine::offsets::character::mesh ) }; };

		inventory_component get_inventory_component() const { return { driver.read<std::uintptr_t>(this->instance + engine::offsets::dbd_player::inventory_component) }; };
	};

	struct camper_player : dbd_player
	{
		camper_player( std::uintptr_t addr ) : dbd_player{ addr } { }

		camper_health_component get_health_component( ) const { return { driver.read<std::uintptr_t>( this->instance + engine::offsets::camper_player::health_component ) }; };
	};

	struct local_player : uobject
	{
		local_player( std::uintptr_t addr ) : uobject{ addr } { }

		camera_manager get_camera_manager( ) const; //Technically Method of PlayerController

		dbd_player get_pawn( ) const;

		skill_check get_skill_check( ) const;
	};

	struct generator : actor
	{
		bool is_repaired( ) const { return driver.read<bool>( this->instance + engine::offsets::agenerator::is_gen_repaired ); };

		float get_percent_complete( ) const { return driver.read<float>( this->instance + engine::offsets::agenerator::percent_complete ); };

		generator( std::uintptr_t addr ) : actor{ addr } { }
	};

	struct player_state : uobject
	{
		dbd_player get_pawn( ) const { return { driver.read<std::uintptr_t>( this->instance + engine::offsets::player_state::pawn_private ) }; };

		std::string get_player_name( ) const;

		player_state( std::uintptr_t addr ) : uobject{ addr } { }
	};

	struct game_state : uobject
	{
		dbd_player get_slasher( ) const { return { driver.read<std::uintptr_t>( this->instance + engine::offsets::game_state::slasher ) }; };

		tobj_array<player_state> get_players( ) const { return driver.read<tobj_array<player_state>>( this->instance + engine::offsets::game_state::players ); };

		tobj_array<generator> get_gens( ) const { return driver.read<tobj_array<generator>>( this->instance + engine::offsets::game_state::generators ); };

		tobj_array<actor> get_searchables( ) const { return driver.read<tobj_array<actor>>( this->instance + engine::offsets::game_state::searchables ); };

		tobj_array<actor> get_hatches( ) const { return driver.read<tobj_array<actor>>( this->instance + engine::offsets::game_state::hatches ); };

		tobj_array<actor> get_pallets( ) const { return driver.read<tobj_array<actor>>( this->instance + engine::offsets::game_state::pallets ); };

		bool is_hatchet_open( ) const { return driver.read<bool>( this->instance + engine::offsets::game_state::hatch_open ); };

		game_state( std::uintptr_t addr ) : uobject{ addr } { }
	};

	struct persistent_level : uobject
	{
		tobj_array<actor> get_actors( ) const { return driver.read<tobj_array<actor>>( this->instance + engine::offsets::world::actors ); };

		persistent_level( std::uintptr_t addr ) : uobject{ addr } { }
	};

	struct uworld : uobject
	{
		uworld( ) = default;

		uworld( std::uintptr_t addr ) : uobject{ addr } { }

		void print_actors( ) const;

		local_player get_local_player( ) const;

		game_state get_game_state( ) const;

		persistent_level get_persistent_level( ) const { return { driver.read< std::uintptr_t >( this->instance + engine::offsets::world::persistent_level ) }; }

		static uworld get( ) { return { driver.read< std::uintptr_t >( engine::values::process_base + engine::offsets::core::world ) }; };
	};

	struct fname_entry
	{
		std::uintptr_t instance;

		fname_entry( std::uintptr_t addr ) : instance{ addr } { }

		std::string get_name( ) const;
	};

	struct object_array
	{
		std::uintptr_t objects;
		std::uintptr_t pre_alloc_objects;

		std::uint32_t max_elements;
		std::uint32_t num_elements;
		std::uint32_t max_chunks;
		std::uint32_t num_chunks;

		uobject get_object_by_id( std::uint32_t id ) const;

		static object_array get( ) { return driver.read<object_array>( engine::values::process_base + engine::offsets::core::objects ); };
	};

	struct name_pool
	{
		std::uint64_t unk;
		std::uint32_t current_block;
		std::uint32_t current_cursor;
		std::uintptr_t blocks; //0x10

		fname_entry get_name_entry( std::uint32_t id ) const;

		static name_pool get( ) { return driver.read<name_pool>( engine::values::process_base + engine::offsets::core::names ); };
	};
}