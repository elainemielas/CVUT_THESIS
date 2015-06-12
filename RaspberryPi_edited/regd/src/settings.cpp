/**
 * @file   settings.cpp
 * @Author Bohdan Vico (vicobohd@fit.cvut.cz)
 * @date   November, 2014
 * @brief  parse switches and configuration file, generate config struct
 *
 */

#include "string.h"

#include "stdio.h"
#include "stdlib.h"

#include "settings.hpp"
#include "logger.hpp"
#include "database.hpp"

//-----------------------------------------------------
typedef enum
{
	PID_FILE, SOCKET_NAME, RUN_BACKGROUND,
	DEVICE, BOUND, PARITY,
	IDENT, ENDFILE, EQ, INTEGER, DECIMAL,
	REGISTER, RW, RO
} symbol_type;


typedef struct lex_symbol
{
	symbol_type type;
	char       *ident;
	int         number;
	double      decimal;
} lex_symbol;

FILE 	  *g_conf_file;
lex_symbol g_lex_symbol;
char 	   g_char;
int 	   g_linenum;
//-----------------------------------------------------
//-----------------------------------------------------
/**
 * @brief   Function to print error from switch
 *
 * @param [in] msg 	message to print
 *
 */
void sw_error( const char *msg )
{
    fprintf( stderr, "in switch %s\n", msg );
}
//-----------------------------------------------------
/**
 * @brief   Function check switch and return switch type
 *
 * @param [in] x 	Argument pointer
 *
 * @retval char Switch type.
 *
 */
char get_sw( const char *x )
{
	//is switch?
	if( strlen( x ) != 2 || *x != '-' )
		return 0;

	return *( x + 1 );
}
//-----------------------------------------------------
/**
 * @brief   Function get next character from configuration file
 *
 */
void lex_get_input()
{
	g_char = fgetc( g_conf_file );
}
//-----------------------------------------------------
/**
 * @brief   Function check if word is keyword
 *
 * if word is keyword set lex type to that keyword, other way set IDENT type
 *
 */
void is_keyword() 
{
	// lex type table
	static const struct { const char* word; symbol_type symb; } keyword_table[] = {
	   { "RUN_BACKGROUND", RUN_BACKGROUND },
	   { "PID_FILE", PID_FILE },
	   { "SOCKET_NAME", SOCKET_NAME },

	   { "DEVICE", DEVICE },
	   { "BOUND", BOUND },
	   { "PARITY", PARITY },

	   { "REGISTER", REGISTER },
	   { "RW", RW },
	   { "RO", RO },

	   { NULL, ( symbol_type ) 0 }
	};

	int i = 0;

	// compare with lex type table
	while( keyword_table[ i ].word ) 
		if( !strcmp( g_lex_symbol.ident, keyword_table[ i ].word ) )
		{
			free( g_lex_symbol.ident );
			g_lex_symbol.ident = NULL;
			g_lex_symbol.type = keyword_table[ i ].symb;
			return;
		}
		else
			i++;

	g_lex_symbol.type = IDENT;
}
//-----------------------------------------------------
/**
 * @brief   Function concat new character to ident string
 *
 * @param [in] c 	New character
 *
 */
void lex_ident_add( char c )
{
	static int mem_l;
	static int id_l;

	// alloc memory for new ident
	if( !g_lex_symbol.ident )
	{
		mem_l = 12;
		g_lex_symbol.ident = ( char* ) malloc( mem_l * sizeof( char ) );
		id_l = 0;
	}

	// memory is small
	if( id_l + 1 == mem_l )
	{
		mem_l *= 2;
		g_lex_symbol.ident = ( char* ) realloc( g_lex_symbol.ident, mem_l * sizeof( char ) );
	}

	g_lex_symbol.ident[ id_l++ ] = c;
	g_lex_symbol.ident[ id_l ]	 = 0;
}
//-----------------------------------------------------
/**
 * @brief   Automat to get next lexem from configuration file
 * 
 * Automat set variable g_lex_symbol with new lexem 
 *
 */
