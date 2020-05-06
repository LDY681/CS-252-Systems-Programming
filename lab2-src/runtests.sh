#!/bin/bash
#text color scheme
RED='\033[0;31m'
GREEN='\033[0;32m'
DEF='\033[0;00m' # default

PASSED="${GREEN} PASSED ${DEF}"
FAILED="${RED} FAILED ${DEF}"

#test scores
SCORE=0
TOTAL=0

#test function
#parameters in order: test_case_name test_password expected_output #score_value
test_case () {
	#print test case descriptor
	case=$1

	#send test password to test file
	test_psw=$2
	echo $test_psw > ~/cs252/lab2-src/testfile
	
	#get output
	output=$(./pwcheck.sh ~/cs252/lab2-src/testfile) 
	expected_output=$3

	#parse expected output to get score
	echo $expected_output > ~/cs252/lab2-src/out
	score=$(egrep -o [0-9]* ~/cs252/lab2-src/out)
	if [ $score ]; then
		#insert a true expected statement
		expected_output="Password Score: ${score}"
	fi
	case_value=$4

	if [ "$output" == "$expected_output" ]; then
		echo -e $case $PASSED
		SCORE=$((SCORE + case_value))
	else 
		echo -e $case $FAILED
		echo Output:
		echo $output
		echo Expected Output:
		echo $expected_output
		echo
	fi

	TOTAL=$((TOTAL + case_value))
}

################ MAIN ######################################
echo Testing... 
echo

#TEST1
case="Test 1: Password length check"
test_psw="qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq"
expected_output="Error: Password length invalid."
test_case "$case" "$test_psw" "$expected_output" 1
case="Test 1: Password length check 2"
test_psw="abcd5"
expected_output="Error: Password length invalid."
test_case "$case" "$test_psw" "$expected_output" 1

#TEST2
case="Test 2: At least one alpha character"
test_psw="aBcDeF"
expected_output="Expected score: 11"
test_case "$case" "$test_psw" "$expected_output" 2
case="Test 2: At least one alpha character 2"
test_psw="aBcDeFgHiJkLmNoPqRsTuVwXyZaBcDe"
expected_output="Expected score: 36"
test_case "$case" "$test_psw" "$expected_output" 2

#TEST3
case="Test 3: At least one number"
test_psw="aBc9F1"
expected_output="Expected score: 16"
test_case "$case" "$test_psw" "$expected_output" 2
case="Test 3: At least one number 2"
test_psw="aBcDeFg94JkLmNo8qRsTuVwXyZaBcD2"
expected_output="Expected score: 41"
test_case "$case" "$test_psw" "$expected_output" 2

#TEST4
case="Test 4: At least one special char"
test_psw="aBcDeF@"
expected_output="Expected score: 17"
test_case "$case" "$test_psw" "$expected_output" 2
case="Test 4: At least one special char 2"
test_psw="aBcDeF$"
expected_output="Expected score: 17"
test_case "$case" "$test_psw" "$expected_output" 2

#TEST5
case="Test 5: Repeated alphanumeric characters"
test_psw="AbCdEfGG"
expected_output="Expected score: 3"
test_case "$case" "$test_psw" "$expected_output" 2
case="Test 5: Repeated alphanumeric characters 2"
test_psw="aBcDeFgg"
expected_output="Expected score: 3"
test_case "$case" "$test_psw" "$expected_output" 2

#TEST6
case="Test 6: Three consecutive lowercase"
test_psw="aBcDeFgHiJkawfa"
expected_output="Expected score: 17"
test_case "$case" "$test_psw" "$expected_output" 2
case="Test 6: Three consecutive lowercase 2"
test_psw="abcdefghijkl"
expected_output="Expected score: 14"
test_case "$case" "$test_psw" "$expected_output" 2

#TEST7
case="Test 7: Three consecutive uppercase"
test_psw="AbCdEfGhIjKAWFA"
expected_output="Expected score: 17"
test_case "$case" "$test_psw" "$expected_output" 2
case="Test 7: Three consecutive uppercase 2"
test_psw="ABCDEFGHIJKL"
expected_output="Expected score: 14"
test_case "$case" "$test_psw" "$expected_output" 2

#TEST8
case="Test 8: Three consecutive numbers"
test_psw="123456"
expected_output="Expected score: 8"
test_case "$case" "$test_psw" "$expected_output" 2
case="Test 8: Three consecutive numbers 2"
test_psw="ab12345f2456gD986f2e35a6f"
expected_output="Expected score: 32"
test_case "$case" "$test_psw" "$expected_output" 2

#TEST9
case="Test 9: Test if multiple regexes work together correctly"
test_psw="Exon#Mobi@Le21"
expected_output="Expected score: 26"
test_case "$case" "$test_psw" "$expected_output" 5
case="Test 9: Test if multiple regexes work together correctly 2"
test_psw="123456789abcdef@gDWSS@Aw4"
expected_output="Expected score: 21"
test_case "$case" "$test_psw" "$expected_output" 5

echo Tests Finished
echo Total: $SCORE/$TOTAL
############## END OF MAIN ##################################

#clean up
rm testfile
rm out