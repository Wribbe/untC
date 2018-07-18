#!/bin/sh
BIN_TEST='./bin/test'
ERR_CODE=11
ERR_CODE_SIGSEV=139
FILE_LOG=test.log
COMMAND_VALG="valgrind --suppressions=suppress.txt --leak-check=full
--error-exitcode=${ERR_CODE}"
# Build the latest files.
make
[ $? -eq 0 ] || { echo "COMPLIATION ERROR, ABORTING!" && exit; }
# Run unit-tests with valgrind.
output=$(((((${COMMAND_VALG} ${BIN_TEST} 2>&1; echo $? >&3) | \
  tee /dev/tty >&4) 3>&1) | (read xs; exit $xs)) 4>&1)
retval=$?
echo "$output" > $FILE_LOG
# Check exit code.
case $retval in
  $ERR_CODE)
    echo "***** ERRORS DETECTED! *****"
    ;;
  $ERR_CODE_SIGSEV)
    echo "***** SEGMENTATION FAULT! *****"
    ;;
  *)
    echo "No errors detected by valgrind."
    rm $FILE_LOG
    ;;
esac
