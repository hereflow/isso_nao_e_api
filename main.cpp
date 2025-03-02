#include "auth.hpp"
#include <string>
#include <iostream>
#include <windows.h>

#pragma comment(lib, "wininet.lib")

int main( )
{
    std::cout << "Username: ";
    std::getline( std::cin, auth::username );

    std::cout << "Password: ";
    std::getline( std::cin, auth::password );

    if ( auth::login( auth::username, auth::password ) )
    {
        std::cout << "Autenticacao feita com sucesso!" << std::endl;
        Sleep( 3000 );
    }
    else
    {
        std::cout << "Falha na autenticacao. Quer tentar novamente? (s/n): ";
        char res;
        std::cin >> res;
        if ( res == 's' || res == 'S' )
        {
            main( );
        }
        else if ( res == 'flow' || res == 'FLOW' )
        {
            std::cout << "Oi, eu sou o flow caso queira comprar um api de verdade me chame no discord @fq3p!";
        }
        else
        {
            std::cout << "Saindo do pograma..." << std::endl;
        }
    }

    return 0;
}
