#!/bin/sh
BIN_TEST='./bin/test'
ERR_CODE_LEAK=11
ERR_CODE_SIGSEV=139
COMMAND_VALG="valgrind --suppressions=suppress.txt --leak-check=full
--error-exitcode=${ERR_CODE_LEAK}"
# Build the latest files.
make
[ $? -eq 0 ] || { echo "COMPLIATION ERROR, ABORTING!" && exit; }
# Run unit-tests with valgrind.
${COMMAND_VALG} ${BIN_TEST}
retval=$?
# Check exit code.
case $retval in
  $ERR_CODE_LEAK)
    echo "***** LEAK(S) DETECTED! *****"
    ;;
  $ERR_CODE_SIGSEV)
    echo "***** SEGMENTATION FAULT! *****"
    ;;
  *)
    echo "No errors."
    ;;
esac
