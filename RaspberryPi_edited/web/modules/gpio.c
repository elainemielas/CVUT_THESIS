/**
 * @file   gpio.c
 * @Author Bohdan Vico (vicobohd@fit.cvut.cz)
 * @date   November, 2014
 * @brief  gpio library
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#define BCM2835_GPIO_BASE 0x20200000

#define GPIO_IN           0
#define GPIO_OUT          1
#define GPIO_ALT0         2
#define GPIO_ALT1         3
#define GPIO_ALT2         4
#define GPIO_ALT3         5
#define GPIO_ALT4         6
#define GPIO_PULL_OFF     0
#define GPIO_PULL_DOWN    1
#define GPIO_PULL_UP      2

#define PAGE_SIZE         (4*1024)
#define BLOCK_SIZE        (4*1024)

#define SERIAL_DEV        "/dev/ttyAMA0"

// i/o access
volatile unsigned *g_gpio;

/**
 * @brief   Set up a memory regions to access GPIO
 *
 * @param [in] first  Pointer to save first for value
 * @param [in] last   Pointer to save last for value
 *
 * @retval 0 Success.
 * @retval 1 Fail.
 *
*/
int setup_io()
{
  if( g_gpio )
    return 0;

  int   mem_fd;
  void *gpio_map;

  // open /dev/mem
  if ( ( mem_fd = open( "/dev/mem", O_RDWR | O_SYNC ) ) < 0 )
    return 1;

  // mmap GPIO
  gpio_map = mmap (
    NULL,                     // any adddress in our space will do
    BLOCK_SIZE,               // map length
    PROT_READ | PROT_WRITE,   // enable reading & writting to mapped memory
    MAP_SHARED,               // shared with other processes
    mem_fd,                   // file to map
    BCM2835_GPIO_BASE         // offset to GPIO peripheral
  );

  close( mem_fd );            // no need to keep mem_fd open after mmap

  if( gpio_map == MAP_FAILED )
    return 1;

  g_gpio = ( volatile unsigned* ) gpio_map;

  return 0;
}


/**
* @brief   Initialize GPIO pins.
*
* @param [in] pin       GPIO pin number
* @param [in] function  GPIO function, GPIO_IN | GPIO_OUT | GPIO_ALT0 | GPIO_ALT1 | GPIO_ALT2 | GPIO_ALT3 | GPIO_ALT4
*
* @retval 0 Success.
* @retval 1 Fail.
*
*/
int gpio_init( int pin, int function )
{
  //mmap
  if( setup_io() )
    return 1;

  // clear gpio bites
  *( ( unsigned* ) g_gpio + pin/10 ) &= ~( 7 << ( pin % 10 ) * 3 ); 
  // set new gpio bites
  *( ( unsigned* ) g_gpio + pin/10 ) |= function << ( pin % 10 ) * 3; 
  return 0;
}

/**
* @brief   Get GPIO function.
*
* @param [in] pin       GPIO pin number
*
* @retval -1 Fail.
* @retval int GPIO_IN | GPIO_OUT | GPIO_ALT0 | GPIO_ALT1 | GPIO_ALT2 | GPIO_ALT3 | GPIO_ALT4  On success.
*
*/
int gpio_func( int pin )
{
  if( setup_io() )
    return -1;

  // read gpio function
  return ( *( ( unsigned* ) g_gpio + pin/10 ) >> ( pin % 10 ) * 3 ) & 7; 
}

/**
* @brief   GPIO write.
*
* @param [in] pin     GPIO pin number
* @param [in] value   Value 1/0
*
* @retval 0 Success.
* @retval 1 Fail.
*
*/
int gpio_write( int pin, int value )
{
  if( setup_io() )
    return 1;

  if( value )
    // set bit
    *( ( unsigned* ) g_gpio + 7 + pin/32 ) = 1 << ( pin % 32 );
  else
    // clear bit
    *( ( unsigned* ) g_gpio + 10 + pin/32 ) = 1 << ( pin % 32 );

  return 0;
}

/**
* @brief   GPIO read.
*
* @param [in] pin     GPIO pin number
* @param [in] value   Value 1/0
*
* @retval -1 Fail.
* @retval 1/0 Success.
*
*/
int gpio_read( int pin )
{
  if( setup_io() )
    return -1;

  return ( *( ( unsigned* ) g_gpio + 13 + pin/32 ) >> ( pin % 32 ) ) & 1;
}

/**
* @brief   Set GPIO pull up/down resistor.
*
* @param [in] pin     GPIO pin number
* @param [in] pull    Pull up/down, GPIO_PULL_UP | GPIO_PULL_DOWN | GPIO_PULL_OFF
*
* @retval 1 Fail.
* @retval 0 Success.
*
*/
int gpio_pull( int pin, int pull )
{
  if( setup_io() )
    return 1;
  //GPPUD
  *( ( unsigned* ) g_gpio + 37 ) |= pull;
  usleep( 1000 );
  //GPCLK
  *( ( unsigned* ) g_gpio + 38 + pin/32 ) |= 1 << ( pin % 32 );
  usleep( 1000 );
  //GPPUD & GPCLK to 0
  *( ( unsigned* ) g_gpio + 37 ) = 0;
  *( ( unsigned* ) g_gpio + 38 + pin/32 ) = 0;

  return 0;
}

/**
* @brief   Get serial link file descriptor.
*
* @retval -1   Fail.
* @retval int  File descriptor.
*
*/
int get_serial_link_fd()
{
  return open( SERIAL_DEV, O_RDWR | O_NOCTTY | O_NDELAY);
}
