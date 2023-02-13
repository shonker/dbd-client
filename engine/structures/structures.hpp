#pragma once

#include <algorithm>

#include <cmath>
#include <iostream>

#define M_PI 3.14159265358979323846

namespace engine::structures
{
	enum class bone_ids : std::uint8_t
	{
		character,
		head = 67,
		stomach = 152,
		back = 154
	};

	enum class health_states : std::uint8_t
	{
		fallen,
		injured,
		alive
	};

	struct rotator_t
	{
		float x;
		float y;
		float z;

		rotator_t clamp( )
		{
			x = std::clamp( remainderf( x, 360.0f ), -89.0f, 89.0f );
			y = std::clamp( remainderf( y, 360.0f ), -180.0f, 180.0f );

			return { x, y };
		}

		rotator_t normalize( )
		{
			if ( x > 180 )
				x -= 360.0f;
			if ( x < 180 )
				x += 360.0f;
			if ( y > 180 )
				y -= 360.0f;
			if ( y < 180 )
				y += 360.0f;

			return { x, y };
		}

		const rotator_t operator*( float value ) const
		{
			return { x * value, y * value, z * value };
		}

		rotator_t operator-( const rotator_t& rot ) const
		{
			return { x - rot.x, y - rot.y, z - rot.z };
		}

		rotator_t operator+( const rotator_t& rot ) const
		{
			return { x + rot.x, y + rot.y, z + rot.z };
		}

		rotator_t operator/( const float& f ) const
		{
			return { x / f, y / f, z / f };
		}
	};

	struct quat_t
	{
		float x;
		float y;
		float z;
		float w;
	};

	struct vector2_t
	{
		float x;
		float y;

		vector2_t operator-( vector2_t value )
		{
			return { x - value.x, y - value.y };
		}

		vector2_t operator+( vector2_t value )
		{
			return { x + value.x, y + value.y };
		}

		vector2_t operator*( float value )
		{
			return { x * value, y * value };
		}

		vector2_t operator/( float value )
		{
			return { x / value, y / value };
		}

		vector2_t operator/=( float value )
		{
			x /= value;
			y /= value;

			return *this;
		}

		vector2_t operator+=( vector2_t value )
		{
			return { x += value.x, y += value.y };
		}

		vector2_t operator-=( vector2_t value )
		{
			return { x -= value.x, y -= value.y };
		}

		const float dot( const vector2_t& vec ) const
		{
			return x * vec.x + y * vec.y;
		}
	};

	struct vector3_t
	{
		float x;
		float y;
		float z;

		vector3_t( float x, float y, float z )
			: x( x ), y( y ), z( z )
		{}


		vector3_t( )
			: x( 0 ), y( 0 ), z( 0 )
		{}

		const float dot( const vector3_t& vec ) const
		{
			return x * vec.x + y * vec.y + z * vec.z;
		}

		const float distance( vector3_t vector ) const
		{
			return sqrtf( powf( vector.x - x, 2.0 ) + powf( vector.y - y, 2.0 ) + powf( vector.z - z, 2.0 ) );
		}

		const vector3_t operator*( float value ) const
		{
			return { x * value, y * value, z * value };
		}

		const float squared( ) const
		{
			return x * x + y * y + z * z;
		}

		vector3_t operator-( const vector3_t& vec ) const
		{
			return { x - vec.x, y - vec.y, z - vec.z };
		}

		vector3_t operator+( const vector3_t& vec ) const
		{
			return { x + vec.x, y + vec.y, z + vec.z };
		}

		float magnitude( )
		{ 
			return std::sqrtf( ( x * x ) + ( y * y ) + ( z * z ) ); 
		}

		vector3_t normalized( )
		{
			float len = magnitude( );
			return vector3_t( x / len, y / len, z / len );
		}

		vector3_t scaled( const float scale )
		{
			return vector3_t{ x * scale, y * scale, z * scale };
		}

		bool empty( ) const
		{
			return x == 0.f && y == 0.f && z == 0.f;
		}
	};

	struct vector4_t
	{
		float x;
		float y;
		float z;
		float w;

		float color[4] = { x, y, z, w };

		const float dot( const vector3_t& vec ) const
		{
			return x * vec.x + y * vec.y + z * vec.z;
		}

		vector3_t operator-( const vector3_t& vec ) const
		{
			return { x - vec.x, y - vec.y, z - vec.z };
		}
	};

	struct matrix3x4_t
	{
		float matrix3x4[3][4];
	};

	struct matrix4x4_t
	{
		float matrix4x4[4][4];
	};

	struct transform_t
	{
		quat_t rotation;
		vector3_t translation;
		uint32_t pad;
		vector3_t scale_3d;
	};
}