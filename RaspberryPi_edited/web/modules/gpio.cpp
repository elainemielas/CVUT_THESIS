/**
 * @file   gpio.cpp
 * @Author Bohdan Vico (vicobohd@fit.cvut.cz)
 * @date   November, 2014
 * @brief  gpio module
 *
 */

#include "../lib/controller.hpp"
#include "gpio.c"

using namespace std;

/**
 * @brief   Gpio module class
 *
 */
class gpio : public CController {
public:
    virtual CView* Run()
    {
        if( _SERVER[ "REQUEST_METHOD" ] == "POST" )
        {
            // change gpio function
            if( _POST[ "action" ] == "func" )
                return changeFunc();
            // change gpio pin state 
            else if( _POST[ "action" ] == "value" )
                return changeValue();
        }

        // default pin overview
        return overview();
    }

private:
    /**
     * @brief   Gpio overview
     *
     * Get gpio pin function and state.
     *
     * @retval CView*   Template view
     *
     */
    CView* overview()
    {
        CArray &data = *( new CArray() );
        
        // refresh page every 10 sec.
        header->AddItem( "Refresh: 10;" );
        // get all gpio state
        for( int i = 0; i < 54; i++ )
        {
            data[ "pin" ][ i ][ "func" ] = gpio_func( i );
            data[ "pin" ][ i ][ "value" ] = gpio_read( i );
        }

        if( data[ "pin" ][ 0 ][ "func" ] == -1 )
            data[ "error" ] = "Error while reading gpio pins, are you root?";

        return new CView( "gpio/overview.view", &data );   
    }

    /**
     * @brief   Gpio change function
     *
     * Change gpio pin function.
     *
     * @retval CView*   Template view
     *
     */
    CView* changeFunc()
    {
        int pin, func;

        sscanf( _POST[ "pin" ].getstr(), "%d", &pin );
        sscanf( _POST[ "func" ].getstr(), "%d", &func );

        if( pin >= 0 && pin < 54 && func >= 0 && func <= 7 )
            gpio_init( pin, func );

        return overview();
    }

    /**
     * @brief   Gpio change state
     *
     * Change gpio pin state.
     *
     * @retval CView*   Template view
     *
     */
    CView* changeValue()
    {
        int pin, value = 0;

        sscanf( _POST[ "pin" ].getstr(), "%d", &pin );

        if( _POST[ "value" ] == "on" )
            value = 1;        

        if( pin >= 0 && pin < 54 )
            gpio_write( pin, value );

        return overview();
    }

};

// the class factories

extern "C" CController* create()
{
    return new gpio;
}

extern "C" void destroy( CController* p )
{
    delete p;
}
