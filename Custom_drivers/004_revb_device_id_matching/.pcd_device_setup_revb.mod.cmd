savedcmd_pcd_device_setup_revb.mod := printf '%s\n'   pcd_device_setup_revb.o | awk '!x[$$0]++ { print("./"$$0) }' > pcd_device_setup_revb.mod
