#include <iostream>
using namespace std;
#include <stdlib.h>
#include <string.h>
#include "file.h"
#include "FileSystem.cpp"

#define DIR		0
#define	CREATE	1
#define	DELETE	2
#define	OPEN	3
#define	CLOSE	4
#define	READ	5	
#define	WRITE	6
#define HELP	7
#define LSEEK	8
#define EXIT	9
#define	OPLIST	10
#define LDISK	11
#define CH_LENGTH	20

int main(){
	printf("��ӭʹ���ļ�ϵͳ��ʹ�ð���������ʾ��\n");
	show_help(); 
	Init();
	create("file1");
	create("file2");
	create("file3");
	open("file1");
	open("file2"); 
	char ch[CH_LENGTH];
	while(gets(ch)){
		int cmd;
		char filename[FILE_NAME_LENGTH];
		//��ʼ��
		cmd = -1;
		Init_block(filename,FILE_NAME_LENGTH);
		if(strncmp("dir",ch,3) == 0){
			cmd = DIR;
		} 
		if(strncmp("create",ch,6) == 0){
			cmd = CREATE;
			strcat(filename,ch + 7);
		}
		if(strncmp("delete",ch,6) == 0){
			cmd = DELETE;
			strcat(filename,ch + 7);
		} 
		if(strncmp("open",ch,4) == 0){
			cmd = OPEN;
			strcat(filename,ch + 5);
		}
		if(strncmp("close",ch,5) == 0){
	        cmd = CLOSE;
        }
        if(strncmp("read",ch,4) == 0){
	        cmd = READ;
        }
        if(strncmp("write",ch,5) == 0){
	        cmd = WRITE;
        }
        if(strncmp("lseek",ch,5) == 0){
	       cmd = LSEEK;
        }
        if(strncmp("oplist",ch,6) == 0){
	       cmd = OPLIST;
        }
        if(strncmp("exit",ch,4) == 0){
	        cmd = EXIT;
	        break;
        }
        if(strncmp("ldisk",ch,5) == 0){
	        cmd = LDISK;
        }
        if(strncmp("help",ch,4) == 0)  {
	        cmd = HELP;
        }
        int index,count,pos;
        switch(cmd){
        	case DIR:
			directory();
			printf("----------------------------------------------\n");
			break;
		case CREATE:
			if(create(filename) == OK)
				printf("�����ļ��ɹ�\n");
			else
			    printf("�д���\n");
			printf("----------------------------------------------\n");
			break;
		case DELETE:
			if(destroy(filename) == OK)
				printf("ɾ���ļ��ɹ�\n");
			printf("----------------------------------------------\n");
			break;
		case OPEN:
			if(open(filename) == OK)
				printf("���ļ��ɹ�\n");
			printf("----------------------------------------------\n");
			break;
		case CLOSE:
			if(show_openlist()==0)
			{
				printf("��ǰû���ļ�����\n");
				printf("----------------------------------------------\n");
				break;
			}
			printf("��������Ҫ�ر��ļ���������\n");
			scanf("%d",&index);
			if(close(index) == OK)
				printf("�رղ����ɹ�\n");
			printf("----------------------------------------------\n");
			break;
		case READ:
			if(show_openlist()==0)
			{
				printf("��ǰû���ļ�����\n");
				printf("----------------------------------------------\n");
				break;
			}
			printf("��������Ҫ�����ļ���������\n");
			scanf("%d",&index);
			printf("��������Ҫ�����ļ��ĳ���\n");
			scanf("%d",&count);
			if(read(index,0,count) == OK)
				printf("���ļ������ɹ�\n");
			printf("----------------------------------------------\n");
			break;
		case WRITE:
			if(show_openlist()==0)
			{
				printf("��ǰû���ļ�����\n");
				printf("----------------------------------------------\n");
				break;
			}
			printf("��������Ҫд���ļ���������\n");
			scanf("%d",&index);
			printf("��������Ҫд���ļ��ĳ���\n");
			scanf("%d",&count);
			if(write(index,0,count) == OK)
				printf("д������ɹ�\n");
			printf("----------------------------------------------\n");
			break;
		case LSEEK:
			if(show_openlist()==0)
			{
				printf("��ǰû���ļ�����\n");
				printf("----------------------------------------------\n");
				break;
			}
			printf("������д���ļ���������\n");
			scanf("%d",&index);
			printf("��������Ҫ���õ��ļ������λ��\n");
			scanf("%d",&pos);
			lseek(index,pos);
			printf("----------------------------------------------\n");
			break;
		case OPLIST:
			if(show_openlist()==0)
			{
				printf("\t\t��ǰû���ļ�����\n");
				printf("----------------------------------------------\n");
				break;
			}
			printf("----------------------------------------------\n");
			break;
		case HELP:
			show_help();
			break;
		case LDISK:
			show_ldisk();
			break;
		default:
			printf("ָ�����\n");
			printf("----------------------------------------------\n");
			break;
		}
		fflush(stdin);
		Init_block(ch,CH_LENGTH);
	}
	return 0;
}
