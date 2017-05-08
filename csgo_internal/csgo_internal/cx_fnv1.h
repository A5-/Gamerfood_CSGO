//https://github.com/elbeno/constexpr/

#pragma once

#include <cstdint>

//----------------------------------------------------------------------------
// constexpr string hashing: fnv1 & fnv1a

namespace cx
{
	namespace err
	{
		namespace
		{
			const char* fnv1_runtime_error;
			const char* fnv1a_runtime_error;
		}
	}

	namespace detail
	{
		namespace fnv
		{
			constexpr uint64_t fnv1( uint64_t h, const char* s )
			{
				return ( *s == 0 ) ? h :
					       fnv1( ( h * 1099511628211ull ) ^ static_cast< uint64_t >( *s ), s + 1 );
			}

			constexpr uint64_t fnv1a( uint64_t h, const char* s )
			{
				return ( *s == 0 ) ? h :
					       fnv1a( ( h ^ static_cast< uint64_t >( *s ) ) * 1099511628211ull, s + 1 );
			}
		}
	}

	constexpr uint64_t fnv1( const char* s )
	{
		return true ?
			       detail::fnv::fnv1( 14695981039346656037ull, s ) :
			       throw err::fnv1_runtime_error;
	}

	constexpr uint64_t fnv1a( const char* s )
	{
		return true ?
			       detail::fnv::fnv1a( 14695981039346656037ull, s ) :
			       throw err::fnv1a_runtime_error;
	}
}