void get_lex()
{
	g_lex_symbol.ident = NULL;
	char sign = 0;
	unsigned long long int point;

s0:
	if( g_char == ( char ) EOF )
	{
		// end of file
		g_lex_symbol.type = ENDFILE;
		return;
	}
	else if( g_char == '\n' )
	{
		// new line
		lex_get_input();
		g_linenum++;
		goto s0;
	}
	else if( g_char <= ' ' )
	{
		// white space
		lex_get_input();
		goto s0;
	}
	else if( g_char == '-' )
	{
		// negative number
		lex_get_input();
		goto s8;
	}
	else if( g_char == '#' )
	{
		// comment	
		lex_get_input();
		goto s1;
	}
	else if( g_char == '"' )
	{
		// string
		lex_get_input();
		goto s7;
	}
	else if( g_char == '=' )
	{
		// equal
		lex_get_input();
		g_lex_symbol.type = EQ;
		return;
	}
	else if( g_char == '0' )
	{
		// hex number?
		lex_get_input();
		g_lex_symbol.number = 0;
		goto s4;
	}
	else if( g_char >= '1' && g_char <= '9' )
	{	// number
		g_lex_symbol.number = g_char - '0';
		lex_get_input();
		goto s2;
	}
	else if( g_char > ' ' && g_char <= '~' )
	{
		// identifier
		lex_ident_add( g_char );
		lex_get_input();
		goto s3;
	}

// commnet
s1:
	switch( g_char )
	{
		case (char) EOF:
		case '\n':
			lex_get_input();
			g_linenum++;
			goto s0;
		default:
			lex_get_input();
			goto s1;
	}

//number
s2:
	if( g_char >= '0' && g_char <= '9' )
	{
		g_lex_symbol.number = g_lex_symbol.number * 10 + g_char - '0';
		lex_get_input();
		goto s2;
	}
	else if( g_char == '.' )
	{
		// decimal number
		lex_get_input();
		g_lex_symbol.decimal = 0;
		point = 10;
		goto s6;
	}
	else
	{
		g_lex_symbol.type = INTEGER;
		if( sign )
			g_lex_symbol.number = -g_lex_symbol.number;
		return;
	}

//identifier
s3:
	if( g_char > ' ' && g_char <= '~' && g_char != '=' )
	{
		lex_ident_add( g_char );
		lex_get_input();
		goto s3;
	}
	else
	{
		is_keyword();
		return;
	}

// hex number
s4:
	if( g_char == 'x' )
	{
		lex_get_input();
		goto s5;
	}
	else
		// dec number
		goto s2;

s5:
	// hex
	if( g_char >= '0' && g_char <= '9' )
	{
		g_lex_symbol.number = g_lex_symbol.number * 16 + g_char - '0';
		lex_get_input();
		goto s5;
	}
	else if( g_char >= 'a' && g_char <= 'f' )
	{
		g_lex_symbol.number = g_lex_symbol.number * 16 + g_char - 'a' + 10;
		lex_get_input();
		goto s5;
	}
	else if( g_char >= 'A' && g_char <= 'F' )
	{
		g_lex_symbol.number = g_lex_symbol.number * 16 + g_char - 'A' + 10;
		lex_get_input();
		goto s5;
	}
	else
	{
		g_lex_symbol.type = INTEGER;
		return;
	}

s6:
	// decimal number
	if( g_char >= '0' && g_char <= '9' )
	{
		g_lex_symbol.decimal += ( double ) ( g_char - '0' ) / point;
		point *= 10;
		lex_get_input();
		goto s6;
	}
	else
	{
		g_lex_symbol.decimal += g_lex_symbol.number;
		if( sign )
			g_lex_symbol.decimal = -g_lex_symbol.decimal;
		g_lex_symbol.type = DECIMAL;
		return;
	}

s7:
	// string "..."
	if( g_char == '"' || g_char == ( char ) EOF )
	{
		lex_get_input();
		is_keyword();
		return;
	}
	else
	{
		lex_ident_add( g_char );
		lex_get_input();
		goto s7;
	}

s8:
	// negative number?
	if( g_char >= '0' && g_char <= '9' )
	{
		// yes
		sign = 1;
		g_lex_symbol.number = 0;
		goto s2;
	}
	else
	{
		// ident
		lex_ident_add( '-' );
		lex_ident_add( g_char );
		lex_get_input();
		goto s3;
	}


}


//-----------------------------------------------------
/**
 * @brief   Function compare expect lexem with get lexem
 *
 * if get lexem not equal witch expect throw expection
 *
 */
void compare( symbol_type type )
{
	get_lex();

	if( g_lex_symbol.type != type )
		throw "comparsion error";
}
//-----------------------------------------------------
/**
 * @brief   Function to parse RW|RO flag
 *
 * @retval 0 If RW
 * @retval 1 If RO
 *
 */
int conf_flag_parse()
{
	get_lex();

	switch( g_lex_symbol.type )
	{
		case RW:
			return 0;
		case RO:
			return 1;
		default:
			throw "expect RW/RO flag";
	}

	return 0;
}
//-----------------------------------------------------
/**
 * @brief   Function to parse number
 *
 * @retval double Number
 *
 */
double conf_float_parse()
{
	get_lex();

	switch( g_lex_symbol.type )
	{
		case INTEGER:
			return ( double ) g_lex_symbol.number;
		case DECIMAL:
			return g_lex_symbol.decimal;
		default:
			throw "expect number";
	}

	return 0;
}
//-----------------------------------------------------
/**
 * @brief   Function to parse configuration file
 * 
 * parse configuration file and save directive to struct Tconfig
 * 
 * @retval struct Tconfig* 	Configuration file structure.
 *
 */
