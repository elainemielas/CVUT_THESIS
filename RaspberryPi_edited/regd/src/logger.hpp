/**
 * @file   logger.hpp
 * @Author Bohdan Vico (vicobohd@fit.cvut.cz)
 * @date   November, 2014
 * @brief  function to log errors
 *
 */

#ifndef LOGGER
#define LOGGER

/**
 * @brief   Emergenci error, program can not continue
 *
 * @param [in] 	message
 *
 */
void emerg( const char *msg );

/**
 * @brief   Error, program continue
 *
 * @param [in] 	message
 *
 */
void error( const char *msg );

/**
 * @brief   Warning, program continue
 *
 * @param [in] 	message
 *
 */
void warn( const char *msg );

/**
 * @brief   Info message, program continue
 *
 * @param [in] 	message
 *
 */
void info( const char *msg );

#endif