/**
 * @file   listeners.cpp
 * @Author Bohdan Vico (vicobohd@fit.cvut.cz)
 * @date   November, 2014
 * @brief  function to create listeners
 *
 */

#include "listeners.hpp"

#include <sys/un.h>
#include <sys/socket.h>

#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include <stdio.h>
#include <unistd.h>

#include "database.hpp"
#include "query_lex.cpp"

/**
 * @brief   Function to create local unix socket
 *
 * @param [in] config 	Program configuration.
 *
 * @retval int    Success, return file descriptor.
 * @retval 0  	  Fail.
 *
 */
int unix_socket( const Tconfig *config )
{

	int                sockfd;
    struct sockaddr_un addr;

    // check socket name
    if( !config->m_socket_name )
    {
    	emerg( "not configured socket name" );
    	return 0;
    }

    // UNIX domain address
    addr.sun_family = AF_UNIX;
    strncpy( addr.sun_path, config->m_socket_name, strlen( config->m_socket_name ) );
    
    // if m_socket_name[ 0 ] == @ -> mean that socket is abstract
    if( addr.sun_path[ 0 ] == '@' )
    	addr.sun_path[ 0 ] = 0;

    if ( ( sockfd = socket( AF_UNIX, SOCK_STREAM, 0 ) ) == -1 )
    {
    	emerg( "unix socket error" );
     	return -1;
    }

    if ( bind( sockfd, (struct sockaddr *) &addr, sizeof( sa_family_t ) + strlen( config->m_socket_name ) ) == -1 )
    {
    	emerg( "unix socket bind error" );
    	return -1;
    }

    listen( sockfd, 5 );

	return sockfd;
}

/**
 * @brief   Function to alloc serial link 
 *
 * @param [in] config 	Program configuration.
 *
 * @retval int    Success, return file descriptor.
 * @retval 0  	  Fail.
 *
 */
int serial_link( const Tconfig *config )
{
	int 		   sfd; 	// File descriptor for the port
	struct termios options; // serial link settings
	int 		   bound;

	if( !config->m_device )
	{
		emerg( "not configured serial link device" );
		return 0;
	}
	
	if( ( sfd = open( config->m_device, O_RDWR | O_NOCTTY | O_NDELAY) ) == -1 )
		return 0;

	//set blocking read
	fcntl( sfd, F_SETFL, 0 );

	// Get the current options for the port...
	tcgetattr( sfd, &options );

	// Set the baud rates ...
	switch( config->m_bound )
	{
		case 50: bound = B50; break;
		case 75: bound = B75; break;
		case 110: bound = B110; break;
		case 134: bound = B134; break;
		case 150: bound = B150; break;
		case 200: bound = B200; break;
		case 300: bound = B300; break;
		case 600: bound = B600; break;
		case 1200: bound = B1200; break;
		case 1800: bound = B1800; break;
		case 2400: bound = B2400; break;
		case 4800: bound = B4800; break;
		case 9600: bound = B9600; break;
		case 19200: bound = B19200; break;
		case 38400: bound = B38400; break;
		case 57600: bound = B57600; break;
		//case 76800: bound = B76800; break;
		case 115200: bound = B115200; break;
		default: bound = 0;
	}
	cfsetispeed( &options, bound );
	cfsetospeed( &options, bound );

	// Enable the receiver and set local mode...
	options.c_cflag |= (CLOCAL | CREAD);
	
	//CHANGED turn off echo
	options.c_lflag &= ~ECHO;

	//set no parity
	switch( config->m_parity )
	{
		// no parity
		case 0:
			options.c_cflag &= ~PARENB;
			options.c_cflag &= ~CSTOPB;
			options.c_cflag &= ~CSIZE;
			options.c_cflag |= CS8;
			break;
		// even parity
		case 1:
			options.c_cflag |= PARENB;
			options.c_cflag &= ~PARODD;
			options.c_cflag &= ~CSTOPB;
			options.c_cflag &= ~CSIZE;
			options.c_cflag |= CS8;
			break;
		// odd parity
		case 2:
			options.c_cflag |= PARENB;
			options.c_cflag |= PARODD;
			options.c_cflag &= ~CSTOPB;
			options.c_cflag &= ~CSIZE;
			options.c_cflag |= CS8;
			break;
		default:
			break;
	}

	// Set the new options for the port...
	tcsetattr( sfd, TCSANOW, &options );

	return sfd;
}

/**
 * @brief   Listener function to listen on local unix socket
 *
 * @param [in] fd 	File descriptor to socket.
 *
 */
void socket_listener( int fd )
{
    struct sockaddr_un client_name; 
	socklen_t          client_name_len; 
	int                client_socket_fd; 
	Cqlex			   *lex;

	client_name_len = sizeof( struct sockaddr_un );

    while( 1 )
    {
		// accept a connection
		client_socket_fd = accept( fd, ( struct sockaddr* ) &client_name, &client_name_len ); 

		if( client_socket_fd <= 0 )
			return;

		// lexer to fd
		lex = new Cqlex( client_socket_fd, 1 );

		// handle the connection
		query( *lex, client_socket_fd );

		delete lex;
		// close connection
		close ( client_socket_fd );
    }
}

/**
 * @brief   Listener function to listen on serial link
 *
 * @param [in] fd 	File descriptor to serial link.
 *
 */
void serial_listener( int fd )
{
	Cqlex lex( fd, 0 );

	while( 1 )
	{
		// query return -1 when error on fd
		if( query( lex, fd, 1 ) == -1 )
			break;
	}
}

/**
 * @brief   Crete unix socket listener thread for parallel processing
 *
 * @param [in] fd 	File descriptor to serial link.
 *
 */
void* socket_pthread( void *fd )
{
	socket_listener( (int) fd );
	printf("close socket\n");

	return NULL;
}



void* serial_pthread( void *fd )
{
	serial_listener( (int) fd );
	printf("close serial\n");

	return NULL;
}
