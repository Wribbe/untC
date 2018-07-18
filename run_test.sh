#!/bin/sh
BIN_TEST='./bin/test'
ERR_CODE=11
COMMAND_VALG="valgrind --suppressions=suppress.txt --leak-check=full
--error-exitcode=${ERR_CODE}"
# Build the latest files.
make
# Run unit-tests with valgrind.
${COMMAND_VALG} ${BIN_TEST}
# Check exit code.
if [ $? -ne ${ERR_CODE} ]; then
  echo "No leaks."
else
  echo "***** LEAK(S) DETECTED! *****"
fi
