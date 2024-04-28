build: parent child

parent: main.cc parent.cc
	gcc -o parent $^

child: child.cc
	gcc -o child $^
	