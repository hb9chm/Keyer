echo Are you really sure?
pause
copy keyer.cpp keyer.bak
..\oosmos\Gen\Gen.exe --init keyer
rename keyer.c keyer.cpp
