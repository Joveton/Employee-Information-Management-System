/*************************************************************************
#	 FileName	: server.c
#	 Author		: fengjunhui 
#	 Email		: 18883765905@163.com 
#	 Created	: 2018年12月29日 星期六 13时44分59秒
 ************************************************************************/

#include<stdio.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#include "common.h"

int do_admin_login(int sfd, MSG *cmessage);
void do_admin_display();
void do_admin_query(int sfd, MSG *cmessage);
void do_admin_modify(int sfd, MSG *cmessage);
void do_admin_add(int sfd, MSG *cmessage);
void do_admin_delete(int sfd, MSG *cmessage);
void do_admin_history(int sfd, MSG *cmessage);

int do_user_login(int sfd, MSG *cmessage);
void do_user_query(int sfd, MSG *cmessage);
void do_user_modify(int sfd, MSG *cmessage);
void do_quit(int sfd, MSG *cmessage);

void display_title();
void display_table_info(MSG *cmessage);   

int main(int argc,const char * argv[])
{
	int sfd;
	int ret,relen;
	int len;
	int num;
	struct sockaddr_in serveraddr;
	MSG cmessage;

	memset(&cmessage,0,sizeof(cmessage));
	len = sizeof(serveraddr);


	if(argc != 3)
	{
		printf("User:%s<ip> <port>\n",argv[0]);
		return -1;
	}


	sfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sfd < 0)
	{
		perror("socket");
		return -1;
	}
	printf("sfd = %d\n",sfd);

	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(atoi(argv[2]));
	serveraddr.sin_addr.s_addr = inet_addr(argv[1]);

	ret = connect(sfd, (struct sockaddr *)&serveraddr, len);
	if(ret < 0)
	{
		perror("connect");
		return -1;
	}

BFP:
	while(1)
	{
		printf("*******************************************\n");
		printf("1.管理员模式\t 2.员工模式\t 3.退出\n");
		printf("*******************************************\n");
		printf("请输入你的选择:");

		ret = scanf("%d", &num);
		if(ret == 0)
		{
			getchar();
			scanf("%d", &num);
		}

		switch(num)
		{
			case 1:
				if(do_admin_login(sfd, &cmessage) == 1)
				{
					goto NEXT_ADMIN;
				}
				break;
			case 2:
				if(do_user_login(sfd, &cmessage) == 1)
				{
					goto NEXT_USER;
				}
				break;
			case 3:
				do_quit(sfd, &cmessage);
				exit(0);
			default:
				break;

		}
	}

NEXT_ADMIN:   //二级页面
	while(1)
	{
		printf("欢迎伟岸的管理员!\n");
		printf("*********************************************************\n");
		printf("1.查询   2.修改   3.添加用户   4.删除用户   5.查询历史记录\n");
		printf("6.退出\n");
		printf("*********************************************************\n");

		printf("请选择操作:");
		ret = scanf("%d",&num);
		while(ret == 0)
		{
			getchar();
			ret = scanf("%d",&num);
		}

		switch(num)
		{
			case 1:
				do_admin_query(sfd, &cmessage);
				break;
			case 2:
				do_admin_modify(sfd, &cmessage);
				break;
			case 3:
				do_admin_add(sfd, &cmessage);
				break;
			case 4:
				do_admin_delete(sfd, &cmessage);
				break;
			case 5:
				do_admin_history(sfd, &cmessage);
				break;
			case 6:
				goto BFP;
			default:
				break;
		}
	}


NEXT_USER:  //二级页面
	while(1)
	{
		printf("国庆七天乐,劳动人民最快乐\n");
		printf("************************\n");
		printf("1.查询   2.修改   3.退出\n");
		printf("************************\n");

		printf("请选择操作:");
		ret = scanf("%d", &num);
		while(ret == 0)
		{
			getchar();
			ret = scanf("%d", &num);
		}

		switch(num)
		{
			case 1:
				do_user_query(sfd,&cmessage);
				break;
			case 2:
				//do_user_modify(sfd, &cmessage);
				break;
			case 3:
				goto BFP;
			default:
				break;
		}


	}


}