Tconfig* conf_parser( const char *path )
{
	int      reg, a, b, c, d, flag;
	char    *desc;
	Tconfig *config;

	// open conf file
	if( !( g_conf_file = fopen( path, "rt" ) ) )
	{
		fprintf( stderr, "unable to open configuration file '%s'\n", path );
		return NULL;
	}

	// alloc conf struct
	config = new Tconfig();
	
	// set line number to 1
	g_linenum = 1;

	// lexer inicialization 
	lex_get_input();
	
	// start parser
	try
	{
		do
		{
			get_lex();

			switch( g_lex_symbol.type )
			{
				// parse RUN_BACKGROUND directive
				case RUN_BACKGROUND:
					compare( EQ );
					compare( INTEGER );
					config->m_daemon = g_lex_symbol.number;
					break;
				// parse PID_FILE directive
				case PID_FILE:
					compare( EQ );
					compare( IDENT );
					config->m_pid_file = g_lex_symbol.ident;
					break;
				// parse SOCKET_NAME directive
				case SOCKET_NAME:
					compare( EQ );
					compare( IDENT );
					config->m_socket_name = g_lex_symbol.ident;
					break;
				// parse DEVICE directive
				case DEVICE:
					compare( EQ );
					compare( IDENT );
					config->m_device = g_lex_symbol.ident;
					break;
				// parse BOUND directive
				case BOUND:
					compare( EQ );
					compare( INTEGER );
					config->m_bound = g_lex_symbol.number;
					break;
				// parse PARITY directive
				case PARITY:
					compare( EQ );
					compare( INTEGER );
					config->m_parity = g_lex_symbol.number;
					break;
				// parse REGISTER directive
				case REGISTER:
					// id reg
					compare( INTEGER );
					reg = g_lex_symbol.number;
					// reg description
					compare( IDENT );
					desc = g_lex_symbol.ident;
					// value a
					compare( INTEGER );
					a = g_lex_symbol.number;
					if( !a )
						throw "in REGISTER invalid value a != 0";
					// value b
					compare( INTEGER );
					b = g_lex_symbol.number;
					// b != 0
					if( !b )
						throw "in REGISTER invalid value b != 0";
					// value c
					compare( INTEGER );
					c = g_lex_symbol.number;
					// value d
					compare( INTEGER );
					d = g_lex_symbol.number;
					// b != 0
					if( !d )
						throw "in REGISTER invalid value d != 0";
					// flag
					flag = conf_flag_parse();

					//printf("register declare: %d '%s' %f %f %d %d\n", reg, desc, min, max, bit, flag );
					if( reg_add( reg, desc, a, b, c, d, flag ) )
						throw "unable to add register, duplicate name?";
					free( desc );
					break;
				// indicate end file
				case ENDFILE:
					fclose( g_conf_file );
					return config;
				// unknown directive
				default:
					free( g_lex_symbol.ident );
					throw "unknown directive";
			}

		}
		while( 1 );
	}
	catch( const char *e )
	{
		fprintf( stderr, "in configuration file:%d %s\n", g_linenum, e );
		fclose( g_conf_file );
		delete config;
		return NULL;
	}

	fclose( g_conf_file );
	delete config;
	return NULL;
}
//-----------------------------------------------------
/**
 * @brief   Function to parse switches
 *
 * @retval struct Tconfig* 	Configuration file structure.
 *
 */
Tconfig* switch_parser( int argv, char **argc )
{
	Tconfig *config = NULL;

	if( argv < 2 )
	{
		fprintf( stderr, "Usage: %s -d <config.file>\n", *argc );
		return NULL;
	}

	for( int i = 1; i < argv; i++ )
	{
		switch( get_sw( *( argc + i ) ) )
		{
			// confgiguration file
			case 'd':
				i++;
				if( i >= argv )
				{
					sw_error( "'-d' missing filename" );
					return NULL;
				}
				config = conf_parser( *( argc + i ) );
				break;
			default:
				fprintf( stderr, "unknown switch '%s'\n", *( argc + i ) );
				break;
		}
	}

	return config;
}
//-----------------------------------------------------
Tconfig::Tconfig()
{
	// fill default values
	m_socket_name  = NULL;
	m_pid_file = NULL;
	m_device   = NULL;
	m_bound    = 9600;
	m_parity   = 1;
	m_stopbit  = 0;
	m_daemon   = 1;
}

Tconfig::~Tconfig()
{
	free( m_socket_name );
	free( m_pid_file );
	free( m_device );
}
