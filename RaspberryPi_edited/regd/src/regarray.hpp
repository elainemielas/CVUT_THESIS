/**
 * @file   regarray.hpp
 * @Author Bohdan Vico (vicobohd@fit.cvut.cz)
 * @date   November, 2014
 * @brief  register array database class
 *
 */

#ifndef REG_ARRAY
#define REG_ARRAY

#include <stdio.h>
#include <pthread.h>

class Cregarray
{
public:
	/**
	 * @brief   Constructor
	 *
	 */
	Cregarray();

	/**
	 * @brief   Destructor
	 *
	 */
	~Cregarray();

	/**
	 * @brief   Add register method
	 *
	 * @param [in] reg 		Register number
	 * @param [in] desc 	Register description
	 * @param [in] a 		Parameter a
	 * @param [in] b 		Parameter b
	 * @param [in] c 		Parameter c
	 * @param [in] d	 	Parameter d
	 * @param [in] flag 	Register RO|RW flag
	 *
	 * @retval 0   Successfully.
	 * @retval 1   With error, duplicit name.
	 *
	 */
	int add( const int reg, 
			 const char *desc, 
			 const int a, 
			 const int b, 
			 const int c, 
			 const int d,
			 const int flag );

	/**
	 * @brief   Write value to the register
	 *
	 * @param [in] reg 		Register number
	 * @param [in] value 	Value
	 *
	 * @retval 1   Successfully.
	 * @retval 0   Error.
	 *
	 */
	int write( int reg, int value );

	/**
	 * @brief   Read value from the register
	 *
	 * @param [in] reg 		Register number
	 *
	 * @retval 0   Error.
	 * @retval int   Register value.
	 *
	 */
	int read( int reg );

	/**
	 * @brief   Read register description
	 *
	 * @param [in] reg 		Register number
	 *
	 * @retval char*   Register description.
	 *
	 */
	const char* desc( int reg );

	/**
	 * @brief   Read register 'a' parameter
	 *
	 * @param [in] reg 		Register number
	 *
	 * @retval int   Register parameter value.
	 *
	 */
	int a( int reg );

	/**
	 * @brief   Read register 'b' parameter
	 *
	 * @param [in] reg 		Register number
	 *
	 * @retval int   Register parameter value.
	 *
	 */
	int b( int reg );

	/**
	 * @brief   Read register 'c' parameter
	 *
	 * @param [in] reg 		Register number
	 *
	 * @retval int   Register parameter value.
	 *
	 */
	int c( int reg );

	/**
	 * @brief   Read register 'd' parameter
	 *
	 * @param [in] reg 		Register number
	 *
	 * @retval int   Register parameter value.
	 *
	 */
	int d( int reg );

	/**
	 * @brief   Read register flag parameter
	 *
	 * @param [in] reg 		Register number
	 *
	 * @retval 0   Register is RW
	 * @retval 1   Register is RO
	 *
	 */
	int flag( int reg );

	/**
	 * @brief   Read list of available registers
	 *
	 * return register number separated with semicolon 
	 *
	 * @retval char*   register list
	 *
	 */
	char* list();
private:
	struct Treg
	{
		Treg( const int reg, 
			  const char *desc, 
			  const int a, 
			  const int b, 
			  const int c, 
			  const int d,
			  const int flag,
			  const long int seek,
			  const int value );
		~Treg();
		int   m_reg;
		int   m_value;
		char *m_desc;
		int   m_a;
		int   m_b;
		int   m_c;
		int   m_d;
		int   m_flag;
		long int m_seek;
		pthread_mutex_t m_mux;

	};

	Treg*    find( int reg );
	long int seek( int reg, int *value );

	int    m_len;
	int    m_alloc;
	Treg **m_root;
	FILE  *m_savefile;
};

#endif