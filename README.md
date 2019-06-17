# Lab5
## 1.实验简介 
>>本实验要求在假设的I/O系统之上开发一个简单的文件系统，这样做既能让实验者对文件系统有整体了解，又避免了涉及过多细节。用户通过create,open,read等命令与文件系统交互。文件系统把磁盘视为顺序编号的逻辑块序列，逻辑块的编号为0至L−1。I/O系统利用内存中的数组模拟磁盘。  
## 2.实验内容 
### 2.1 I/O系统 
>>I/O系统的任务是隐藏磁盘的结构细节，把磁盘以逻辑块的面目呈现给文件系统。逻辑块顺序编号，编号取值范围为0 至L−1，其中L表示磁盘的存储块总数。 
其中文件系统和I/O系统之间的接口定义如下两个函数:  
>>read_block(int i, char *p);   
>>该函数把逻辑块i的内容读入到指针p指向的内存位置，拷贝的字符个数为存储块的长度B。  
>>write_block(int i, char *p);  
>>该函数把指针p指向的内容写入逻辑块i，拷贝的字符个数为存储块的长度B。 
### 2.2 文件系统 
#### 2.2.1 用户与文件系统之间的接口 
>>文件系统提供如下函数：  
>>create(char* filename)：根据指定的文件名创建新文件。  
>>destroy(char* filename)：删除指定文件   
>>open(char* filename)：打开文件，返回索引号用于后续的read，write，lseek或close操作   
>>close(int index) ：关闭指定文件  
>>read(int index,int mem_area,int count)：从指定文件顺序读入count个字节mem_area指定的内容位置。读操 作从文件的读写指针指示的位置开始。  
>>write(int index,int mem_area,int count)：把mem_area指定的内存位置开始的count个字节顺序写入指定文 件。写操作从文件的读写指针指示的位置开始。  
>>lseek(int index,int pos)：把文件的读写指针移动到pos指定的位置。pos是一个整数，表示从文件开始位置的偏 移量。文件打开时，读写指针自动设置为0。每次读写操作之后，它指向最后被访问的字节的下一个位置。 lseek能够在不进行读写操作的情况下改写读写指针的位置。  
>>directory()：列表显示所有文件及长度。  
#### 2.2.2 文件系统的组织 
>>在本次实验中实现的系统中，磁盘包括两个区域，分别为数据区和保留区，保留区中包括位图区和文件描述符区。数 据区的首部是文件的目录项，即在创建文件时会创建相应的目录。而位图用于表示数据的占用情况。   
## 3.系统测试 
>>在整个系统开始时，首先创建3个空的文件file1，file2以及file3，并且打开file1和file2文件，接着测试文件系统的功能。  
*	文件系统菜单  
![image text](https://github.com/ToTkr2/Lab5/blob/master/img/wj1.png)  
*	创建新文件  
![image text](https://github.com/ToTkr2/Lab5/blob/master/img/wj2.png)  
*	查看目录  
![image text](https://github.com/ToTkr2/Lab5/blob/master/img/wj3.png)  
*	打开新文件  
![image text](https://github.com/ToTkr2/Lab5/blob/master/img/wj4.png)  
*	将内容写到文件中  
![image text](https://github.com/ToTkr2/Lab5/blob/master/img/wj5.png)  
*	关闭指定文件  
![image text](https://github.com/ToTkr2/Lab5/blob/master/img/wj6.png)  
*	删除指定文件  
![image text](https://github.com/ToTkr2/Lab5/blob/master/img/wj7.png)  
*	读取文件内容  
![image text](https://github.com/ToTkr2/Lab5/blob/master/img/wj8.png)  
*	定位读写指针  
![image text](https://github.com/ToTkr2/Lab5/blob/master/img/wj9.png)  
*	查看磁盘数组的内容  
![image text](https://github.com/ToTkr2/Lab5/blob/master/img/wj10.png)
## 4.代码模块
>>代码见上传的code文件中,其中TotalSystem.cpp为主菜单的编写，FileSystem.cpp为主要功能的实现  
