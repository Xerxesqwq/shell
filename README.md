# XERXES' Terminal
### Source
To get the source code, use(coming soon)

```bash
$ git clone https://github.com/Xerxesqwq/terminal
```

### Building From source
You need to have a g++ compile that supports C++11 completely, both 32-bit and 64-bit are OK.
For debian related distributions(including ubutnu), you can use this to install the compile.
Of course, you can use gcc, but some features may be unavailable.

```bash
$ sudo apt install g++
```
For Arch users(If you use Arch Linux most likely you don't have to look at this.)

```bash
$ sudo pacman -S g++
```
Due to many **LINUX** system calls is used in this project, you may fail to build this in Windows Operating System, but you can try wsl.

After cloning and g++ compile is installed, we can begin our build, Then:
```bash
#Change to the source code directory

$ cd xerxes-terminal/src
#run the script to build
$ sh make.sh
```
After wait for seconds, the build will be done.

If there is something wrong with the compile, it must because you have not config zlib correctly, you can try(for ubuntu user)
```bash
$ sudo apt install zlib1g-dev
```

**Notice if you are using gcc compiler, you may not be able to build compress moudle which is optional, so the main project can be built as normal**

### Files Introduction
After the build, we can get 

```txt
── bin
│   ├── back
│   ├── compress
│   ├── delay
│   ├── help.txt
│   └── terminal
├── config
│   └── terminal.dat
├── lib
│   ├── basic.so
│   └── read.so
├── src
│   ├── back.c
│   ├── basic.c
│   ├── basic.h
│   ├── basic.o
│   ├── compress.cpp
│   ├── compress.o
│   ├── make.sh
│   ├── proj.h
│   ├── read.c
│   ├── read.h
│   ├── read.o
│   ├── terminal.c
│   ├── trie.c
│   ├── trie.h
│   └── trie.o
```

`bin` directory is where binary stores binary executables, `back` is used to handle background process to get the return value of it immediately after the background process works done, `compress` is used to comoress the data of the data structure for fewer disk space usage, and `terminal`is the main project, you can use commands below to run the project.
```bash
$ cd bin
$ ./terminal
```
`config` directory have the config files of the project,bacause this is just a ealier version, so we don't have lots of config files to be saved, except `terminal.dat`, the file stores the data of the data structure of our project such as trie(for more efficient insertion and interpolation of string).

`lib` directory stores `.so` files, which means shared object file and are shared library in Linux and is similar with `.dll` in Windows, some module is built to `so` files for main precess(`src/terminal`) to call.`basic.so` provides some basic operation, while `read.so` provides the operation to read, which needs to give the command snip after `Tab Key` is pressed.  

For futher introduction, you can see course-design report.

### Contributing
Bugs should be filled here: noip2018rp@gmail.com, please provide the steps to reproduction in detail.
If you want to provide a patch, you can contact to the e-mail above or the github user https://github.com/Xerxesqwq
