#include <iostream>
#include <malloc.h>
#include <string.h>
#include "file.h"
using namespace std;


//I/Oϵͳ

//���߼���i�����ݶ��뵽ָ��pָ����ڴ�λ�ã��������ַ�����Ϊ�洢�鳤��B
void read_block(int i, char *p){
	char * temp = (char *)malloc(sizeof(char));  //����һ����̬�����洢p�е����� 
	temp = p;
	// ���뵽pָ����λ�� 
	for(int j = 0;j < B;j++){
		*temp = ldisk[i][j];
		temp++;
	}
} 

//��ָ��pָ�������д���߼���i���������ַ�����Ϊ�洢��ĳ���B
void write_block(int i, char *p){
	char* temp = (char*)malloc(sizeof(char));
	temp = p;
	//��p������д���߼����� 
	for(int j = 0;j < B;j++){
		ldisk[i][j] = *temp;
		temp++;
	}
} 

//��ʼ��һ���ַ�����鴦����ַ�����鳤��ΪB������Ϊ'\0'
void Init_block(char *temp,int length){
	int i;
	for(i = 0;i < length;i++){
		temp[i] = '\0';
	} 
} 

//������д�뻺��������뵽�ڴ��� 
int write_buffer(int index,int list)
{

	int i;
	int j;
	int freed;
	char temp[B];

	int buffer_length = BUFFER_LENGTH;
	for(i = 0; i < BUFFER_LENGTH ; i ++)
	{
		if(open_list[list].buffer[i] == '\0')
		{
			buffer_length = i;					//��������Ч���� 
			break;
		}
	}
	
	int x = open_list[list].pointer[0];
	int y = open_list[list].pointer[1];
	int z = B-y;									//��ǰ��������� 

	if( buffer_length < z )					//��������д�뻺��������Ҫ�ٷ���
	{	
		read_block(x,temp);
		strncat(temp+y,open_list[list].buffer,buffer_length);			//���������� 
		write_block(x,temp);
		
		read_block(index+FILE_SIGN_AREA,temp);	//�����ļ����� 
		temp[1] += buffer_length;	
		write_block(index+FILE_SIGN_AREA,temp);
		
		open_list[list].pointer[0] = x;
		open_list[list].pointer[1] = y + buffer_length ;	//�����ļ���дָ�� 
	}
	else									//���ڣ���Ҫ�����¿� 
	{
		read_block(index+FILE_SIGN_AREA,temp);
		if(temp[2]+(buffer_length - z)/B+1 > FILE_BLOCK_LENGTH)
		{
			printf("�ļ��������鲻������\n");
			return ERROR;
		}

		//��Ϊ������
		//��һ���֣����� 
		read_block(x,temp);
		strncat(temp+y,open_list[list].buffer,z);			//����������ĳ��ȣ�������ǰ�� 
		write_block(x,temp);

		//�ڶ����ֻ���Ҫ���� (buffer_length - z)/B+1��

		//******************Ѱ���ļ�����Ŀ¼��֮�󣩵Ŀ��п飬�����¿� 
		for(i = 0; i < (buffer_length - z)/B ; i ++ )
		{
			for(j = K + FILE_NUM; j < L ; j++)
			{
				read_block((j-K)/B,temp);
				if(temp[(j-K)%B] == FREE)
				{
					freed = j;
					break;
				}
			}
			if(j == L)
			{
				printf("��������������ʧ��\n");
				return ERROR;
			}
			
			Init_block(temp,B);						
			strncpy(temp,(open_list[list].buffer+z+(i*B)),B);
			write_block(freed,temp);				//д������ 

			read_block((freed-K)/B,temp);			//����λͼ״̬ 
			temp[(freed-K)%B] = BUSY;
			write_block((freed-K)/B,temp);			//д����̣�λͼ(�ļ���������Ӧ��)

			read_block(index+FILE_SIGN_AREA,temp);
			temp[2] ++;								//�����ļ��������鳤�� 
			temp[2+temp[2]] = freed;
			write_block(index+FILE_SIGN_AREA,temp);	
		}

		//(buffer_length - z)%B ����һ����β���Ĳ��� 
		for(j = K + FILE_NUM; j < L ; j++)
		{
			read_block((j-K)/B,temp);
			if(temp[(j-K)%B] == FREE)
			{
				freed = j;
				break;
			}
		}
		if(j == L)
		{
			printf("��������������ʧ��\n");
			return ERROR;
		}
		Init_block(temp,B);	
		strncpy(temp,(open_list[list].buffer+z+(i*B)),(buffer_length - z)%B);
		write_block(freed,temp);				//д������ 

		read_block((freed-K)/B,temp);			//����λͼ״̬ 
		temp[(freed-K)%B] = BUSY;
		write_block((freed-K)/B,temp);			//д����̣�λͼ(�ļ���������Ӧ��)

		read_block(index+FILE_SIGN_AREA,temp);
		temp[2] ++;								//�����ļ��������鳤�� 
		temp[2+temp[2]] = freed;
		write_block(index+FILE_SIGN_AREA,temp);


		read_block(index+FILE_SIGN_AREA,temp);	
		temp[1] += buffer_length;				//�����ļ����� 
		write_block(index+FILE_SIGN_AREA,temp);

		open_list[list].pointer[0] = freed;
		open_list[list].pointer[1] = (buffer_length - z)%B ;
	}	
}


