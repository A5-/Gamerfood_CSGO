//https://github.com/elbeno/constexpr/

#pragma once
#pragma warning(disable : 4244)
#pragma warning(disable : 4146)

#include "cx_fnv1.h"
#include <cstdint>

// -----------------------------------------------------------------------------
// A constexpr implementation of Melissa O'Neill's PCG32
// see http://www.pcg-random.org/
// see cx_counter.h for the constexpr counter on which this is based
namespace cx
{
	namespace err
	{
		namespace
		{
			const char* pcg32_runtime_error;
		}
	}

	namespace pcg
	{
		// advance the pcg state
		constexpr uint64_t pcg32_advance( uint64_t s )
		{
			// for uint64_t, default multiplier = 6364136223846793005ULL
			// default increment = 1442695040888963407ULL
			return s * 6364136223846793005ULL + ( 1442695040888963407ULL | 1 );
		}

		constexpr uint64_t pcg32_advance( uint64_t s, int n )
		{
			return n == 0 ? s : pcg32_advance( pcg32_advance( s ), n - 1 );
		}

		// output function (XSH RR)
		constexpr uint32_t pcg32_xorshift( uint64_t s )
		{
			return ( ( s >> 18u ) ^ s ) >> 27u;
		}

		constexpr uint32_t pcg32_rot( uint64_t s )
		{
			return s >> 59u;
		}

		constexpr uint32_t pcg32_output( uint64_t s )
		{
			return ( pcg32_xorshift( s ) >> pcg32_rot( s ) )
					| ( pcg32_xorshift( s ) << ( ( -pcg32_rot( s ) ) & 31 ) );
		}
	}

	namespace
	{
		namespace detail_pcg
		{
			// Everything that follows here is the same as in cx_counter.h except the
			// writer, which emits the pcg32 value, and the fact that we use the seed
			// as a template parameter so that the max applies per line (by default).
			// And here, recursive template depth = MAX + 5
			constexpr int BIT_DEPTH = 6; // max value = 4095
			constexpr int MAX = ( 1 << BIT_DEPTH ); // recursive template depth = 69
			constexpr int BIT_MASK = MAX - 1;

			template< uint64_t S, int H, int L >
			struct flag1
			{
				friend constexpr int adl_flag1( flag1< S, H, L > );
			};

			template< uint64_t S, int H >
			struct flag2
			{
				friend constexpr int adl_flag2( flag2< S, H > );
			};

			template< uint64_t S, int H >
			struct r1
			{
				template< int L, int = adl_flag1( flag1< S, H, L >{} ) >
				static constexpr int reader( int, flag1< S, H, L > )
				{
					return L;
				}

				template< int L >
				static constexpr int reader(
					float, flag1< S, H, L >, int R = reader( 0, flag1< S, H, L - 1 >{} ) )
				{
					return R;
				}

				static constexpr int reader( float, flag1< S, H, 0 > )
				{
					return 0;
				}
			};

			template< uint64_t S >
			struct r2
			{
				template< int H, int = adl_flag2( flag2< S, H >{} ) >
				static constexpr int reader( int, flag2< S, H > )
				{
					return H;
				}

				template< int H >
				static constexpr int reader(
					float, flag2< S, H >, int R = reader( 0, flag2< S, H - 1 >{} ) )
				{
					return R;
				}

				static constexpr int reader( float, flag2< S, 0 > )
				{
					return 0;
				}
			};

			template< uint64_t S, int H, int L >
			struct writelo
			{
				friend constexpr int adl_flag1( flag1< S, H, L > )
				{
					return L;
				}

				static constexpr int value = L;
			};

			template< uint64_t S, int H, bool B >
			struct writehi
			{
				friend constexpr int adl_flag2( flag2< S, H > )
				{
					return H;
				}

				static constexpr int value = H;
			};

			template< uint64_t S, int H >
			struct writehi< S, H, false >
			{
				static constexpr int value = H;
			};

			// the pcg32 value: the output result of the advance of the initial state
			template< uint64_t S, int H, int L >
			struct writer
			{
				static constexpr int hi_value =
				writehi< S, H + 1, L == MAX >::value;
				static constexpr int lo_value =
				writelo< S, H, ( L & BIT_MASK ) >::value;
				static constexpr uint32_t value =
				pcg::pcg32_output( pcg::pcg32_advance( S, ( H << BIT_DEPTH ) + L ) );
			};
		}
	}

	namespace pcg
	{
		template< uint64_t S,
		          int N = 1,
		          int H = detail_pcg::r2< S >::reader( 0, detail_pcg::flag2< S, detail_pcg::MAX - 1 >{} ),
		          int L = detail_pcg::r1< S, H >::reader( 0, detail_pcg::flag1< S, H, detail_pcg::MAX >{} ) >
		inline constexpr uint32_t pcg32(
			uint32_t R = detail_pcg::writer< S, H, L + N >::value )
		{
			return true ? R :
				       throw err::pcg32_runtime_error;
		}
	}
}

// macro that seeds the rng differently every compile, and provides for
// per-line instantiations
#define cx_pcg32 cx::pcg::pcg32<cx::fnv1(__FILE__ __DATE__ __TIME__) + __LINE__>
