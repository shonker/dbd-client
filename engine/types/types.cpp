#include <mutex>

#include "types.hpp"

engine::types::uobject engine::types::object_array::get_object_by_id( std::uint32_t id ) const
{
	std::uint32_t chunk_index = id / 65536;

	std::uintptr_t chunk = driver.read<std::uintptr_t>( this->objects + chunk_index * sizeof( std::uintptr_t ) );

	std::uint32_t within_chunk_index = id % 65536 * 24;

	return { driver.read<std::uintptr_t>( chunk + within_chunk_index * sizeof( std::uintptr_t ) ) };
}

std::string engine::types::uobject::get_name( bool class_name ) const
{
	auto object = instance;

	if ( class_name )
		object = driver.read<std::uintptr_t>( object + 0x10 );

	const auto name_index = driver.read<std::uint32_t>( object + 0x18 );

	return engine::types::name_pool::get( ).get_name_entry( name_index ).get_name( );
}

std::string engine::types::fname_entry::get_name( ) const
{
	auto info = driver.read<std::uint16_t>( instance + 0x4 );
	auto len = info >> 1;

	return driver.read_string( instance + 0x6, len );
}

engine::types::fname_entry engine::types::name_pool::get_name_entry( std::uint32_t id ) const
{
	const std::uint32_t block = id >> 16;
	const std::uint32_t offset = id & 65535;

	const auto blocks = engine::values::process_base + engine::offsets::core::names + 0x10;

	const auto name = driver.read<std::uintptr_t>( blocks + block * sizeof( std::uintptr_t ) ) + 0x4 * offset;

	return { name };
}

void engine::types::uworld::print_actors( ) const
{
	const auto level = driver.read<std::uintptr_t>( instance + engine::offsets::world::persistent_level );

	const auto actors = driver.read<std::uintptr_t>( level + engine::offsets::world::actors );
	const auto actors_count = driver.read<std::uint32_t>( level + engine::offsets::world::actors + sizeof( std::uintptr_t ) );

	std::printf( "Level: 0x%llX\n\n", level );

	for ( auto i = 0u; i < actors_count; ++i )
	{
		auto actor = engine::types::uobject{ driver.read<std::uintptr_t>( actors + i * sizeof( std::uintptr_t ) ) };

		if ( !actor.instance )
			continue;

		const auto actor_name = actor.get_name( false );
		const auto actor_class_name = actor.get_name( true );

		std::printf( "Actor: 0x%llX | 0x%llX | %s | %s\n", actor.instance, actor.get_vtable( ) - engine::values::process_base, actor_name.c_str( ), actor_class_name.c_str( ) );
	}
}

engine::types::local_player engine::types::uworld::get_local_player( ) const
{
	const auto game_instance = driver.read<std::uintptr_t>( instance + engine::offsets::world::owning_game_instance );
	const auto local_players = driver.read<std::uintptr_t>( game_instance + engine::offsets::world::local_players );

	return { driver.read<std::uintptr_t>( local_players ) };
}

engine::types::game_state engine::types::uworld::get_game_state( ) const
{
	return { driver.read<std::uintptr_t>( instance + engine::offsets::world::game_state ) };
}

engine::types::camera_manager engine::types::local_player::get_camera_manager( ) const
{
	const auto lp_controller = driver.read<std::uintptr_t>( instance + engine::offsets::local_player::player_controller );

	return { driver.read<std::uintptr_t>( lp_controller + engine::offsets::player_controller::camera_manager ) };
}

engine::types::skill_check engine::types::local_player::get_skill_check( ) const
{
	const auto interaction_handler = driver.read<std::uintptr_t>( this->get_pawn( ).instance + engine::offsets::dbd_player::interaction_handler );

	return { driver.read<std::uintptr_t>( interaction_handler + engine::offsets::skill_check::self ) };
}

engine::types::dbd_player engine::types::local_player::get_pawn( ) const
{
	const auto lp_controller = driver.read<std::uintptr_t>( instance + engine::offsets::local_player::player_controller );

	return { driver.read<std::uintptr_t>( lp_controller + engine::offsets::acontroller::pawn ) };
}

