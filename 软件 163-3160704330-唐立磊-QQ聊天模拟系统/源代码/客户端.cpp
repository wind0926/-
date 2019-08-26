#include<stdio.h>    
#include<stdlib.h>    
#include<netinet/in.h>    
#include<sys/socket.h>    
#include<arpa/inet.h>    
#include<string.h>    
#include<unistd.h>    

#include <sys/epoll.h>
#include <errno.h>
#include <fcntl.h>

#define BUFFER_SIZE 1024    

int main(int argc, const char * argv[])    
{   
    int i,n;
    int connfd,sockfd;
    struct epoll_event ev,events[20]; //ev����ע���¼�,�������ڻش�Ҫ������¼�
    int epfd=epoll_create(256);//����һ��epoll�ľ��������256Ϊ��epoll��֧�ֵ��������

    struct sockaddr_in client_addr;
    struct sockaddr_in server_addr;    

    server_addr.sin_family = AF_INET;    
    server_addr.sin_port = htons(11277);    
    server_addr.sin_addr.s_addr =INADDR_ANY;    
    bzero(&(server_addr.sin_zero), 8);    

    int server_sock_fd = socket(AF_INET, SOCK_STREAM, 0);  

    ev.data.fd=server_sock_fd;//������Ҫ������¼���ص��ļ�������
    ev.events=EPOLLIN|EPOLLET;//����Ҫ������¼�����
    epoll_ctl(epfd,EPOLL_CTL_ADD,server_sock_fd,&ev);//ע��epoll�¼�

    if(server_sock_fd == -1)    
    {    
        perror("socket error");    
        return 1;    
    }    

    char recv_msg[BUFFER_SIZE];    
    char input_msg[BUFFER_SIZE];    

    if(connect(server_sock_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in)) == 0)    
    {    
        for(;;)
        {
            int nfds=epoll_wait(epfd,events,20,500);//�ȴ�epoll�¼��ķ���
            for(i=0;i<nfds;++i)
            {    
                if(events[i].events&EPOLLOUT) //�����ݷ��ͣ�дsocket
                {
                    bzero(input_msg, BUFFER_SIZE);    
                    fgets(input_msg, BUFFER_SIZE, stdin);    

                    sockfd = events[i].data.fd;
                    write(sockfd, recv_msg, n);

                    ev.data.fd=sockfd;
                    ev.events=EPOLLIN|EPOLLET;
                    epoll_ctl(epfd,EPOLL_CTL_ADD,sockfd,&ev);
                }   

                else if(events[i].events&EPOLLIN)//�����ݵ�������socket
                {
                    bzero(recv_msg, BUFFER_SIZE);
                    if((n = read(server_sock_fd, recv_msg, BUFFER_SIZE)) <0 )
                    {
                        printf("read error!");
                    }

                    ev.data.fd=server_sock_fd;
                    ev.events=EPOLLOUT|EPOLLET;
                    printf("%s\n",recv_msg);
                }

            }        
        }
    }    
    return 0;    
}   
