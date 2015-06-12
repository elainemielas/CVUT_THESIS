#!/bin/bash

PIDFILE=/var/run/regd.pid
EXECFILE=regd
CONFFILE=/etc/regd/regd.conf
REGD_RUN_DIR=/usr/sbin/regd

export REGD_RUN_DIR

function start()
{
	[ -f "$PIDFILE" -a -s "$PIDFILE" ] && { echo -e "\e[31m * \e[0mThere exist pid file, seems program is running"; return; }

	$REGD_RUN_DIR/$EXECFILE -d "$CONFFILE"
	sleep 1
	
	[ -f "$PIDFILE" ] || echo -e "\e[31m * \e[0mProccess may not run, pidfile doesn't exist!"
}

function stop()
{
	[ -f "$PIDFILE" -a -s "$PIDFILE" ] || { echo -e "\e[31m * \e[0mProgram not run, pidfile doesn't exist!"; return; }

	kill -15 $( cat "$PIDFILE" )
	sleep 1
	[ -f "$PIDFILE" ] && echo -e "\e[31m * \e[0mProgram may run, pidfile exist!"
}

function restart()
{
	stop
	start
}

function status()
{
	if [ -f "$PIDFILE" -a -s "$PIDFILE" ]
	then
		echo -e "\e[92m * \e[0mRegisterd is running"
	else
		echo -e "\e[31m * \e[0mRegisterd is not running"
	fi
}

case "$1" in
	start) 
		echo " * Starting register daemon"
		start
		;;
	stop)
		echo " * Stopping register daemon"
		stop
		;;
	restart)
		echo " * Restarting register daemon"
		restart
		;;
	status)
		status
		;;
	*)
		echo "Usage: $0 {start|stop|restart|status}"
		exit 1
		;;
esac

exit 0