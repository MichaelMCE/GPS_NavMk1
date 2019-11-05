@echo off

k:
cd K:\code\Arduino\bike\bike

call apath

arduino.exe --verify --board arduino:sam:arduino_due_x --preserve-temp-files bike.ino


rem pause