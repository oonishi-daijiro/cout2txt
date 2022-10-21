main:main.cpp
	g++ main.cpp -o cout2txt.exe
	cout2txt.exe -c dir
child:./child/child.cpp
	g++ ./child/child.cpp -o ./child/child.exe
sample:sample.cpp
	g++ sample.cpp -o smaple.exe
	smaple.exe ./arg.txt
