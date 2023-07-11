#pragma once
#include <common.h>
#include <implementations/utf8/utf8.h>
#include <random>

namespace others
{
	namespace string
	{
		std::string random( int i_size )
		{
			static auto& chars = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

			thread_local static std::mt19937 rg{ std::random_device{}( ) };
			thread_local static std::uniform_int_distribution<std::string::size_type> pick( 0, sizeof( chars ) - 2 );

			std::string string;
			string.reserve( i_size );

			while ( i_size-- )
				string += chars[ pick( rg ) ];

			return string;
		}

		std::pair<std::string, unsigned long> randomcrypt( int i_size, long cseed = -1 )
		{
			static auto& chars = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZç !@#$%¨&*()-_=+`^~{}[];:><,.|\/?";
			unsigned long seed = ( unsigned )time( NULL );

			if ( cseed == -1 ) {
				srand( seed );
			}
			else {
				srand( cseed );
			}

			std::string string;
			string.reserve( i_size );

			std::vector<int> generated;
			while ( i_size-- ) {
				GENERATE:
				int random = 0 + ( rand( ) % sizeof( chars ) - 1 );
				string += chars[ random ];

				for ( auto& g : generated ) {
					if ( random == g ) {
						string.pop_back( );
						goto GENERATE;
					}
				}

				generated.emplace_back( random );
			}

			return { string, seed };
		}

        std::string to_lower( std::string str )
        {
            transform( str.begin( ), str.end( ), str.begin( ), static_cast< int( * )( int ) >( ::tolower ) );
            return str;
        }

        std::string sanitize_utf8( std::string in )
        {
            std::string temp;
            utf8::replace_invalid( in.begin( ), in.end( ), back_inserter( temp ) );
            return temp;
        }

		std::string string_to_hex( const std::string str, bool capital = false )
		{
			std::string hexstr;
			hexstr.resize( str.size( ) * 2 );
			const size_t a = capital ? 'A' - 1 : 'a' - 1;

			for ( size_t i = 0, c = str[ 0 ] & 0xFF; i < hexstr.size( ); c = str[ i / 2 ] & 0xFF ) {
				hexstr[ i++ ] = c > 0x9F ? ( c / 16 - 9 ) | a : c / 16 | '0';
				hexstr[ i++ ] = ( c & 0xF ) > 9 ? ( c % 16 - 9 ) | a : c % 16 | '0';
			}

			return hexstr;
		}

		std::string hex_to_string( const std::string hexstr )
		{
			std::string str;
			str.resize( ( hexstr.size( ) + 1 ) / 2 );

			for ( size_t i = 0, j = 0; i < str.size( ); i++, j++ ) {
				str[ i ] = ( hexstr[ j ] & '@' ? hexstr[ j ] + 9 : hexstr[ j ] ) << 4, j++;
				str[ i ] |= ( hexstr[ j ] & '@' ? hexstr[ j ] + 9 : hexstr[ j ] ) & 0xF;
			}
			
			return str;
		}

		std::string string_to_decimal( const std::string str )
		{
			std::string tmp;
			for ( auto& c : str ) {
				tmp += std::to_string( ( int )c ) + ".";
			}

			return tmp;
		}

		std::string decimalstr_to_string( const std::string str )
		{
			std::string tmp;
			std::string word = "";
			for ( auto x : str ) {
				if ( x == '.' ) {
					int stoi = std::stoi( word );
					tmp += stoi;
					word = "";
				}
				else {
					word = word + x;
				}
			}

			return tmp;
		}
	}

    namespace utils
    {
        bool is_process_with_admin_rights( )
        {
            auto h_handle = LI_FN( GetCurrentProcess ).cached( )( );

            auto f_ret = false;
            HANDLE h_token = nullptr;

            if ( OpenProcessToken( h_handle, TOKEN_QUERY, &h_token ) ) {
                TOKEN_ELEVATION elevation;
                unsigned long cb_size = sizeof( TOKEN_ELEVATION );
                if ( LI_FN( GetTokenInformation ).cached( )( h_token, TokenElevation, &elevation, sizeof( elevation ), &cb_size ) )
                    f_ret = elevation.TokenIsElevated;
            }

            if ( h_token )
                LI_FN( CloseHandle ).cached( )( h_token );

            if ( h_handle )
                LI_FN( CloseHandle ).cached( )( h_handle );

            return f_ret;
        }

		void to_clipboard( HWND hwnd, const std::string &s )
		{
			OpenClipboard( hwnd );
			EmptyClipboard( );
			HGLOBAL hg = GlobalAlloc( GMEM_MOVEABLE, s.size( ) + 1 );

			if ( !hg ) {
				CloseClipboard( );
				return;
			}

			memcpy( GlobalLock( hg ), s.c_str( ), s.size( ) + 1 );
			GlobalUnlock( hg );
			SetClipboardData( CF_TEXT, hg );
			CloseClipboard( );
			GlobalFree( hg );
		}
    }
}