编译: make all
删除编译文件：make clean

build-in command:
	style 1:
		 $<variable name>=<value>
	style 2:
		print <arg> <arg> ...or $<variable name>
		log
		theme <color> -> red blue green initial
		exit

none-build-in command:
	ls 
	pwd
	whoami
	...
		
interactive mode: ./cshell
script mode:./cshell 脚本文件.txt