int do_admin_login(int sfd, MSG *cmessage)
{
	cmessage->usertype = ADMIN;
	printf("请输入用户名:");
	scanf("%s", cmessage->username);
	printf("\n请输入密码:");
	scanf("%s", cmessage->passwd);
	cmessage->msgtype = ADMIN_LOGIN;

	send(sfd, cmessage, sizeof(MSG), 0);
	recv(sfd, cmessage, sizeof(MSG), 0);
	if(strcmp(cmessage->recvmsg, "OK")!= 0)
	{
		printf("admin login failed\n");
		return -1;
	}
	return 1;
}

void do_admin_query(int sfd, MSG *cmessage)
{
	int num;
	cmessage->msgtype = ADMIN_QUERY;
	printf("**********************************\n");
	printf("1.按姓名查询   2.查询所有    3退出\n");
	printf("**********************************\n");
	printf("请输入操作:");
	scanf("%d",&num);
	switch(num)
	{
		case 1:
			printf("请输入要查询的姓名:");
			scanf("%s",cmessage->info.name);
			strcpy(cmessage->recvmsg, "name");

			send(sfd, cmessage, sizeof(MSG), 0);
			recv(sfd, cmessage, sizeof(MSG), 0);
			if(strcmp(cmessage->recvmsg, "OK")==0)
			{
				display_title();
				display_table_info(cmessage);
			
			}else
			{
				printf("查询姓名失败\n");
			}
			break;
		case 2:
			strcpy(cmessage->recvmsg, "all");
			send(sfd, cmessage, sizeof(MSG), 0);
			recv(sfd, cmessage, sizeof(MSG), 0);
			display_table_info(cmessage);

			if(strcmp(cmessage->recvmsg,"begin") == 0)
			{

				while(strcmp(cmessage->recvmsg, "over")!= 0)
				{
					recv(sfd, cmessage, sizeof(MSG), 0);
					display_table_info(cmessage);
				}
			}else
			{
				printf("查询所有失败\n");
			}
		case 3:
			exit(0);

	}

}

void do_admin_modify(int sfd, MSG *cmessage)
{
	int num; 
	cmessage->msgtype = ADMIN_MODIFY;
	printf("请输入要修改的工号:");
	scanf("%d", &cmessage->info.no);
	while(1)
	{
		printf("\n请输入要修改的选项:\n");
		printf("****************************************\n");
		printf("1.姓名    2.年龄    3.家庭住址    4.电话\n");
		printf("5.职位    6.工资    7.入职年月    8.评级\n");
		printf("9.密码    10.退出\n");
		printf("****************************************\n");
		scanf("%d", &num);
		switch(num)
		{
			case 1:
				printf("请输入要修改的姓名:\n");
				scanf("%s", cmessage->info.name);
				strcpy(cmessage->recvmsg,"name");
				send(sfd, cmessage, sizeof(MSG), 0);
				recv(sfd, cmessage, sizeof(MSG), 0);
				if(strcmp(cmessage->recvmsg, "OK")==0)
				{
					printf("修改姓名成功!\n");
				}else
				{
					printf("修改姓名失败!\n");
				}
				break;
			case 2:
				printf("请输入要修改的年龄:\n");
				scanf("%d", &cmessage->info.age);
				strcpy(cmessage->recvmsg,"age");
				//printf("----------------%d\n",cmessage->info.age);
				//printf("----------------%s\n",cmessage->recvmsg);
				send(sfd, cmessage, sizeof(MSG), 0);
				recv(sfd, cmessage, sizeof(MSG), 0);
				if(strcmp(cmessage->recvmsg, "OK")==0)
				{
					printf("修改年龄成功!\n");
				}else
				{
					printf("修改年龄失败!\n");
				}
				break;
			case 10:
				return ;
			default:
				break;

		}
	}

}

