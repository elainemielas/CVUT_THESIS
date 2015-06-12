#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdio.h>

int main( int argc, char **argv )
{
    int         sockfd;
    int         len;
    struct sockaddr_un address;
    int         result;

    if( argc != 3 )
    {
        fprintf(stderr, "Usage %s [ <socket_name> | @<socket_name> ] <message>\n", *argv );
        return 1;
    }

    if( !( sockfd = socket( AF_UNIX, SOCK_STREAM, 0 ) ) )
    {
        perror( "socket() error\n" );
        return 1;
    }

    address.sun_family = AF_UNIX;
    strcpy( address.sun_path, *( argv + 1 ) );

    if( address.sun_path[ 0 ] == '@' )
        address.sun_path[ 0 ] = 0;

    len = sizeof( sa_family_t ) + strlen( *( argv + 1 ) );

    result = connect( sockfd, ( struct sockaddr* ) &address, len );

    if( result == -1 )
    {
        perror( "oops:  client1" );
        return 1;
    }

    write( sockfd, *( argv + 2 ), strlen( *( argv + 2 ) ) );

    char buffer[ 1024 ];
    len = read( sockfd, buffer, sizeof( buffer ) );
    buffer[ len ] = 0;
    printf("%s", buffer );

    close( sockfd );
    
    return 0;
}