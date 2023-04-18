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

int main()
{
    char recvbuf[maxn_len]; //接受缓冲区
    char sendbuf[maxn_len]; //发送缓冲区

    //创建用于通信的的socket，注意这里是报文模式
    int con_fd = socket(AF_INET,SOCK_DGRAM,0);  
    if(con_fd == -1)
    {
        perror("创建con_fd失败！");
        exit(-1);
    }

    //通过struct sockaddr_in 结构设置服务器地址和监听端口；
    struct sockaddr_in ser_add;
    inet_pton(AF_INET,sever_ip_addr,&ser_add.sin_addr.s_addr);    //点分十进制 转换成 整数字节序
    ser_add.sin_family = AF_INET;
    ser_add.sin_port = htons(port);   //主机字节序 转换为 网络字节序
    //这里不用连接

    while(1)
    {
        //直接向服务端发送数据就行了
        printf("请输入要发送的数据：");
        scanf("%s",sendbuf);
        int server_len = sizeof(ser_add);
        int send_len = sendto(con_fd,sendbuf,sizeof(sendbuf),0,(struct sockaddr *)&ser_add,(socklen_t)server_len);
        if(send_len == -1)
        {
            perror("发送数据失败！");
            exit(-1);
        }

        //接受服务器回射的数据
        int recv_len = recvfrom(con_fd,recvbuf,sizeof(recvbuf),0,(struct sockaddr *)&ser_add,(socklen_t *)&server_len);
        printf("接受到服务器的回射数据为：%s\n\n",recvbuf);
    }

    return 0;
}


