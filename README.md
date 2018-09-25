# Mini-Shell
An implementation of the Unix Shell in the C Programming language


# How to Run

**1.** Run ```./makefile```

**2.** Run ```./shell```


## Commands Supported

- **mypwd**
  - prints the present working directory 

- **mymkdir**
  - creates a directory 
  - Single directory: ```mymkdir dir1```
  - Multiple directories: ```mymkdir dir1    dir2    dir3```
  - With absolute path: ```mymkdir /home/x/testdir```

- **mycd**
  - changes current working directory to specified directory
  
- **myrm**
  - Removes a file or directory
  - Remove    file: ```myrm file1```
  - Remove    directory: ```myrm dir1``` *removes specified directory only if it is empty*
  - Remove    directory    recursively: ```myrm –r dir1```  *removes specified directory and all its contents*
  
- **mymv**

- **myls**

- **mycat**

- **mytail**

- **myps**

- **myexit**
