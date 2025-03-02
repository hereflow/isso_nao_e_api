#ifndef AUTH_HPP
#define AUTH_HPP

#include <windows.h>
#include <wininet.h>
#include <string>
#include <sstream>
#include <vector>
#include <iostream>

namespace auth
{
    static std::string username {}, password {};

    inline std::string get_hwid( )
    {
        DWORD volumeSerialNumber = 0;
        GetVolumeInformationA(
            "C:\\",
            nullptr,
            0,
            &volumeSerialNumber,
            nullptr,
            nullptr,
            nullptr,
            0
        );
        std::stringstream hwid_stream;
        hwid_stream << std::hex << volumeSerialNumber;

        return hwid_stream.str( );
    }

    struct User {
        std::string username;
        std::string password;
        std::string hwid;

        bool banned;
        bool blacklisted;
    };

    inline std::vector<User> parse_users( const std::string &response )
    {
        std::vector<User> users;
        std::istringstream iss( response );
        std::string line;

        while ( std::getline( iss, line ) )
        {
            std::istringstream line_stream( line );
            std::string credentials;
            std::string hwid_ban_info;

            if ( std::getline( line_stream, credentials, '|' ) && std::getline( line_stream, hwid_ban_info ) )
            {
                std::istringstream cred_stream( credentials );
                std::string username;
                std::string password;

                if ( std::getline( cred_stream, username, ':' ) && std::getline( cred_stream, password ) )
                {
                    std::string hwid = hwid_ban_info.substr( 0, hwid_ban_info.find( '[' ) );
                    std::string ban_info = hwid_ban_info.substr( hwid_ban_info.find( '[' ) + 1, hwid_ban_info.find( ']' ) - hwid_ban_info.find( '[' ) - 1 );
                    bool banned = ( ban_info == "true" );
                    bool blacklisted = ( ban_info == "blacklisted" );

                    users.push_back( { username, password, hwid, banned, blacklisted } );
                }
            }
        }

        return users;
    }

    inline bool login( const std::string &username, const std::string &password )
    {
        std::string hwid = get_hwid( );

        //  std::string message = "HWID: " + hwid;
        //  MessageBoxA( nullptr, message.c_str( ), "HWID Info", MB_ICONINFORMATION );

        HINTERNET hInternet = InternetOpenW( L"User-Agent", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0 );
        if ( !hInternet )
        {
            MessageBoxA( nullptr, "Falha ao conectar com a RAW.", "Erro", MB_ICONERROR );
            return false;
        }

        HINTERNET hConnect = InternetOpenUrlW( hInternet, L"COLOQUEAQUIARAW", NULL, 0, INTERNET_FLAG_RELOAD, 0 ); // LINK DE ALGUMA RAW FORMATO: flow:123|HWID[false] (POR LINHA)
        if ( !hConnect )
        {
            MessageBoxA( nullptr, "Falha ao acessar a RAW.", "Erro", MB_ICONERROR );
            InternetCloseHandle( hInternet );
            return false;
        }

        std::string response_string;
        char buffer[ 1024 ];
        DWORD bytesRead;
        while ( InternetReadFile( hConnect, buffer, sizeof( buffer ), &bytesRead ) && bytesRead > 0 )
        {
            response_string.append( buffer, bytesRead );
        }

        InternetCloseHandle( hConnect );
        InternetCloseHandle( hInternet );

        std::vector<User> users = parse_users( response_string );

        for ( const auto &user : users )
        {
            if ( user.username == username && user.password == password )
            {
                if ( user.hwid == hwid )
                {
                    if ( user.banned )
                    {
                        MessageBoxA( nullptr, "Você esta banido!", "Erro", MB_ICONERROR );
                        return false;
                    }
                    if ( user.blacklisted )
                    {
                        MessageBoxA( nullptr, "Usuario blacklist!", "Erro", MB_ICONERROR );
                        return false;
                    }
                    return true;
                }
                else
                {
                    MessageBoxA( nullptr, "HWID incompatível!", "Erro", MB_ICONERROR );
                    return false;
                }
            }
        }

        MessageBoxA( nullptr, ( "Usuario ou senha invalidos!" ), "Erro", MB_ICONERROR );
        return false;
    }
}

#endif // AUTH_HPP