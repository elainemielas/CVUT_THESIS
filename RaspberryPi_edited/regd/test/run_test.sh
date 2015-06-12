#!/bin/bash

SOCKET=@/tmp/regd
OK="[  \e[92mOK\e[0m  ]"
FAIL="[ \e[91mFAIL\e[0m ]"

NEWLINE="
"

TOTAL=0
SUCCESS=0
function test()
{
	TOTAL=$(( TOTAL + 1 ))

	ret=$( ./test "$SOCKET" "$1$NEWLINE" )

	if [ "$ret" == "$2"  ]; then 
		echo -e "$OK query: '$1'  ret: '$2'"
		SUCCESS=$(( SUCCESS + 1 ))
	else
		echo -e "$FAIL query: '$1'  ret: '$ret' != '$2' "
	fi
}

function read_reg()
{
	./test "$SOCKET" "$1$NEWLINE"
}

#reg list
test '@@' 'ok;0;1;2;3;4;5'

# reg read/write/description/param

test '$0' 'ok;1;1000;0;1;0'
test '#0' 'ok;test0'
test '@W0;0' 'ok'
test '@R0' 'ok;0'

test '$1' 'ok;-2;2000;0;1;0'
test '#1' 'ok;test1'
test '@W1;1' 'ok'
test '@R1' 'ok;1'

test '$2' 'ok;3;-3000;0;1;0'
test '#2' 'ok;test2'
test '@W2;2' 'ok'
test '@R2' 'ok;2'

test '$3' 'ok;4;4000;2;1;0'
test '#3' 'ok;test3'
test '@W3;3' 'ok'
test '@R3' 'ok;3'

test '$4' 'ok;1;1000;-2;1;1'
test '#4' 'ok;test4'

# reg4 READ ONLY

ro=$( read_reg '@R4' )
test '@W4;12345' 'ok'
test '@R4' "$ro"

test '$5' 'ok;5;500;1;-1;1'

#fail test

test '@R44' 'reg missing'
test '@W44;44' 'reg missing'
test '#44' 'reg missing'
test '$44' 'reg missing'

test 'abc' 'syntax error'


echo "test complete: $SUCCESS/$TOTAL success"