//�ļ�ϵͳ
//��ʼ������

//���ļ��Ķ�дָ���ƶ���posָ����λ��
int lseek(int index,int pos){
	int i;
	int list = -1;   //��ʾ��ʼʱ������λ��Ϊ-1 
	char temp[B];
	int pos_i = pos / B;   //�������ldisk�еĿ��� 
	int pos_j = pos % B;   //������ڵ�i���е���ʼλ��
	for(i = 0;i < FILE_NUM;i++){
		if(open_list[i].filesignnum == index){
			list = i;
			break;
		}
	}
	if(list == -1){
		printf("δ�ҵ�λ�ã�����ʧ��\n");
		return ERROR;
	} 
	if(open_list[list].flag != BUSY){
		printf("������������󣬲���ʧ��\n");
		return ERROR;
	}
	//���ݱ�Ŀ�е�index���ļ�ɨ������ҵ���������
	read_block(open_list[list].filesignnum + FILE_SIGN_AREA,temp);
	if(pos_i > temp[2] - 1){   //�������ļ�ʵ�ʵĿ���ʱ 
		printf("�쳣Խ�磬��λʧ��\n");
		return ERROR; 
	} 
	
	//��λָ��
	open_list[list].pointer[0] = temp[3 + pos_i];  //���ļ��ĵ�1���ʵ�ʴ��̵�ַ
	open_list[list].pointer[1] = pos_j;
	return OK; 
}

void Init(){
	int i;
	char temp[B];
	for(i = 0;i < L;i++){
		Init_block(temp,B);
		write_block(i,temp);  //��ʼ������ 
	}
	for(i = K;i < L;i++){  //��ʼ����������λͼ 
		read_block((i - K) / B,temp);
		temp[(i - K) % B] = FREE;
		write_block((i - K) / B,temp); 
	} 
	//��ʼ��Ŀ¼���ļ���ʶ��
	filesign temp_cnt_sign;
	temp_cnt_sign.filesign_flag = 1;
	temp_cnt_sign.file_length = 0;   //�ļ����ȳ�ʼΪ0 
	
	temp_cnt_sign.file_block = FILE_BLOCK_LENGTH;  //����FILE_BLOCK_LENGTH*B�Ŀռ����ڴ���Ŀ¼��
	
	Init_block(temp,B);
	temp[0] = temp_cnt_sign.filesign_flag;
	temp[1] = temp_cnt_sign.file_length;
	temp[2] = temp_cnt_sign.file_block;
	
	for(i = 0;i < FILE_BLOCK_LENGTH;i++){
		temp[i+3] = K + i;   //Ĭ����������ǰFILE_BLOCK_LENGTH����Ŀ¼ռ�� 
	}
	write_block(FILE_SIGN_AREA,temp);
	
	//����λͼ��״̬
	read_block(0,temp); 
	for(i = 0; i < FILE_NUM ; i++ )
	{
		temp[i] = FREE;						//��ʱδ�����ļ� 
	}
	write_block(0,temp);

}
 
