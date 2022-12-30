main: src/main.cpp src/myregex.h
	g++ src/main.cpp src/myregex.h -o main

clean:
	rm main.exe