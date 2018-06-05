/*
 ============================================================================
 Name        : c4vm.c
 Author      : jjinl
 Version     :
 Copyright   : LGPL
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>


#include <string.h>
#include <unistd.h>
//֧�ֵ�ָ��

enum inst{
	LEA = 1, //LEA <n>,��sp+2 ���ݼ��ص��Ĵ���a
	IMM ,JMP ,JSR ,BZ  ,BNZ ,ENT ,ADJ ,LEV ,LI  ,LC  ,SI  ,SC  ,PSH , OR  ,XOR ,AND ,EQ  ,NE  ,LT  ,GT  ,LE
	,GE  ,SHL ,SHR ,ADD ,SUB ,MUL ,DIV ,MOD ,OPEN,READ,CLOS,PRTF,MALC,FREE,MSET,MCMP,EXIT
};

void *vm_malloc(int size);
int vm_start(unsigned char *text,unsigned char *stack);
int stack_len = 128*1024; //ջ�ռ��СĬ��128K
/*C4ָ�������*/
int main(int argc, char **argv) {

	unsigned char *code;
	unsigned char *stack;
	FILE *fp;
	int file_len,ret;
	printf("C4 Virtual Machine Loading\n");

	if(argc != 2){
		printf("usage : %s vmfile.c4b",argv[0]);
		return -1;
	}

	//���ش��뵽�ڴ�
	fp = fopen(argv[1], "rb");
	if(!fp){
		perror("open:");
		exit(-1);
	}
	//��ȡ�ļ���С
	fseek(fp,0,SEEK_END);
	file_len = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	code = vm_malloc(file_len);
	//���ش���
	ret = fread(code, file_len, 1, fp);
	if(ret != file_len){
		printf("read length error\n");
		exit(-1);
	}
	fclose(fp);

	stack = vm_malloc(stack_len);
	//��������
	vm_start(code, stack);
	return EXIT_SUCCESS;
}

/*�ڴ����룬ʧ���˳�����*/
void *vm_malloc(int size)
{
	void *p;
	p = malloc(size);
	if(!p){
		printf("malloc error\n");
		exit(-1);
	}
	return p;
}

//������Ĵ���
int *vm_pc;  //pc�Ĵ���
int *vm_sp;  //ջ��ָ��
int *vm_bp;  //ջ��ָ��
int vm_a; //�Ĵ���a
int vm_cycle;
int vm_start(unsigned char *text,unsigned char *stack)
{
	int *t;
	//����ջ��ָ��
	vm_bp = vm_sp = (int *)(stack + stack_len);

	//���� 16*4 ��С��ջ�ռ�
	vm_sp -= 16;

	//��һ��ָ��
	vm_pc = (int *)text;
	while(1){

		switch(*vm_pc++){
		case LEA: //LEA <n>,��ջ�ϵı��ر������ص��Ĵ���
			vm_a = (int)(vm_bp + *vm_pc++);
			break;
		case IMM:
			vm_a = *vm_pc++;
			break;
		case JMP:
			vm_pc =(int *) *vm_pc;
			break;
		case JSR:
			*--vm_sp = (int)(vm_pc + 1);
			vm_pc = (int *)*vm_pc;
			break;
		case BZ:
			vm_pc = vm_a ? vm_pc + 1 : (int *)*vm_pc;
			break;
		case BNZ:
			vm_pc = vm_a ? (int *)*vm_pc : vm_pc + 1;
			break;
		case ENT:
			*--vm_sp = (int)vm_bp;
			vm_bp = vm_sp;
			vm_sp = vm_sp - *vm_pc++;
			break;
		case ADJ:
			vm_sp = vm_sp + *vm_pc++;
			break;

		case LEV:
			vm_sp = vm_bp;
			vm_bp = (int *)*vm_sp++;
			vm_pc = (int *)*vm_sp++;
			break;
		case LI:
			vm_a = *(int *)vm_a;
			break;
		case LC:
			vm_a = *(char *)vm_a;
			break;
		case SI:
			*(int *)*vm_sp++ = vm_a;
			break;
		case SC:
			vm_a = *(char *)*vm_sp++ = vm_a;
			break;
		case PSH:
			*--vm_sp = vm_a;
			break;
		case OR:
			vm_a = *vm_sp++ |  vm_a;
			break;
		case XOR:
			vm_a = *vm_sp++ ^  vm_a;
			break;
		case AND:
			vm_a = *vm_sp++ &  vm_a;
			break;
		case EQ:
			vm_a = *vm_sp++ == vm_a;
			break;
		case NE:
			vm_a = *vm_sp++ != vm_a;
			break;
		case LT:
			vm_a = *vm_sp++ <  vm_a;
			break;
		case GT:
			vm_a = *vm_sp++ >  vm_a;
			break;
		case LE:
			vm_a = *vm_sp++ <= vm_a;
			break;
		case GE:
			vm_a = *vm_sp++ >= vm_a;
			break;
		case SHL:
			vm_a = *vm_sp++ << vm_a;
			break;
		case SHR:
			vm_a = *vm_sp++ >> vm_a;
			break;
		case ADD:
			vm_a = *vm_sp++ +  vm_a;
			break;
		case SUB:
			vm_a = *vm_sp++ -  vm_a;
			break;
		case MUL:
			vm_a = *vm_sp++ *  vm_a;
			break;
		case DIV:
			vm_a = *vm_sp++ /  vm_a;
			break;
		case MOD:
			vm_a = *vm_sp++ %  vm_a;
			break;
		case OPEN:
			vm_a = open((char *)vm_sp[1], *vm_sp);
			break;
		case READ:
			vm_a = read(vm_sp[2], (char *)vm_sp[1], *vm_sp);
			break;
		case CLOS:
			vm_a = close(*vm_sp);
			break;
		case PRTF:
			t = vm_sp + vm_pc[1];
			vm_a = printf((char *)t[-1], t[-2], t[-3], t[-4], t[-5], t[-6]);
			break;
		case MALC:
			vm_a = (int)malloc(*vm_sp);
			break;
		case FREE:
			free((void *)*vm_sp);
			break;
		case MSET:
			vm_a = (int)memset((char *)vm_sp[2], vm_sp[1], *vm_sp);
			break;
		case MCMP:
			vm_a = memcmp((char *)vm_sp[2], (char *)vm_sp[1], *vm_sp);
			break;
		case EXIT:
			printf("exit(%d) cycle = %d\n", *vm_sp, vm_cycle);
			return *vm_sp;
			break;
		default:
			printf("unknown instruction = %d! cycle = %d\n", *vm_pc, vm_cycle);
			return -1;
		}

	}
}


