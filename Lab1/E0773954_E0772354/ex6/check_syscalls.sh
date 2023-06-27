#!/bin/bash

####################
# Lab 1 Exercise 6
# Name: XIAO ZHENG ZE BILL
# Student No: A0239356W
# Lab Group: B07
####################

echo "Printing system call report"

# Compile file
gcc -std=c99 pid_checker.c -o ex6

# Use strace to get report
strace -c ./ex6
