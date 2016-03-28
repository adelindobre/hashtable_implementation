CC=cl
all:tema1.exe
build: hash.obj tema1.obj
	link  /out:tema1.exe hash.obj tema1.obj 
hash.obj: hash.c
	cl /c hash.c
tema1.obj: tema1.c
	cl /c tema1.c
clean:
	del *.exe *.obj
