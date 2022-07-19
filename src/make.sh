echo 'compiling..., please wait'
gcc -c -Ofast basic.c -o basic.o
echo '10% complete'
gcc -shared basic.o -o ../lib/basic.so
echo '20% complete'
gcc -c -Ofast trie.c -o trie.o
echo '30% complete'
gcc -c read.c -o read.o
echo '40% complete'
gcc -shared read.o -o ../lib/read.so
echo '50% complete'
gcc back.c -Ofast -o ../bin/back
cp ../bin/back ../test
echo '60% complete'
g++ -Ofast compress.cpp -lz -o ../bin/compress
echo '70% complete'
gcc terminal.c trie.o ../lib/basic.so ../lib/read.so -lm -lz -Ofast -lpthread -o ../bin/terminal
echo '100% complete'
echo 'make done!'