engine::structures::matrix3x4_t engine::types::camera_manager::get_rotation_matrix( ) const
{
	engine::structures::matrix3x4_t rotation_matrix;

	const auto camera_rotation = get_rotation( );

	const auto p = camera_rotation.x * M_PI / 180;
	const auto y = camera_rotation.y * M_PI / 180;
	const auto r = camera_rotation.z * M_PI / 180;

	const auto sp = std::sinf( p ), cp = std::cosf( p ), sy = std::sinf( y ), cy = std::cosf( y ), sr = std::sinf( r ), cr = std::cosf( r );

	rotation_matrix.matrix3x4[0][0] = cp * cy; rotation_matrix.matrix3x4[0][1] = cp * sy; rotation_matrix.matrix3x4[0][2] = sp; rotation_matrix.matrix3x4[0][3] = 0.f;
	rotation_matrix.matrix3x4[1][0] = sr * sp * cy - cr * sy; rotation_matrix.matrix3x4[1][1] = sr * sp * sy + cr * cy; rotation_matrix.matrix3x4[1][2] = -sr * cp; rotation_matrix.matrix3x4[1][3] = 0.f;
	rotation_matrix.matrix3x4[2][0] = -( cr * sp * cy + sr * sy ); rotation_matrix.matrix3x4[2][1] = cy * sr - cr * sp * sy; rotation_matrix.matrix3x4[2][2] = cr * cp; rotation_matrix.matrix3x4[2][3] = 0.f;

	return rotation_matrix;
}

bool engine::types::camera_manager::world_to_screen( engine::structures::vector3_t& in, engine::structures::vector3_t& out ) const
{
	const auto rotation_matrix = get_rotation_matrix( );

	engine::structures::vector3_t x{ rotation_matrix.matrix3x4[0][0], rotation_matrix.matrix3x4[0][1], rotation_matrix.matrix3x4[0][2] };
	engine::structures::vector3_t y{ rotation_matrix.matrix3x4[1][0], rotation_matrix.matrix3x4[1][1], rotation_matrix.matrix3x4[1][2] };
	engine::structures::vector3_t z{ rotation_matrix.matrix3x4[2][0], rotation_matrix.matrix3x4[2][1], rotation_matrix.matrix3x4[2][2] };

	auto delta = in - get_position( );

	engine::structures::vector3_t transformed{ delta.dot( y ), delta.dot( z ), delta.dot( x ) };

	transformed.z = transformed.z < 1.f ? 1.f : transformed.z;

	const auto [width, height] = engine::tools::get_screen_size( );

	const auto center_x = width / 2.0f;
	const auto center_y = height / 2.0f;

	const auto fov = std::tanf( get_fov( ) * M_PI / 360.f );

	out.x = center_x + transformed.x * ( center_x / fov ) / transformed.z;
	out.y = center_y - transformed.y * ( center_x / fov ) / transformed.z;

	if ( out.x > width || out.x < 0 || out.y > height || out.y < 0 )
		return false;

	return true;
}

engine::structures::vector3_t engine::types::actor::get_origin( ) const
{
	const auto actor_root_comp = driver.read<std::uintptr_t>( instance + engine::offsets::actor::root_component );

	return driver.read<engine::structures::vector3_t>( actor_root_comp + engine::offsets::scene_component::root_position );
}

std::string engine::types::player_state::get_player_name( ) const
{
	engine::types::fstring fstr{ this->instance + engine::offsets::player_state::name_private };

	return fstr.get_string( );
}

std::string engine::types::fstring::get_string( ) const
{
	auto buffer = this->str.objects;

	std::wstring string_buffer;
	do
	{
		if ( driver.read<char>( buffer ) == '\0' )
		{
			++buffer;

			continue;
		}

		string_buffer.push_back( driver.read<char>( buffer++ ) );
	} while ( driver.read<char>( buffer ) != '\0' || driver.read<char>( buffer + 1 ) != '\0' );

	return std::string( string_buffer.begin( ), string_buffer.end( ) );
}

const char* engine::types::camper_health_component::get_health_state_str( ) const
{
	const auto health_state = this->get_health_state( );

	if ( health_state == engine::structures::health_states::alive )
		return "Alive";

	else if ( health_state == engine::structures::health_states::injured )
		return "Injured";

	else if ( health_state == engine::structures::health_states::fallen )
		return "Fallen";

	return "Dead";
}

void engine::types::mesh_component::print_bones( ) const
{
	const auto skeletal_mesh = driver.read<std::uintptr_t>( this->instance + engine::offsets::skeletal_mesh_component::skeletal_mesh );

	const auto bone_names_array = driver.read<std::uintptr_t>( skeletal_mesh + engine::offsets::skeletal_mesh_component::bone_names_array );
	const auto bone_names_array_size = driver.read<std::uint32_t>( skeletal_mesh + engine::offsets::skeletal_mesh_component::bone_names_array + sizeof( std::uintptr_t ) );

	auto name_pool = engine::types::name_pool::get( );

	for ( auto i = 0u; i < bone_names_array_size; ++i )
	{
		const auto name_index = driver.read<std::uint32_t>( bone_names_array + 0x10 * i );

		std::printf( "Index: %i | Bone: %s\n", i, name_pool.get_name_entry( name_index ).get_name( ).c_str( ) );
	}
}