//����ָ�����ļ����������ļ�
int create(char filename[]){
	int frees;   //���е��ļ�������λ��
	int freed;   //����������λ�ã�Ŀ¼�
	int freed2;
	char temps[B];
	char tempc[B];
	char temp[B];
	int i;
	//�鿴�ļ����Ƿ����
	for(i = K;i < K + FILE_NUM;i++){
		read_block((i - K) / B,temp);
		if(temp[(i - K) % B] == BUSY){
			read_block(i,temp);
			if(strncmp(temp + 1,filename,FILE_NAME_LENGTH) == 0){
				printf("��Ŀ¼�Ѵ����ļ���Ϊ%s���ļ�\n",filename);
				return ERROR;
			}
		}
	}
	//Ѱ�ұ������п��е��ļ�������
	for(i = FILE_SIGN_AREA;i < K;i++){
		read_block(i,temp);
		if(temp[0] == FREE){
			frees = i;
			break;
		}
	} 
	if(i == K){
		printf("û�п��е��ļ�������\n");
		return ERROR;
	}
	//Ѱ��������Ŀ¼��������ָ���Ŀ��еĴ洢��
	for(i = K;i < K + FILE_NUM;i++){
		read_block((i-K)/B,temp);						
		if(temp[(i-K)%B] == FREE)
		{
			freed = i;
			break;
		}
	} 
	if(i == K + FILE_NUM){
		printf("�ļ������Ѵ�����\n");
		return ERROR;
	}
	//Ѱ���ļ����Ŀ��п������´������ļ�
	for(i = K + FILE_NUM;i < L;i++){
		read_block((i-K)/B,temp);
		if(temp[(i-K)%B] == FREE)
		{
			freed2 = i;
			break;
		}
	}
	if(i == L)
	{
		printf("��������������ʧ��\n");
		return ERROR;
	}
	//�������ʼ����
	filesign temp_filesign;   //������ʱ�ļ�������
	contents temp_contents;   //������ʱĿ¼��
	//�����ļ�������
	temp_filesign.filesign_flag = 1;
	temp_filesign.file_length = 0;
	temp_filesign.file_block = 1; 
	
	Init_block(temp,B);
	temps[0] = temp_filesign.filesign_flag;
	temps[1] = temp_filesign.file_length;
	temps[2] = temp_filesign.file_block;
	temps[3] = freed2;
	
	for(i = 4;i < FILE_BLOCK_LENGTH;i++){
		temps[i] = '\0'; 
	}
	write_block(frees,temps);  //д������ļ�������
	
	//����Ŀ¼�����Ŀ¼�ļ���������ָ��������� 
	temp_contents.filesignnum = frees - FILE_SIGN_AREA;
	strncpy(temp_contents.filename,filename,FILE_NAME_LENGTH);
	
	Init_block(tempc,B);
	tempc[0] = temp_contents.filesignnum;   
	tempc[1] = '\0';
	strcat(tempc,temp_contents.filename);
	write_block(freed,tempc);	 //д�����������
	
	//����λͼ״̬
	read_block((freed-K)/B,temp);  //����λͼ״̬��Ŀ¼������Ӧ�ģ�
	temp[(freed-K)%B] = BUSY;
	write_block((freed-K)/B,temp); //д�����λͼ
	read_block((freed2-K)/B,temp);  //����λͼ״̬
	temp[(freed2-K)%B] = BUSY;
	write_block((freed2-K)/B,temp);  //д�����λͼ���ļ���������Ӧ��
	
	//����Ŀ¼�ļ��������еĳ�����
	read_block(FILE_SIGN_AREA,temp);
	temp[1]++;
	write_block(FILE_SIGN_AREA,temp);
	printf("�����ļ��ɹ�\n"); 
	return OK; 
} 

