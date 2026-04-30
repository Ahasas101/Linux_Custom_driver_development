savedcmd_test.mod := printf '%s\n'   test.o | awk '!x[$$0]++ { print("./"$$0) }' > test.mod
