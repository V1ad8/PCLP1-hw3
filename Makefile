build:
	indent -linux -ts4 -i4 image_editor.c
	gcc -g -Wall -Wextra -std=c99 image_editor.c -o image_editor

clean:
	rm -f image_editor
	
pack:
	zip -FSr 315CA_UngureanuVlad-Marin_Tema3.zip README Makefile *.c