//ɾ��ָ���ļ�
int destroy(char* filename){
	int i;
	int dtys;  //��Ҫɾ�����ļ���Ŀ¼����ļ�������λ��
	int dtyd;  //��Ҫɾ�����ļ���Ŀ¼��λ��
	int use_block;  //���ļ�ʵ��ʹ�õĿ���
	int index;
	char temp[B];
	char tempd[B];
	//�����ļ���Ѱ���ļ���Ŀ¼����ļ�������
	for(i = K;i < K + FILE_NUM;i++){
		read_block((i-K)/B,temp);
		if(temp[(i-K)%B] == BUSY){
			read_block(i,temp);
			if(strncmp(temp+1,filename,FILE_NAME_LENGTH) == 0){
				dtyd = i;  //�ҵ��ļ�Ŀ¼��λ�� 
				dtys = temp[0] + FILE_SIGN_AREA;  //�ҵ��ļ�Ŀ¼���Ӧ���ļ���������λ��
				index = temp[0];
				break; 
			}
		} 
	}
	if(i == K + FILE_NUM){
		printf("û���ҵ����ļ�\n");
		return ERROR;
	} 
	
	//�����ļ���������index�鿴���ļ��Ƿ��
	int list = -1;
	for(i = 0;i < FILE_NUM;i++){
		if(open_list[i].filesignnum == index){
			list = i;
			break;
		} 
	} 
	if(open_list[list].flag == BUSY && list != -1){
		printf("���ļ��Ѿ����򿪣���Ҫ�رղ���ɾ��\n");
		return ERROR;
	}
	
	//�ҵ�λ�ú�ʼ����
	//���θ����ļ���������ָ���Ŀ��λͼ��
	read_block(dtys,temp);
	use_block = temp[2];
	for(i = 0;i < use_block;i++){
		read_block((temp[i+3]-K)/B,tempd);				
		tempd[(temp[i+3]-K)%B] = FREE;
		write_block((temp[i+3]-K)/B,tempd);
	} 
	
	//ɾ����Ŀ¼��
	Init_block(temp,B);
	write_block(dtys,temp);
	
	//ɾ���ļ�������
	Init_block(temp,B);
	write_block(dtyd,temp); 
	
	//����λͼ��
	read_block((dtyd-K)/B,temp);
	temp[(dtyd-K)%B] = FREE;
	write_block((dtyd-K)/B,temp);
	
	//Ŀ¼�ļ��������еĳ��ȼ�1
	read_block(FILE_SIGN_AREA,temp);
	temp[1]--;
	write_block(FILE_SIGN_AREA,temp);
	
	return OK; 
} 

//���ļ� ,���ص������ſ����ں�����read��write��lseek��close����
int open(char *filename){
	int i;
	int opd;
	int ops;
	int list;
	char temp[B];
	int index;
	//�����ļ���Ѱ���ļ���Ŀ¼����ļ�������
	for(i = K;i < K + FILE_NUM;i++){
		read_block((i-K)/B,temp);
		if(temp[(i-K)%B] == BUSY){
			read_block(i,temp);
			if(strncmp(temp+1,filename,FILE_NAME_LENGTH) == 0){
				opd = i;  //�ҵ��ļ�Ŀ¼����ְ
				ops = temp[0];  //�ҵ��ļ�Ŀ¼���Ӧ���ļ����������
				break; 
			}
		}
	}
	if(i == K + FILE_NUM){
		printf("�ļ�������\n");
		return ERROR;
	} 
	
	//�鿴���ļ��Ƿ񱻴�
	for(i = 0;i < FILE_NUM;i++){
		if(open_list[i].filesignnum == ops && open_list[i].flag == BUSY){
			printf("���ļ��Ѿ�����\n");
			return ERROR;
		}
	} 
	
	//��һ��û��ʹ�õı�Ŀ�Ա����
	for(i = 0;i < FILE_NUM;i++){
		if(open_list[i].flag != BUSY){
			list = i;
			break;
		}
	} 
	
	//�Ա�Ŀ���в���
	open_list[list].filesignnum = ops;  //д���ļ����������
	open_list[list].flag = BUSY;  //�ñ�־λΪռ��
	index = open_list[list].filesignnum;  //����������
	lseek(index,0);   //ָ��ָ���ļ��ײ� 
	Init_block(open_list[list].buffer,BUFFER_LENGTH);  //��ʼ��������
	read_block(open_list[list].pointer[0],temp);  //���ļ����׿�
	strncpy(open_list[list].buffer,temp,BUFFER_LENGTH);  //���ļ��ĵ�һ��д�뻺����
	
	return OK; 
} 

//�ر�ָ���ļ�
int close(int index){
	int i;
	int list = -1;
	char temp[B];
	//����index�ұ�Ŀ
	for(i = 0;i < FILE_NUM;i++){
		if(open_list[i].filesignnum == index){
			list = i;
			break;
		}
	} 
	if(list == -1){
		printf("�ļ������ڣ�����ʧ��\n");
		return ERROR;
	}
	if(open_list[list].flag != BUSY){
		printf("Ҫ�رյ��ļ�δ�򿪣�����ʧ��\n");
		return ERROR;
	}
	
	//��������������д�����
	write_buffer(index,list);  //����ǰlist���ļ����Ӧ�Ļ�����index�����ŵ��ļ�
	
	//��������ͷű�Ŀ
	Init_block(open_list[list].buffer,BUFFER_LENGTH);  //���������
	open_list[list].filesignnum = FREE;   //����ļ�������
	open_list[list].flag = FREE;  //���ռ�ñ�־λ
	open_list[list].pointer[0] = NULL;
	open_list[list].pointer[1] = NULL;   //���ָ��
	return OK; 
} 