engine::structures::matrix4x4_t to_scaled_matrix( const engine::structures::transform_t& transform )
{
	engine::structures::matrix4x4_t matrix;

	matrix.matrix4x4[0][3] = transform.translation.x;
	matrix.matrix4x4[1][3] = transform.translation.y;
	matrix.matrix4x4[2][3] = transform.translation.z;

	const auto x2 = transform.rotation.x + transform.rotation.x;
	const auto y2 = transform.rotation.y + transform.rotation.y;
	const auto z2 = transform.rotation.z + transform.rotation.z;

	const auto xx2 = transform.rotation.x * x2;
	const auto yy2 = transform.rotation.y * y2;
	const auto zz2 = transform.rotation.z * z2;

	matrix.matrix4x4[0][0] = ( 1.0f - ( yy2 + zz2 ) ) * transform.scale_3d.x;
	matrix.matrix4x4[1][1] = ( 1.0f - ( xx2 + zz2 ) ) * transform.scale_3d.y;
	matrix.matrix4x4[2][2] = ( 1.0f - ( xx2 + yy2 ) ) * transform.scale_3d.z;

	const auto yz2 = transform.rotation.y * z2;
	const auto wx2 = transform.rotation.w * x2;

	matrix.matrix4x4[1][2] = ( yz2 - wx2 ) * transform.scale_3d.z;
	matrix.matrix4x4[2][1] = ( yz2 + wx2 ) * transform.scale_3d.y;

	const auto xy2 = transform.rotation.x * y2;
	const auto wz2 = transform.rotation.w * z2;

	matrix.matrix4x4[0][1] = ( xy2 - wz2 ) * transform.scale_3d.y;
	matrix.matrix4x4[1][0] = ( xy2 + wz2 ) * transform.scale_3d.x;

	const auto xz2 = transform.rotation.x * z2;
	const auto wy2 = transform.rotation.w * y2;

	matrix.matrix4x4[0][2] = ( xz2 + wy2 ) * transform.scale_3d.z;
	matrix.matrix4x4[2][0] = ( xz2 - wy2 ) * transform.scale_3d.x;

	matrix.matrix4x4[3][0] = 0.0f;
	matrix.matrix4x4[3][1] = 0.0f;
	matrix.matrix4x4[3][2] = 0.0f;
	matrix.matrix4x4[3][3] = 1.0f;

	return matrix;
}

engine::structures::matrix4x4_t matrix_multiply( const engine::structures::matrix4x4_t& lhs, const engine::structures::matrix4x4_t& rhs )
{
	engine::structures::matrix4x4_t matrix;

	const auto a00 = lhs.matrix4x4[0][0]; const auto a01 = lhs.matrix4x4[0][1]; const auto a02 = lhs.matrix4x4[0][2]; const auto a03 = lhs.matrix4x4[0][3];
	const auto a10 = lhs.matrix4x4[1][0]; const auto a11 = lhs.matrix4x4[1][1]; const auto a12 = lhs.matrix4x4[1][2]; const auto a13 = lhs.matrix4x4[1][3];
	const auto a20 = lhs.matrix4x4[2][0]; const auto a21 = lhs.matrix4x4[2][1]; const auto a22 = lhs.matrix4x4[2][2]; const auto a23 = lhs.matrix4x4[2][3];
	const auto a30 = lhs.matrix4x4[3][0]; const auto a31 = lhs.matrix4x4[3][1]; const auto a32 = lhs.matrix4x4[3][2]; const auto a33 = lhs.matrix4x4[3][3];

	const auto b00 = rhs.matrix4x4[0][0]; const auto b01 = rhs.matrix4x4[0][1]; const auto b02 = rhs.matrix4x4[0][2]; const auto b03 = rhs.matrix4x4[0][3];
	const auto b10 = rhs.matrix4x4[1][0]; const auto b11 = rhs.matrix4x4[1][1]; const auto b12 = rhs.matrix4x4[1][2]; const auto b13 = rhs.matrix4x4[1][3];
	const auto b20 = rhs.matrix4x4[2][0]; const auto b21 = rhs.matrix4x4[2][1]; const auto b22 = rhs.matrix4x4[2][2]; const auto b23 = rhs.matrix4x4[2][3];
	const auto b30 = rhs.matrix4x4[3][0]; const auto b31 = rhs.matrix4x4[3][1]; const auto b32 = rhs.matrix4x4[3][2]; const auto b33 = rhs.matrix4x4[3][3];

	matrix.matrix4x4[0][0] = a00 * b00 + a10 * b01 + a20 * b02 + a30 * b03;
	matrix.matrix4x4[0][1] = a01 * b00 + a11 * b01 + a21 * b02 + a31 * b03;
	matrix.matrix4x4[0][2] = a02 * b00 + a12 * b01 + a22 * b02 + a32 * b03;
	matrix.matrix4x4[0][3] = a03 * b00 + a13 * b01 + a23 * b02 + a33 * b03;

	matrix.matrix4x4[1][0] = a00 * b10 + a10 * b11 + a20 * b12 + a30 * b13;
	matrix.matrix4x4[1][1] = a01 * b10 + a11 * b11 + a21 * b12 + a31 * b13;
	matrix.matrix4x4[1][2] = a02 * b10 + a12 * b11 + a22 * b12 + a32 * b13;
	matrix.matrix4x4[1][3] = a03 * b10 + a13 * b11 + a23 * b12 + a33 * b13;

	matrix.matrix4x4[2][0] = a00 * b20 + a10 * b21 + a20 * b22 + a30 * b23;
	matrix.matrix4x4[2][1] = a01 * b20 + a11 * b21 + a21 * b22 + a31 * b23;
	matrix.matrix4x4[2][2] = a02 * b20 + a12 * b21 + a22 * b22 + a32 * b23;
	matrix.matrix4x4[2][3] = a03 * b20 + a13 * b21 + a23 * b22 + a33 * b23;

	matrix.matrix4x4[3][0] = a00 * b30 + a10 * b31 + a20 * b32 + a30 * b33;
	matrix.matrix4x4[3][1] = a01 * b30 + a11 * b31 + a21 * b32 + a31 * b33;
	matrix.matrix4x4[3][2] = a02 * b30 + a12 * b31 + a22 * b32 + a32 * b33;
	matrix.matrix4x4[3][3] = a03 * b30 + a13 * b31 + a23 * b32 + a33 * b33;

	return matrix;
}

