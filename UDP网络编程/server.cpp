#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>

const int maxn_len = 1024;
const int maxn_fd_size = 1024;
#define sever_ip_addr "192.168.107.130"
#define port 9999   //端口号

/*
    UDP通信的话不需要建立连接；
    客户端知道服务器的ip和端口，直接往服务器发送就行；
    服务器这边直接等客户端发送的数据就行；
    
    UDP通信使用：recvfrom 和 sendto 函数接受 和 发送数据

    int recvfrom(int s, void *buf, int len, unsigned int flags,
               struct sockaddr *from, int *fromlen);
        参数说明：
            s：         socket描述符；

            buf：      UDP数据报缓存区（包含所接收的数据）；

            len：      缓冲区长度。

            flags：   调用操作方式（一般设置为0）。

            from：    指向发送数据的客户端地址信息的结构体（sockaddr_in需类型转换）；

            fromlen：指针，指向from结构体长度值。


    int sendto(int s, const void *buf, int len, unsigned int flags, 
                const struct sockaddr *to, int tolen);
        参数说明：
            s：     socket描述符；

            buf：UDP数据报缓存区（包含待发送数据）；

            len：  UDP数据报的长度；

            flags：调用方式标志位（一般设置为0）；

            to：　 指向接收数据的主机地址信息的结构体（sockaddr_in需类型转换）；

            tolen：to所指结构体的长度；
*/

int main()
{
    char recvbuf[maxn_len]; //接受缓冲区
    char sendbuf[maxn_len]; //发送缓冲区

    //创建服务器的socket，注意这里是报文模式
    int ser_fd = socket(AF_INET,SOCK_DGRAM,0);  
    if(ser_fd == -1)
    {
        perror("创建ser_fd失败！");
        exit(-1);
    }

    //绑定服务器的ip和端口
    struct sockaddr_in serveraddr;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);   //ip
    serveraddr.sin_port = htons(port);    //端口
    serveraddr.sin_family = AF_INET;    //协议族
    bind(ser_fd,(struct sockaddr *)&serveraddr,(socklen_t)sizeof(serveraddr));

    struct sockaddr_in clientaddr;
    while(1)
    {
        //直接recvfrom接受客户端的数据就行了
        int client_len = sizeof(clientaddr);
        int recv_len = recvfrom(ser_fd,recvbuf,sizeof(recvbuf),0,(struct sockaddr *)&clientaddr,(socklen_t*)&client_len);
        if(recv_len == -1)
        {
            perror("接收数据失败！");
            exit(-1);
        }


        printf("接受到的客户端数据为：%s\n",recvbuf);
        printf("现在发送回去.\n\n");

        //向客户端发送数据
        sprintf(sendbuf, recvbuf);
        int send_len = sendto(ser_fd,sendbuf,sizeof(sendbuf),0,(struct sockaddr *)&clientaddr,(socklen_t)client_len);
        if(send_len == -1)
        {
            perror("发送数据失败！");
            exit(-1);
        }
    }

    close(ser_fd);

    return 0;
}

