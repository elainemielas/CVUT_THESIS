/**
 * @file   main.cpp
 * @Author Bohdan Vico (vicobohd@fit.cvut.cz)
 * @date   November, 2014
 * @brief  main function to run program
 *
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>

#include <signal.h>

#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <syslog.h>
#include <pthread.h>
#include <pwd.h>
#include <grp.h>

#include "settings.hpp"
#include "logger.hpp"
#include "listeners.hpp"
#include "database.hpp"

Tconfig *config;    // configuration file structure
int      sfd;       // serial link file descriptor
int      ufd;       // unix socket file descriptor

/**
 * @brief   Signals handler function
 *
 * @param [in] signum Signal number
 *
 */
void signal_sigterm_handler( int signum )
{
    info( "caught SIGTERM, shutting down" );

    // shutdown serial link and unix socket descriptors
    shutdown( ufd, 0 );
    shutdown( sfd, 0 );
    close( sfd );
}


/**
 * @brief   Run program in background
 *
 * This function run program in background, as daemon
 *
 * @retval int Number of new pid.
 *
 */
int run_as_daemon()
{
    // our process ID and Session ID
    pid_t pid, sid;
    
    // fork off the parent process 
    pid = fork();
    if( pid < 0 )
        return pid;

    // if we got a good PID, then we can exit the parent process
    if( pid > 0 ) 
        return 0;

    // change the file mode mask
    umask( 0 );   
            
    // create a new SID for the child process
    sid = setsid();
    if( sid < 0 )
        return 0;        
    
    // change the current working directory
    if ( chdir( "/" ) < 0 ) 
            return 0;

    // close out the standard file descriptors
    close( STDIN_FILENO  );
    close( STDOUT_FILENO );
    close( STDERR_FILENO );

    return sid;
}


/**
 * @brief   Main function
 *
 * This function run first
 *
 * @retval 0 success
 * @retval 1 fail
 *
 */
int main( int argv, char **argc )
{
    pthread_t tus;  // thread for communication over unix socket
    int       pid = 0;

    // open syslog
    openlog( "regd", LOG_PERROR | LOG_PID, LOG_USER );

    // init database -> config fill it
    init_database();

    // parse switches + config file
    config = switch_parser( argv, argc );

    // parser error?
    if( !config )
    {
        delete_database();
        return 1;
    }

    // run as deamon
    if( config->m_daemon )
    {
        pid = run_as_daemon();

        // error 
        if( pid < 0 )
            emerg( "error when running as daemon" );

        // end parent process
        if( pid <= 0 )
        {            
            delete config;
            delete_database();
            closelog();
            return 0;
        }
    }

    // open unix socket
    if( -1 == ( ufd = unix_socket( config ) ) )
    {
        emerg( "error opening unix socket" );
        delete config;
        delete_database();
        closelog();
        return 1;
    }

    // alloc serial link
    if( !( sfd = serial_link( config ) ) )
    {
        emerg( "error opening serial link" );
        delete config;
        delete_database();
        closelog();
        close( ufd );
        return 1;
    }

    // info message
    info( "registerd start" );

    // Register signal and signal handler
    signal( SIGTERM, signal_sigterm_handler );
    signal( SIGINT, signal_sigterm_handler );

    // make pid file
    if( !config->m_pid_file )
    {
        emerg( "not configured pid file" );
        return 1;
    }

    FILE *pidf = fopen( config->m_pid_file, "w" );
    if( pidf )
    {
        fprintf( pidf , "%d", pid );
        fclose( pidf );
    }
    else
        error( "unable to create pid file" );

    // create thread for unix socket listener
    pthread_create( &tus, NULL, &socket_pthread, ( void* ) ufd );

    //socket_listener( ufd );
    serial_listener( sfd );

    // wait for listeners end
    pthread_join( tus, NULL );
    //pthread_join( tsl, NULL );

    closelog();         // close syslog
    delete_database();  // close database
    remove( config->m_pid_file );      // remove pid file
    delete config;      // delete config structure

    return 0;
}

