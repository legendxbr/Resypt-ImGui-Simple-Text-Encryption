#pragma once
#include <common.h>

namespace render
{
    void style( )
    {
        ImGuiStyle* style = &ImGui::GetStyle( );
    }

    bool init( )
    {
        ImGui::CreateContext( );
        ImGuiIO& io = ImGui::GetIO( );

        ImGui::StyleColorsDark( );

        style( );

        if ( !ImGui_ImplWin32_Init( window::vars::hwnd ) )
            return false;

        return ImGui_ImplDX11_Init( device::vars::g_pd3dDevice, device::vars::g_pd3dDeviceContext );
    }

    void render( )
    {
        ImGuiIO& io = ImGui::GetIO( );
        ImGuiStyle* style = &ImGui::GetStyle( );
        auto i_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar;
        ImGui::SetNextWindowSize( { 400, 300 }, ImGuiCond_::ImGuiCond_Always );
        ImGui::Begin( xorstr( "resypt" ), nullptr,  i_flags );
        {
            ImDrawList* drawlist = ImGui::GetWindowDrawList( );
            ImVec2 pos = ImGui::GetWindowPos( );
            ImVec2 size = ImGui::GetWindowSize( );
            ImGuiWindow* window = ImGui::GetCurrentWindow( );

			std::string chars = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZç !@#$%¨&*()-_=+`^~{}[];:><,.|\/?";

			static std::string crypted;
			static std::string encrypt;
			ImGui::InputText( "text", &encrypt );
			if ( ImGui::Button( "encrypt" ) ) {
				crypted.clear( );
				auto rcrypt = others::string::randomcrypt( chars.size( ) );

				std::string tmp;
				for ( int i = 0; i < encrypt.size( ); i++ ) {
					auto letter = encrypt[ i ];
					for ( int j = 0; j < chars.size( ); j++ ) {
						if ( letter == chars[ j ] ) {
							tmp += rcrypt.first[ j ];
						}
					}
				}

				crypted += fmt::format( "Text: {}\n", others::string::string_to_hex( others::string::string_to_decimal( tmp ) ) );
				crypted += fmt::format( "Password: {}\n", others::string::string_to_hex( others::string::string_to_decimal( std::to_string( rcrypt.second ) ) ) );
				//crypted += fmt::format( "Generated: {}\n", rcrypt.first );
				others::utils::to_clipboard( 0, crypted );
			}
			
			if ( !crypted.empty( ) ) {
				ImGui::Text( crypted.c_str( ) );
			}

			static std::string decrypted;
			static std::string input, seed;
			ImGui::InputText( "decrypt text", &input );
			ImGui::InputText( "decrypt password", &seed );
			if ( ImGui::Button( "decrypt" ) ) {
				std::string s = others::string::decimalstr_to_string( others::string::hex_to_string( seed ) );
				auto rcrypt = others::string::randomcrypt( chars.size( ), stol( s ) );

				std::string inp = others::string::decimalstr_to_string( others::string::hex_to_string( input ) );
				std::string tmp;
				for ( int i = 0; i < inp.size( ); i++ ) {
					auto letter = inp[ i ];
					for ( int j = 0; j < rcrypt.first.size( ); j++ ) {
						if ( letter == rcrypt.first[ j ] ) {
							tmp += chars[ j ];
						}
					}
				}

				decrypted = tmp;
			}

			ImGui::Text( decrypted.c_str( ) );
        }
        ImGui::End( );
    }

    bool window( )
    {
        MSG msg;
        std::memset( &msg, 0, sizeof( msg ) );

        while ( msg.message != WM_QUIT ) {
            if ( LI_FN( PeekMessageA ).cached( )( &msg, nullptr, 0U, 0U, PM_REMOVE ) ) {
                LI_FN( TranslateMessage ).cached( )( &msg );
                LI_FN( DispatchMessageA ).cached( )( &msg );
                continue;
            }

            device::pre_render( );
            render( );
            device::post_render( );     
        }

        device::shutdown( );
        window::destroy_window( );
        return true;
    }
}
