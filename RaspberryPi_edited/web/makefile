CXX=g++
LD=g++
CFLAGS=-Wall -pedantic -Wno-long-long -O0 -ggdb
DIR=/var/www

all: dir libmenu.so libheader.so libarray.so libview.so index.cgi status.so gpio.so registers.so

dir:
	mkdir -p "$(DIR)"
	mkdir -p "$(DIR)/view"
	mkdir -p "$(DIR)/lib"
	mkdir -p "$(DIR)/modules"

index.cgi: index.cpp lib/controller.hpp lib/menu.hpp lib/header.hpp lib/array.hpp lib/view.hpp
	$(CXX) $(CFLAGS) index.cpp -ldl -fPIC -lview -lmenu -larray -lheader -L$(DIR)/lib -Wl,-rpath=./lib -o "$(DIR)/index.cgi"
	cp -r html/css html/img "$(DIR)/"
	cp html/view/main.view "$(DIR)/view/"

libmenu.so: lib/menu.hpp lib/menu.cpp
	$(CXX) $(CFLAGS) -shared -fPIC lib/menu.cpp -o $(DIR)/lib/libmenu.so

libheader.so: lib/header.hpp lib/header.cpp
	$(CXX) $(CFLAGS) -shared -fPIC lib/header.cpp -o $(DIR)/lib/libheader.so

libview.so: lib/view.hpp lib/view.cpp lib/lexer.hpp lib/lexer.cpp lib/parser.hpp lib/parser.cpp lib/tree.hpp lib/tree.cpp lib/array.hpp
	$(CXX) $(CFLAGS) -shared -fPIC lib/view.cpp lib/lexer.cpp lib/parser.cpp lib/tree.cpp -o $(DIR)/lib/libview.so

libarray.so: lib/array.hpp lib/array.cpp lib/parser.o lib/lexer.o
	$(CXX) $(CFLAGS) -shared -fPIC lib/array.cpp -o $(DIR)/lib/libarray.so

status.so: modules/status.cpp
	$(CXX) $(CFLAGS) -shared -fPIC modules/status.cpp -o $(DIR)/modules/status.so
	cp -r html/view/status "$(DIR)/view/"

gpio.so: modules/gpio.cpp
	$(CXX) $(CFLAGS) -shared -fPIC modules/gpio.cpp -o $(DIR)/modules/gpio.so
	cp -r html/view/gpio "$(DIR)/view/"

registers.so: modules/registers.cpp
	$(CXX) $(CFLAGS) -shared -fPIC modules/registers.cpp -o $(DIR)/modules/registers.so
	cp -r html/view/registers "$(DIR)/view/"
