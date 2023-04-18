#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
using namespace std;

const int maxn_len = 1024;

#define sever_ip_addr "192.168.107.130"
#define port 9999   //端口号

int main()
{
    //创建socket
    int fd = socket(AF_INET,SOCK_STREAM,0);
    if(fd == -1)    //创建失败
    {
        perror("创建失败！");
        exit(-1);
    }

    //连接服务器
    struct sockaddr_in serveradd;
    inet_pton(AF_INET,sever_ip_addr,&serveradd.sin_addr.s_addr);    //点分十进制 转换成 整数字节序
    serveradd.sin_family = AF_INET;     //协议族
    serveradd.sin_port = htons(port);   //主机字节序 转换为 网络字节序
    int ret = connect(fd,(struct sockaddr*)&serveradd,sizeof(serveradd));

    if(ret == -1)
    {
        perror("连接失败！");
        exit(-1);
    }

    //通信
    char recvbuff[maxn_len];
    while(1)
    {
        //从键盘输入，给服务端发送数据
        char date[maxn_len];
        memset(date,0,sizeof date);
        printf("请输入发送数据：");
        scanf("%s",date);
        // write(fd,date,sizeof date);
        send(fd, date, strlen(date)+1,0);   //第四个参数=0，则send和write一样

        sleep(1);

        //接受数据
        // int len = read(fd,recvbuff,sizeof recvbuff);
        int len = recv(fd,recvbuff,sizeof recvbuff,0);  //第四个参数=0，则recv和read一样

        if(len == -1)
        {
            perror("接收数据错误！");
            exit(-1);
        }
        else if(len == 0)
        {
            printf("服务器断开连接！");
            break;
        }
        else if(len > 0)
        {
            printf("我接受到了回射服务器的返回的数据 : %s\n\n", recvbuff);
        }
    }

    close(fd);

    return 0;
}

