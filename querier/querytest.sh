#!/bin/bash
# querytest.sh -- tests for the querier (Module 6, Step 5)
# usage: querytest.sh <pageDirectory> <indexFile>
#
# Runs every test case listed on the lab sheet. Argument tests should each
# print an error or the usage statement and an exit code of 1. Each query
# test prints the query (tabs shown as <TAB>) followed by the querier's output.
 
if [ $# -ne 2 ]; then
	echo "usage: querytest.sh <pageDirectory> <indexFile>"
	exit 1
fi
 
PAGES=$1
INDEX=$2
Q=./query-5
 
# run the querier with no input, so a broken test can never hang
runargs() {
	"$Q" "$@" < /dev/null
	echo "   exit code: $?"
}
 
echo "=================== argument tests ==================="
echo "--- no arguments";               runargs
echo "--- one argument";               runargs "$PAGES"
echo "--- too many arguments";         runargs "$PAGES" "$INDEX" -q extra
echo "--- third argument is not -q";   runargs "$PAGES" "$INDEX" -x
echo "--- directory that does not exist"; runargs nosuchdirectory "$INDEX"
 
EMPTYDIR=$(mktemp -d)
echo "--- directory that has not been crawled"; runargs "$EMPTYDIR" "$INDEX"
 
NOACCESS=$(mktemp -d)
chmod 000 "$NOACCESS"
echo "--- directory that is not accessible";   runargs "$NOACCESS" "$INDEX"
 
NOREAD=$(mktemp)
cp "$INDEX" "$NOREAD"
chmod 000 "$NOREAD"
echo "--- index file that is not readable";    runargs "$PAGES" "$NOREAD"
echo "--- index file that does not exist";     runargs "$PAGES" nosuchindexfile
 
chmod 700 "$NOACCESS"
rm -rf "$EMPTYDIR" "$NOACCESS" "$NOREAD"
 
echo
echo "=================== stdin tests ==================="
echo "--- empty input (should print nothing)"
"$Q" "$PAGES" "$INDEX" -q < /dev/null
echo "   exit code: $?"
echo "--- tab then EOF (should print nothing)"
printf '\t' | "$Q" "$PAGES" "$INDEX" -q
echo "--- enter then EOF (should print nothing)"
printf '\n' | "$Q" "$PAGES" "$INDEX" -q
echo "--- tab, enter, then EOF (should print nothing)"
printf '\t\n' | "$Q" "$PAGES" "$INDEX" -q
 
# one query per entry; $'...' lets \t stand for a tab
QUERIES=(
	"dartmouth college"                        # basic two-word query
	"Dartmouth AND College"                    # same as above
	"xyzzyplugh"                               # a single word that does not exist
	"dartmouth or xyzzyplugh"                  # valid word or'd with an invalid word
	"dartmouth and xyzzyplugh"                 # valid word and'd with an invalid word
	$'the\tcourse and  notes or reading\tschedule'  # mix of and, or, spaces, tabs
	"and dartmouth"                            # and at the beginning
	"or dartmouth"                             # or at the beginning
	$'\tand dartmouth'                         # tab then and at the beginning
	"dartmouth and"                            # and at the end
	"dartmouth or"                             # or at the end
	$'dartmouth or\t'                          # or then tab at the end
	"dartmouth and or college"                 # adjacent and/or in the middle
	"dartmouth or and college"                 # adjacent or/and in the middle
	"dartmouth and and college"                # adjacent and/and in the middle
	"and"                                      # an operator alone
	"engineering 50"                           # a number
	"Dartmouth!"                               # punctuation
	"xyzzyplugh and plughxyzzy"                # no scores returned
)
 
echo
echo "=================== query tests ==================="
for query in "${QUERIES[@]}"; do
	echo "--- query: [${query//$'\t'/<TAB>}]"
	printf '%s\n' "$query" | "$Q" "$PAGES" "$INDEX" -q
done
