#!/bin/bash

for client_no in `seq 1 1 $1`
do
	client/client localhost /home/pranav/time.cpp
done
