#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <iostream>
using namespace std;

const int maxn_len = 1024;
const int maxn_fd_size = 1024;
#define sever_ip_addr "192.168.107.130"
#define port 9999   //端口号

int main()
{
    //创建socket
    int lfd = socket(AF_INET,SOCK_STREAM,0);
    if(lfd == -1)
    {
        perror("创建lfd失败！");
        exit(-1);
    }

    //绑定 ip 和 端口
    struct sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;    //协议族
    serveraddr.sin_port = htons(port);  //主机字节序转换到网络字节序
    serveraddr.sin_addr.s_addr = INADDR_ANY;    //绑定主机就行
    bind(lfd,(struct sockaddr*)&serveraddr,sizeof serveraddr);

    //监听
    listen(lfd,8);  //第二个参数是accept队列的最大长度+1

    //创建epoll实例
    int epfd = epoll_create(8);

    //将监听的文件描述符相关的检测信息添加到epoll实例中
    struct epoll_event epev;
    epev.data.fd = lfd;
    epev.events = EPOLLIN;  //检测可写
    epoll_ctl(epfd,EPOLL_CTL_ADD,lfd,&epev);

    struct epoll_event epevs[maxn_fd_size]; //就绪事件链表

    cout<<"开始rpoll_wait\n";

    //开始epoll_wait
    while(1)
    {
        int ret = epoll_wait(epfd,epevs,maxn_fd_size,-1);   //最后一个参数表示epoll_wait是否阻塞
        if(ret == -1)
        {
            perror("epoll_wait检测失败！");
            exit(-1);
        }

        printf("检测到的就绪事件个数为：%d\n",ret);

        //遍历就绪事件链表
        for(int i=0;i<ret;i++)
        {
            int curfd = epevs[i].data.fd;

            if(curfd == lfd)    //是监听描述符
            {
                // 监听的文件描述符有数据达到，有客户端连接
                struct sockaddr_in clientaddr;
                int len = sizeof(clientaddr);
                int cfd = accept(lfd, (struct sockaddr *)&clientaddr, (socklen_t *)&len);

                //将用于通信的socket加入到epoll中
                epev.events = EPOLLIN;
                epev.data.fd = cfd;
                epoll_ctl(epfd,EPOLL_CTL_ADD,cfd,&epev);
            }
            else    //客户端有数据发送过来
            {
                if(epevs[i].events & EPOLLOUT) {
                    continue;
                }  

                // 有数据到达，需要通信
                char recvbuf[maxn_len];
                cout<<"recvbuf的大小为：";
                cout<<sizeof(recvbuf)<<'\n';
                int len = read(curfd,recvbuf,100);
                // int len = recv(curfd,recvbuf,sizeof recvbuf,0); //第四个参数=0，则recv和read一样

                if(len == -1)
                {
                    perror("读取数据出错！");
                    exit(-1);
                }
                else if(len == 0)
                {
                    printf("客户端断开连接！");
                    epoll_ctl(epfd,EPOLL_CTL_DEL,curfd,NULL);   //从epoll中删除
                    break;
                }
                else
                {
                    printf("接收到的数据为：%s\n", recvbuf);
                    printf("现在发送回去\n");
                    // write(curfd, recvbuf, strlen(recvbuf)+1);
                    send(curfd, recvbuf, strlen(recvbuf)+1,0);
                }

            }
        }
    }

    close(lfd);
    close(epfd);

    return 0;
}

