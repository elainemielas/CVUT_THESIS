#ifndef CONTROLLER
#define CONTROLLER

#include "menu.hpp"
#include "header.hpp"
#include "view.hpp"
#include "array.hpp"

class CController 
{
	public:
		CController();
		virtual ~CController();
	    virtual CView* Run() { return new CView( NULL, new CArray() ); }

	    CHeader *header;
		CMenu   *menu;
	protected:
		CArray  _SERVER;
		CArray  _GET;
		CArray  _POST;

	private:
		void fill_SERVER();
		void fill_POST();
		void fill_GET();
};

// the types of the class factories
typedef CController* create_t();
typedef void destroy_t( CController* );

#include "controller.cpp"

#endif