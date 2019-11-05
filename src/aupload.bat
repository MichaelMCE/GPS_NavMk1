@echo off

k:
cd K:\code\Arduino\bike\bike

call apath

arduino --upload --board arduino:sam:arduino_due_x --port COM30 --preserve-temp-files bike.ino


rem pause --verbose-build 