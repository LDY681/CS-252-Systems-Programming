#!/bin/bash

#DO NOT REMOVE THE FOLLOWING TWO LINES
git add $0 >> .local.git.out
git commit -q -a -m "Lab 2 commit" >> .local.git.out
git push -q >> .local.git.out || echo

#My code begins here
FILE="$1"
COUNT=0
#Read text from file and save as password
read -r PASSWORD<"$1"

#Password Checker

#Password has less than 6, or more than 32 characters
if [ ${#PASSWORD} -gt 32 ] || [ ${#PASSWORD} -lt 6 ] ; then
	echo "Error: Password length invalid."
else
	#For any valid password:
	let COUNT=${#PASSWORD}

	#If the password contains one of the following special characters (#$+%@)
	if egrep -q "[#$+%@]" $FILE; then
		let COUNT=$COUNT+5
	fi

	#If the password contains at least one number (0-9)
	if egrep -q "[0-9]" $FILE; then
		let COUNT=$COUNT+5
	fi

	#If the password contains at least one alpha character (A-Za-z)
	if egrep -q "[A-Za-z]" $FILE; then
		let COUNT=$COUNT+5
	fi

	#If the password contains a repeated alphanumeric character (i.e. aa, bbb, 55555)
	if egrep -q "([A-Za-z0-9])\1+" $FILE; then
		let COUNT=$COUNT-10
	fi

	#If the password contains 3 or more consecutive lowercase characters (i.e. bbb, abe, this)
	if egrep -q "[a-z]{3,}" $FILE; then
		let COUNT=$COUNT-3
	fi

	#If the password contains 3 or more consecutive uppercase characters (i.e. BBB, XQR, APPLE)
	if egrep -q "[A-Z]{3,}" $FILE; then
		let COUNT=$COUNT-3
	fi

	#If the password contains 3 or more consecutive numbers (i.e. 55555, 1747, 123, 321)
	if egrep -q "[0-9]{3,}" $FILE; then
		let COUNT=$COUNT-3
	fi
	echo "Password Score: $COUNT"
fi