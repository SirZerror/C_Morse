default: morse.o 
	gcc -o morseTest morse.o

morse.o: morse.c
	gcc -c morse.c

run: 
	./morseTest toTranslate.txt -o ThisMorseText.txt
	./morseTest -d ReadMorse.txt -o ThisTextMorse.txt 

clean:
	rm *.o morseTest ThisMorseText.txt ThisTextMorse.txt