void do_admin_add(int sfd, MSG *cmessage)
{
	char ch;
	cmessage->msgtype = ADMIN_ADDUSER;
	printf("*****热烈欢迎新员工******\n");
	printf("请输入新员工的工号:");
	scanf("%d", &cmessage->info.no);
	printf("\n请输入新员工的姓名:");
	scanf("%s", cmessage->info.name);
	printf("\n请输入新员工的年龄:");
	scanf("%d", &cmessage->info.age);
	printf("请输入新员工的电话:");
	scanf("%s", cmessage->info.phone);
	printf("请输入新员工的地址:");
	scanf("%s", cmessage->info.addr);
	printf("请输入新员工的等级:");
	scanf("%d",&cmessage->info.level);
	printf("请输入新员工的工资:");
	scanf("%lf", &cmessage->info.salary);
	printf("是否将员工设置为管理员(y or n)");
	scanf("%c",&ch);

	if(ch == 'y')
	{
		cmessage->info.usertype = 1;
	}else
	{
		cmessage->info.usertype = 0;
	}
	send(sfd, cmessage, sizeof(MSG), 0);
	recv(sfd, cmessage, sizeof(MSG), 0);
	if(strcmp(cmessage->recvmsg, "OK") == 0)
	{
		printf("插入新用户成功\n");
	}else
	{
		printf("插入新用户失败\n");
	}


}

void do_admin_delete(int sfd, MSG *cmessage)
{
	cmessage->msgtype = ADMIN_DELUSER;
	printf("请输入要删除员工的工号:");
	scanf("%d", &cmessage->info.no);
	send(sfd, cmessage, sizeof(MSG), 0);
	recv(sfd, cmessage, sizeof(MSG), 0);
	if(strcmp(cmessage->recvmsg, "OK"))
	{
		printf("删除用户成功\n");
	}else
	{
		printf("删除用户失败\n");
	}

}

void do_admin_history(int sfd, MSG *cmessage)
{
	cmessage->msgtype = ADMIN_HISTORY;
	send(sfd, cmessage, sizeof(MSG), 0);
	recv(sfd, cmessage, sizeof(MSG), 0);

	if(strcmp(cmessage->recvmsg, "begin")==0)
	{
		printf("开始接收数据\n");
		while(strcmp(cmessage->recvmsg, "over")!=0)
		{
			recv(sfd,cmessage, sizeof(MSG), 0);
			printf("%s\n",cmessage->recvmsg);
		}
	}else
	{
		printf("查询历史失败!\n");
	}
}






int do_user_login(int sfd, MSG *cmessage)
{
	cmessage->msgtype = USER_LOGIN;
	printf("请输入用户名:");
	scanf("%s",cmessage->username);
	printf("\n请输入密码:");
	scanf("%s",cmessage->passwd);
	cmessage->usertype = USER;
	send(sfd, cmessage, sizeof(MSG), 0);
	recv(sfd, cmessage, sizeof(MSG), 0);

	if(strcmp(cmessage->recvmsg, "OK") == 0)
	{
		return 1;
	}
	return -1;
}


void do_user_query(int sfd, MSG *cmessage)
{
	cmessage->msgtype = USER_QUERY;
	send(sfd, cmessage, sizeof(MSG), 0);
	recv(sfd, cmessage, sizeof(MSG), 0);

	if(strcmp(cmessage->recvmsg, "OK")==0)
	{
		display_title();
		display_table_info(cmessage);
	}else
	{
		printf("用户查询失败\n");
	}
}

void do_user_modify(int sfd, MSG *cmessage)
{
	int num;
	printf("*****USER MODIFY*****\n");
	printf("*****************************************\n");
	printf("1.家庭住址    2.电话    3.密码     4.退出\n");
	printf("*****************************************\n");

	printf("请输入数字:");
	scanf("%d",&num);
	switch(num)
	{
		case 1:
			strcpy(cmessage->recvmsg,"addr");
			printf("请输入新的家庭住址:");
			scanf("%s", cmessage->info.addr);
			send(sfd, cmessage, sizeof(MSG), 0);
			recv(sfd, cmessage, sizeof(MSG), 0);
			if(strcmp(cmessage->recvmsg ,"OK")==0)
			{
				printf("住址更改成功\n");
			}else
			{
				printf("住址更改失败\n");
			}
	}
}

void do_quit(int sfd, MSG *cmessage)
{
	cmessage->msgtype = QUIT;
	send(sfd, cmessage, sizeof(MSG), 0);
}


void display_title()
{
	printf("工号    用户类型     姓名     密码     年龄    电话\n");
}

void display_table_info(MSG *cmessage)
{
	printf("%d\t %4d\t %12s %8s %d\t %s\t\n",cmessage->info.no, cmessage->info.usertype,\
			cmessage->info.name, cmessage->info.passwd,cmessage->info.age, cmessage->info.phone);
}