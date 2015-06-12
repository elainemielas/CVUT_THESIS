/**
 * @file   registers.cpp
 * @Author Bohdan Vico (vicobohd@fit.cvut.cz)
 * @date   November, 2014
 * @brief  registers overview module
 *
 */

#include "../lib/controller.hpp"

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdio.h>
#include <math.h>

#define SOCKET_NAME "@/tmp/regd"

using namespace std;

/**
 * @brief   Register module class
 *
 */
class registers : public CController {
public:
    
    virtual CView* Run()
    {
        // change some reg. value
        if( _SERVER[ "REQUEST_METHOD" ] == "POST" )
            return changeValue();

        //default
        return overview();
    }

private:
     /**
     * @brief   Change register value
     *
     * @retval CView*   Template view
     *
     */
    CView* changeValue()
    {
        char *buffer;
        int fd, reg, flag;
        int a, b, c, d;
		//CHANGE float -> int
		unsigned int val;

        sscanf( _POST[ "reg" ].getstr(), "%d", &reg );
		//CHANGE %f -> %x
        sscanf( _POST[ "value" ].getstr(), "%x", &val );
        get_attr( reg, &a, &b, &c, &d, &flag );

        if( !( fd = open_socket() ) )
            return overview();

        val = ( ( val * d - c ) * b ) / ( a * d );

		// writing value of reg
        buffer = ( char* ) malloc( ( _POST[ "reg" ].length() + _POST[ "value" ].length() + 15 ) * sizeof( char ) );
		//CHANGE %d -> %X, (int)val -> val _POST[ "reg" ].getstr() -> reg
        sprintf( buffer, "@W%X;%X\n", reg, val );

        write( fd, buffer, strlen( buffer ) );

        return overview();
    }

    /**
     * @brief   Print register overview
     *
     * @retval CView*   Template view
     *
     */
    CView* overview()
    {
        CArray &data = *( new CArray() );
        int *arr, len, flag;
        char *tmp;

        len = reg_list( &arr );

        if( len < 0 )
            data[ "error" ] = "Unable to connect with register daemon!";

        for( int i = 0; i < len; i++ )
        {
            data[ "reg" ][ arr[ i ] ][ "name" ]  = arr[ i ];
            data[ "reg" ][ arr[ i ] ][ "value" ] = get_val( arr[ i ], &flag );
            data[ "reg" ][ arr[ i ] ][ "flag" ] = flag;
            tmp = get_desc( arr[ i ] );
            if( tmp )
            {
                data[ "reg" ][ arr[ i ] ][ "desc" ] = tmp;
                free( tmp );
            }
        }
        
        free( arr );
        return new CView( "registers/overview.view", &data );   
    }

//---------------------------------------
    /**
     * @brief   Read character from file descriptor
     *
     * When character not loaded throw error
     *
     * @param [in] fd   File descriptor.
     *
     * @retval char   Character from file descriptor.
     *
     */
    char read_c( int fd )
    {
        char c;

        if( read( fd, &c, 1 ) == -1 )
            throw -1;

        return c;
    }

    /**
     * @brief   Compare function
     *
     * Throw exception when char from file descriptor and expected character are different
     *
     * @param [in] fd   File descriptor.
     * @param [in] c    Expect character.
     *
     */
    void compare( int fd, char c )
    {
        if( read_c( fd ) != c )
            throw -1;
    }

    /**
     * @brief   Get register description
     *
     * @retval char*    Register Overview.
     *
     */
    char* get_desc( int reg )
    {
        int fd, len = 0, alloc = 16;
        char c, *s;

        if( !( fd = open_socket() ) )
            return NULL;

        s = ( char* ) malloc( alloc * sizeof( char ) );

        sprintf( s, "#%X\n", reg );
        write( fd, s, strlen( s ) + 1 );

        // automata ok;string\n...
        try
        {
            compare( fd, 'o' );
            compare( fd, 'k' );
            compare( fd, ';' );
    q0:
            switch( ( c = read_c( fd ) ) )
            {
                case '\n':
                    break;
                default:
                    if( len == alloc )
                    {
                        alloc *= 2;
                        s = ( char* ) realloc( s, alloc * sizeof( char ) );
                    }
                    s[ len++ ] = c;
                    goto q0;
            }

        }
        catch( int e )
        {
            free( s );
            return NULL;
        }

        close( fd );
        s[ len ] = 0;
        return s;
    }

