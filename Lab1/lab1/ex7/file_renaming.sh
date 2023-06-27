#!/bin/bash

####################
# Lab 1 Exercise 7
# Name:
# Student No:
# Lab Group:
####################

####################
# Strings that you may need for prompt (not in order)
####################
# Enter $N numbers:
# Enter NEW prefix (only alphabets):
# Enter prefix (only alphabets):
# INVALID
# Number of files to create:
# Please enter a valid number [0-9]:
# Please enter a valid prefix [a-z A-Z]:

####################
# Strings that you may need for prompt (not in order)
####################

#################### Steps #################### 

# Fill in the code to request user for the prefix

echo "Enter prefix (only alphabets):"
read prefix

# Check the validity of the prefix #

while true;
do
  if [[ $prefix =~ ^[a-zA-Z]+$ ]]; then
    break
  else
    echo "INVALID"
    echo "Please enter a valid prefix [a-z A-Z]:"
    read prefix
  fi
done

# Enter numbers and create files #

echo "Number of files to create:"
read num_files

while true;
do
  if [[ $num_files =~ ^[0-9]+$ ]]; then
    break
  else
    echo "INVALID"
    echo "Please enter a valid number [0-9]:"
    read num_files
  fi
done

numbers=()
echo "Enter $num_files numbers:"
for (( i=0; i<$num_files; i++ ))
do
  read num_[$i]
  while true;
  do
    if [[ ${num_[$i]} =~ ^[0-9]+$ ]]; then
      break
    else
      echo "INVALID"
      echo "Please enter a valid number [0-9]:"
      read num_[$i]
    fi
  done
  numbers+=($num_[$i])
  # create new file with prefix and number
  touch "${prefix}_${num_[$i]}.txt"
done

echo ""
echo "Files Created"
ls *.txt
echo ""

# Fill in the code to request user for the new prefix

echo "Enter NEW prefix (only alphabets):"
read new_prefix

while true;
do
  if [[ $new_prefix =~ ^[a-zA-Z]+$ ]]; then
    break
  else
    echo "INVALID"
    echo "Please enter a valid prefix [a-z A-Z]:"
    read new_prefix
  fi
done

# Renaming to new prefix #

for i in ${numbers[@]};
do
  mv "${prefix}_${i}.txt" "${new_prefix}_${i}.txt"
done

echo ""
echo "Files Renamed"
ls *.txt
echo ""

exit 0