//��ָ���ļ�˳�����count���ֽ�mem_areaָ�����ڴ�λ�á�
//���������ļ��Ķ�дָ��ָʾ��λ�ÿ�ʼ
int read(int index,int mem_area,int count){
	int i;
	int list = -1;
	char temp[B];
	//���������ҵ����ļ���Ŀ
	for(i = 0;i < FILE_NUM;i++){
		if(open_list[i].filesignnum == index){
			list = i;
			break;
		}
	} 
	if(list == -1){
		printf("δ�ҵ���ǰ�������ļ�������ʧ��\n");
		return ERROR; 
	}
	if(open_list[list].flag != BUSY){
		printf("��ǰ�ļ�δ�򿪣�����ʧ��\n");
		return ERROR;
	}
	
	//����index���ļ�������
	char temp_output[OUTPUT_LENGTH];
	Init_block(temp_output,OUTPUT_LENGTH);
	char output[OUTPUT_LENGTH];
	Init_block(output,OUTPUT_LENGTH);
	
	read_block(FILE_SIGN_AREA + index,temp);
	int file_length = temp[1];  //�ļ�����
	int file_block = temp[2];  //�ļ�ʵ��ʹ�ÿ�
	int file_area;
	
	//�����ļ����ݵ�temp_output
	for(i = 0;i < file_block - 1;i++){
		read_block(FILE_SIGN_AREA + index,temp);
		read_block(temp[3+i],temp);
		strncpy(temp_output+i*B,temp,B);
	} 
	read_block(FILE_SIGN_AREA+index,temp);
	read_block(temp[3+i],temp);
	strncpy(temp_output+i*B,temp,B);
	
	//��ǰ�ļ���д����
	int x = open_list[list].pointer[0];
	int y = open_list[list].pointer[1];
	
	for(i = 0;i < file_block;i++){
		read_block(FILE_SIGN_AREA+index,temp);
		if(temp[3+i] == x){
			break;
		}
	} 
	file_area = i * B + y;   //ת��Ϊ�ļ��������λ��
	for(i = 0 ;  i < count ; i++){
		output[i+mem_area] = temp_output[i+file_area]; 
	}
	printf("%s\n",output+mem_area);
	return OK;

} 

//��mem_areaָ�����ڴ�λ�ÿ�ʼ��count���ֽ�˳��д��ָ���ļ�
//д�������ļ��Ķ�дָ��ָʾ��λ�ÿ�ʼ
int write(int index,int mem_area,int count){
	int i;
	int list = -1;
	int input_length;
	char temp[B];
	
	//����index�ҵ��ļ��򿪱� 
	for(i = 0;i < FILE_NUM;i++){
		if(open_list[i].filesignnum == index){
			list = i;
			break;
		}
	} 
	if(list == -1){
		printf("δ�ҵ���ǰ�������ļ�������ʧ��\n");
		return ERROR; 
	}
	if(open_list[list].flag != BUSY){
		printf("��ǰ�ļ�δ�򿪣�����ʧ��\n");
		return ERROR;
	}
	
	char input[INPUT_LENGTH];
	Init_block(input,INPUT_LENGTH);
	i = 0;
	fflush(stdin);
	while(scanf("%c",&input[i])){
		if(input[i] == '\n'){
			input[i] = '\0';
			break;
		}
		i++;
	} 
	input_length = i;
	//�������봮�ĳ��ȣ���Ҫд��ĳ���Ϊimput_length - mem_area
	//������������д���������ݣ�����Ҫ�ٷ���
	if(count <= BUFFER_LENGTH){
		strncat(open_list[list].buffer,input+mem_area,count);  //���뻺���� 
	} 
	//���ڻ��������ȣ��ִ�д�룬��Ҫ�ֿ�д��
	else{
		int rest;  //��ǰ��������������
		for(i = 0;i < BUFFER_LENGTH;i++){
			if(open_list[list].buffer[i] == FREE){
				rest = BUFFER_LENGTH - i;
				break;
			}
		}
		//��һ���֣���������һ����������������д���ļ�����ջ�����
		strncat(open_list[list].buffer+BUFFER_LENGTH-rest,input + mem_area,rest);
		write_buffer(index,list);
		Init_block(open_list[list].buffer,BUFFER_LENGTH);
		//�ڶ����֣�ѭ��(input_length-mem_area)/BUFFER_LENGTH�飬������д���ļ�
		for(i = 0;i < (count/BUFFER_LENGTH) - 1;i++){
			strncpy(open_list[list].buffer,(input+mem_area)+rest+i*BUFFER_LENGTH,BUFFER_LENGTH);
			write_buffer(index,list);
			Init_block(open_list[list].buffer,BUFFER_LENGTH);
		} 
		//�������֣�ʣ��δ��һ���д�뻺����
		Init_block(open_list[list].buffer,BUFFER_LENGTH);
		strncpy(open_list[list].buffer,(input+mem_area)+rest+i*BUFFER_LENGTH,count%BUFFER_LENGTH);
	} 
	return OK;
}  

