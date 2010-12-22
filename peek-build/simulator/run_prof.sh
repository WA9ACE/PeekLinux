#!/bin/sh
cd peek-simulator/dist
valgrind --tool=callgrind --dump-instr=yes --trace-jump=yes --separate-threads=yes ../../simulator
