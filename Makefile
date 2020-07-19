# https://github.com/arduino/Arduino/blob/master/build/shared/manpage.adoc

clean:
	rm -rf obj/*

upload:
	/tmp/arduino-1.8.12/arduino -v --upload --pref build.path=/home/coder/project/Documents/CodeSpace/Hydroponics_Arduino/obj --preserve-temp-files Hydroponics_Drivers.ino

verify:
	/tmp/arduino-1.8.12/arduino -v --verify --pref build.path=/home/coder/project/Documents/CodeSpace/Hydroponics_Arduino/obj --preserve-temp-files Hydroponics_Drivers.ino