//�б���ʾ���е��ļ����䳤��
void directory(){
	int i;
	int filenum;
	int filelength;
	char filename[FILE_NAME_LENGTH];
	char temp[B];
	char tempd[B];
	char temps[B];
	read_block(FILE_SIGN_AREA,temp);
	filenum = temp[1];   //ʵ�ʴ��ڵ��ļ�����
	printf("\n");
	if(filenum == 0){
		printf("��Ŀ¼��û���ļ�\n");
	} 
	
	for(i = 0;i < FILE_NUM;i++){
		read_block(temp[3+i],tempd);   //��ȡĿ¼��
		if(tempd[0] != 0){
			read_block(tempd[0]+FILE_SIGN_AREA,temps); //��ȡ�ļ�������
			if(temps[0] == BUSY && tempd[0] != 0){
				filelength = temps[1];
				strcpy(filename,tempd+1);
				printf("%s\t\t%s�ֽ�\n",filename,filelength);
			} 
		} 
	}
	
	if(filenum != 0){
		printf("����%d���ļ�\n",filenum);
	}
} 

int show_openlist(){
	int i,j;
	int openfile = 0;
	char temp[B];
	int index;
	printf("������\t\t��С\t\t�ļ���\n");
	for(i = 0;i < FILE_NUM;i++){
		if(open_list[i].flag == BUSY){
			index = open_list[i].filesignnum;
			printf("%d",index);
			openfile++;
			read_block(FILE_SIGN_AREA+index,temp);
			printf("\t\t%d",temp[1]);
			//���������ҵ���Ŀ¼��
			for(j = K;j < K + FILE_NUM;j++){
				read_block(j,temp);
				if(temp[0] == index){
					printf("\t\t%s\n",temp+1);
				}
			} 
		}
	}
	return openfile;
}

void show_help(){
	printf("**---------------------------------------------------**\n");
	printf("**                  ����˵��                         **\n");
	printf("**                                                   **\n");
	printf("** dir                 ��ʾĿ¼����                  **\n");
	printf("**                                                   **\n");
	printf("** create + filename   �½���filenameΪ�ļ������ļ�  **\n");
	printf("**                                                   **\n");
	printf("** delete + filename   ɾ����filenameΪ�ļ������ļ�  **\n");
	printf("**                                                   **\n");
	printf("** open + filename     ����filenameΪ�ļ������ļ�  **\n");
	printf("**                                                   **\n");
	printf("** close               �ر�indexΪ�����ŵ��ļ�       **\n");
	printf("**                                                   **\n");
	printf("** read                �������ʾ��������Ҫ�󣬸�����**\n");
	printf("**                     ���Լ���ȡ���Ƚ��ж�����      **\n");
	printf("**                                                   **\n");
	printf("** write               �������ʾ��������Ҫ�󣬸�����**\n");
	printf("**                     ���Լ�д�볤�Ƚ���д����      **\n");
	printf("**                                                   **\n");
	printf("** lseek               �������ʾ��������Ҫ��Ҫ����**\n");
	printf("**                     ��һ���������ļ����ȵ�����    **\n");
	printf("**                     ���ڶ�λ��дָ��              **\n");
	printf("**                                                   **\n");
	printf("** help                ����                          **\n");
	printf("**                                                   **\n");
	printf("** exit                �˳�ϵͳ                      **\n");
	printf("**---------------------------------------------------**\n");
}

void show_ldisk(){
	int a,b;
	for(a = 0;a < K+30;a++){
		printf("%d:",a);
		for(b = 0;b < B;b++){
			printf("%d\t",ldisk[a][b]);
		}
		printf("\n");
	}
}