engine::structures::vector3_t engine::types::mesh_component::get_bone_position( engine::structures::bone_ids bone_id ) const
{
	const auto cached_bone_space = driver.read<std::uintptr_t>( this->instance + engine::offsets::skeletal_mesh_component::bone_space + driver.read<std::uint32_t>( this->instance + engine::offsets::skeletal_mesh_component::double_buffer ) * 16 );
	const auto head_transform = driver.read<engine::structures::transform_t>( cached_bone_space + static_cast<std::uint8_t>( bone_id ) * 0x30 );

	const auto bone_matrix = to_scaled_matrix( head_transform );

	engine::structures::transform_t player_transform = driver.read<engine::structures::transform_t>( this->instance + engine::offsets::skeletal_mesh_component::transform );

	const auto component_to_world_matrix = to_scaled_matrix( player_transform );

	const auto result_matrix = matrix_multiply( bone_matrix, component_to_world_matrix );

	return { result_matrix.matrix4x4[0][3], result_matrix.matrix4x4[1][3], result_matrix.matrix4x4[2][3] };
}

engine::types::collectable engine::types::inventory_component::get_item() const
{
	const auto inv_address = this->get_inv_address();

	const auto item_set = driver.read<std::uint8_t>(inv_address + 0xFC) == 3;

	auto offset = 0x140;

	if (item_set)
		offset = 0x110;

	if (driver.read<std::uint32_t>(offset + this->instance + 8) <= 0)
		return { 0 };
	else
		return { driver.read<std::uintptr_t>(driver.read<std::uintptr_t>(offset + this->instance)) };
}

std::uintptr_t engine::types::inventory_component::get_inv_address() const
{
	auto objs = engine::types::object_array::get();

	std::uintptr_t v3 = 0;
	
	auto v1 = 0;

	auto v2 = driver.read<DWORD>(this->instance + 0xC);

	if (v2 >= (int)driver.read<std::uintptr_t>(engine::values::process_base + engine::offsets::inventory_component::item_address))
		v3 = 0;
	else
		v3 = driver.read<std::uintptr_t>(objs.objects + 8 * (v2 / 0x10000)) + 24 * (v2 % 0x10000);

	if (driver.read<DWORD>((v3 + 8) & 0x20000000) == 0)
	{
		auto v4 = driver.read<std::uintptr_t>(this->instance + 0xA8);

		if (v4)
		{
			std::uintptr_t v6 = 0;

			auto v5 = driver.read<DWORD>(v4 + 12);

			if (v5 >= (int)driver.read<std::uintptr_t>(engine::values::process_base + engine::offsets::inventory_component::item_address))
				v6 = 0;
			else
				v6 = driver.read<std::uintptr_t>(objs.objects + 8 * (v5 / 0x10000)) + 24 * (v5 % 0x10000);

			if ((driver.read<DWORD>(v6 + 8) & 0x20000000) == 0)
				return driver.read<std::uintptr_t>(this->instance + 0xA8);
		}
	}

	return v1;
}
