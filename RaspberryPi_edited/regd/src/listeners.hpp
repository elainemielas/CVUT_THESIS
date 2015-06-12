/**
 * @file   listeners.hpp
 * @Author Bohdan Vico (vicobohd@fit.cvut.cz)
 * @date   November, 2014
 * @brief  function to create listeners
 *
 */

#ifndef LISTENERS
#define LISTENERS

#include "settings.hpp"
#include "logger.hpp"

/**
 * @brief   Function to create local unix socket
 *
 * @param [in] config 	Program configuration.
 *
 * @retval int    Success, return file descriptor.
 * @retval 0  	  Fail.
 *
 */
int unix_socket( const Tconfig *config );

/**
 * @brief   Function to alloc serial link 
 *
 * @param [in] config 	Program configuration.
 *
 * @retval int    Success, return file descriptor.
 * @retval 0  	  Fail.
 *
 */
int serial_link( const Tconfig *config );

/**
 * @brief   Listener function to listen on local unix socket
 *
 * @param [in] fd 	File descriptor to socket.
 *
 */
void socket_listener( int fd );

/**
 * @brief   Listener function to listen on serial link
 *
 * @param [in] fd 	File descriptor to serial link.
 *
 */
void serial_listener( int fd );

/**
 * @brief   Crete unix socket listener thread for parallel processing
 *
 * @param [in] fd 	File descriptor to serial link.
 *
 */
void* socket_pthread( void *fd );

void* serial_pthread( void *fd );

#endif