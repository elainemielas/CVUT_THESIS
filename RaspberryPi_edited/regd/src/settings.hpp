/**
 * @file   settings.hpp
 * @Author Bohdan Vico (vicobohd@fit.cvut.cz)
 * @date   November, 2014
 * @brief  parse switches and configuration file, generate config struct
 *
 */

#ifndef SETTINGS
#define SETTINGS


typedef struct Tconfig
{
	Tconfig();
	~Tconfig();
	// main config
	int   m_daemon;
	char *m_socket_name;
	char *m_pid_file;
	// serial link config
	char *m_device;
	int   m_bound;
	int   m_parity;
	int   m_stopbit;
	//...
} Tconfig;

Tconfig* switch_parser( int argv, char **argc );
void delete_config( Tconfig *x );

#endif