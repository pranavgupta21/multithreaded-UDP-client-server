#!/bin/bash -v

rm -f *.o
make
if [ "$#" -eq 2 ]
then
	./client $1 $2
elif [ "$#" -eq 1 ]
then
	./client $1
else
	./client localhost
fi