    /**
     * @brief   Get value from register
     *
     * @param [in] reg   Register number.
     *
     * @retval -1   Error.
     * @retval int  Register value.
     *
     */
    int get_value( int reg )
    {
        int fd;
		unsigned int val;
        char buffer[ 16 ];

        if( !( fd = open_socket() ) )
            return -2;

        //get value
        sprintf( buffer, "@R%X\n", reg );
        write( fd, buffer, strlen( buffer ) + 1 );

        if( ( val = read( fd, buffer, sizeof( buffer ) -1 ) ) == -1 )
        {
            close( fd );
            return -1;
        }

        close( fd );
        buffer[ val ] = 0;
		// CHANGE %d -> %x
        sscanf( buffer, "%x\n", &val );

        return val;
    }

    /**
     * @brief   Get register attributes
     *
     * @param [in] reg   Register number.
     * @param [in] a     Register 'a' parameter pointer for save.
     * @param [in] b     Register 'b' parameter pointer for save.
     * @param [in] c     Register 'c' parameter pointer for save.
     * @param [in] d     Register 'd' parameter pointer for save.
     * @param [in] flag  Register RW|RO flag pointer for save.
     *
     */
    void get_attr( int reg, int *a, int *b, int *c, int *d, int *flag )
    {
        char buffer[ 32 ];
        int  fd, l;

        if( !( fd = open_socket() ) )
            return;

        sprintf( buffer, "$%X\n", reg );
        write( fd, buffer, strlen( buffer ) + 1 );

        if( ( l = read( fd, buffer, sizeof( buffer ) -1 ) ) == -1 )
            return;
        buffer[ l ] = 0;

        close( fd );

        sscanf( buffer, "ok;%d;%d;%d;%d;%d", a, b, c, d, flag );
    }

    /**
     * @brief   Get converted register value.
     *
     * @param [in] reg   Register number.
     * @param [in] flag  Pointer to save flag value.
     *
     * @retval float  Register value.
     *
     */
    float get_val( int reg, int *flag )
    {
        int val;
        int a, b, c, d;

        val = get_value( reg );
        get_attr( reg, &a, &b, &c, &d, flag );

        return ( val * ( (float) a / b ) + ( (float) c / d ) );
    }

    /**
     * @brief   Get register list
     *
     * @param [in] arr   Pointer to save register numbers.
     *
     * @retval -2   Socket error.
     * @retval int  Register count.
     *
     */
    int reg_list( int **arr )
    {
        int fd, len = 0, num = 0, alloc = 0;
        char c;
        *arr = NULL;

        if( !( fd = open_socket() ) )
            return -2;

        write( fd, "@@\n", 3 );

        // automata ok;nun;num\n...
        try
        {
            compare( fd, 'o' );
            compare( fd, 'k' );
            compare( fd, ';' );
    q0:
            switch( ( c = read_c( fd ) ) )
            {
                case '\n':
                case ';':
                    // end num

                    // resize array
                    if( len == alloc )
                    {
                        alloc = ( alloc ? alloc * 2 : 5 );
                        *arr = ( int* ) realloc( *arr, alloc * sizeof( int ) );
                    }
                    (*arr)[ len++ ] = num;
                    num = 0;

                    // newline return; 
                    if( c == '\n' )
                        return len;

                    goto q0;
                    // FIXME????
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                    num = num * 16 + c - '0';
                    goto q0;
				case 'A':
				case 'B':
				case 'C':
				case 'D':
				case 'E':
				case 'F':
					num = num * 16 + c - 'A' + 10;
					goto q0;
				case 'a':
				case 'b':
				case 'c':
				case 'd':
				case 'e':
				case 'f':
					num = num * 16 + c - 'a' + 10;
					goto q0;
            }

        }
        catch( int e )
        {
            return e;
        }

        close( fd );
        return len;
    }

    /**
     * @brief   Open unix socket for communication with register daemon
     *
     * @retval 0    Error.
     * @retval int  File descriptor.
     *
     */
    int open_socket()
    {
        int         sockfd;
        int         len;
        struct sockaddr_un address;

        if( !( sockfd = socket( AF_UNIX, SOCK_STREAM, 0 ) ) )
            return 0;

        address.sun_family = AF_UNIX;
        strcpy( address.sun_path, SOCKET_NAME );

        // virtual unix socket
        if( address.sun_path[ 0 ] == '@' )
            address.sun_path[ 0 ] = 0;

        len = sizeof( sa_family_t ) + strlen( SOCKET_NAME );

        if( connect( sockfd, ( struct sockaddr* ) &address, len ) )
            return 0;

        return sockfd;
    }
};

// the class factories

extern "C" CController* create()
{
    return new registers;
}

extern "C" void destroy( CController* p )
{
    delete p;
}
