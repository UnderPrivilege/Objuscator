# For use with NMAKE

hello.exe: hide.obj
	link $**

hide.obj: hide.c
	$(CC)  /c hide.c
	obfuscator.exe hide.obj

obfuscator.exe: ../src/obfuscator.c
	$(CC) $**
	del $*.obj

clean:
	del *.exe
	del *.obj
