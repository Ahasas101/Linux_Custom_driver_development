savedcmd_002_test.mod := printf '%s\n'   002_test.o | awk '!x[$$0]++ { print("./"$$0) }' > 002_test.mod
