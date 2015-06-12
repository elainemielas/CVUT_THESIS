/**
 * @file   logger.hpp
 * @Author Bohdan Vico (vicobohd@fit.cvut.cz)
 * @date   November, 2014
 * @brief  function to log errors
 *
 */

 #include "logger.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include "database.hpp"

/**
 * @brief   Emergenci error, program can not continue
 *
 * @param [in] 	message
 *
 */
void emerg( const char *msg )
{
    syslog( LOG_EMERG, "<emerg> %s", msg );
}

/**
 * @brief   Error, program continue
 *
 * @param [in] 	message
 *
 */
void error( const char *msg )
{
    syslog( LOG_ERR, "<error> %s", msg );
}

/**
 * @brief   Warning, program continue
 *
 * @param [in] 	message
 *
 */
void warn( const char *msg )
{
    syslog( LOG_WARNING, "<warn> %s", msg );
}

/**
 * @brief   Info message, program continue
 *
 * @param [in] 	message
 *
 */
void info( const char *msg )
{
    syslog( LOG_INFO, "<info> %s", msg );
}
