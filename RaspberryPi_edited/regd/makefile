CXX=g++
LD=g++
CFLAGS=-Wall -pedantic -Wno-long-long -O0 -ggdb
BIN_PATH=/usr/sbin/regd
CONF_PATH=/etc/regd

all: install

install:
	mkdir -p $(BIN_PATH)
	g++ $(CFLAGS) src/*.cpp -o $(BIN_PATH)/regd -lpthread
	mkdir -p $(CONF_PATH)
	cp regd.conf $(CONF_PATH)/regd.conf
	cp init.sh /etc/init.d/regd
	chmod +x /etc/init.d/regd

	ln -s ../init.d/regd /etc/rc0.d/K01regd
	ln -s ../init.d/regd /etc/rc1.d/K01regd

	ln -s ../init.d/regd /etc/rc2.d/S95regd
	ln -s ../init.d/regd /etc/rc3.d/S95regd
	ln -s ../init.d/regd /etc/rc4.d/S95regd
	ln -s ../init.d/regd /etc/rc5.d/S95regd

	ln -s ../init.d/regd /etc/rc6.d/K01regd

uninstall:
	rm -f $(BIN_PATH)/regd
	rmdir --ignore-fail-on-non-empty $(BIN_PATH)
	rm -f $(CONF_PATH)/regd.conf
	rmdir --ignore-fail-on-non-empty $(CONF_PATH)

	rm -f /etc/rc0.d/K01regd
	rm -f /etc/rc1.d/K01regd
	rm -f /etc/rc2.d/S95regd
	rm -f /etc/rc3.d/S95regd
	rm -f /etc/rc4.d/S95regd
	rm -f /etc/rc5.d/S95regd
	rm -f /etc/rc6.d/K01regd

	rm -f /etc/init.d/regd

doc:
	doxygen 2>&1 1>/dev/null

clean:
	rm *~
 
