#!/bin/bash

####################
# Lab 1 Exercise 5
# Name: 
# Student No: 
# Lab Group: 
####################

# Fill the below up
hostname=$(hostname)
machine_hardware=$(uname -m)
max_process_id=$(cat /proc/sys/kernel/pid_max)
user_process_count=$(ps -u $(whoami) | wc -l)
user_with_most_processes=$(ps -eo user= | sort | uniq -c | sort -nr | head -n 1 | awk '{print $2}')
mem_free_percentage=$(free | awk '/^Mem/ { a=($4/$2 * 100); print a}')

echo "Hostname: $hostname"
echo "Machine Hardware: $machine_hardware"
echo "Max Process ID: $max_process_id"
echo "User Processes: $user_process_count"
echo "User With Most Processes: $user_with_most_processes"
echo "Memory Free (%): $mem_free_percentage"
