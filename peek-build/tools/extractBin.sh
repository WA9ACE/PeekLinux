#!/bin/sh
srec_cat $1 -offset -100663296 --byte-swap 2 -Output $2 -Binary
