//头文件
#include "utilities/my_network.h"
#include "utilities/common.h"

/******************************************
 * 函数功能: 获取本地所有网卡的IP地址
 * 函数参数: 1个
 * ip_info: 保存下每一个ip地址的vector
 * 返回值: 无意义
 *****************************************/
int get_local_ip(std::vector<std::string> &ip_info)
{
    int fd, interface, retn = 0;
    struct ifreq buf[INET_ADDRSTRLEN];
    struct ifconf ifc;
    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) >= 0)
    {
        ifc.ifc_len = sizeof(buf);
        // caddr_t,Linux内核源码里定义的：typedef void *caddr_t；
        ifc.ifc_buf = (caddr_t)buf;
        if (!ioctl(fd, SIOCGIFCONF, (char *)&ifc))
        {
            interface = ifc.ifc_len / sizeof(struct ifreq);
            std::cout << "本机网卡IP如下: " << std::endl;
            while (interface-- > 0)
            {
                if (!(ioctl(fd, SIOCGIFADDR, (char *)&buf[interface])))
                {
                    std::string tmp_ip = (inet_ntoa(((struct sockaddr_in*)(&buf[interface].ifr_addr))->sin_addr));
                    ip_info.push_back(tmp_ip);
                    std::cout << "IP: " << tmp_ip << std::endl;
                }
            }
            // 将默认IP地址也添加至ip内
            ip_info.push_back("0.0.0.0");
        }
    close(fd);
    }
    return 0;
}

/******************************************
 * 函数功能: 初始化Server端
 * 函数参数: 3个
 * ip: 所需要绑定的ip地址
 * port: 所需要绑定的端口号
 * waiting_num: 表示监听队列的最大长度
 * 返回值: server端的socket的句柄
 *****************************************/
int init_server(std::string ip, int port, int waiting_num, bool nonblock)
{
    int server_sock;
    //创建服务器的套接字--IPv4协议，TCP协议
    if((server_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{  
		perror("socket error");
		return -1;
	}
    if(nonblock){
        //设置客户端socket为非阻塞方式
        int sock_flags = fcntl(server_sock, F_GETFL, 0);
        fcntl(server_sock, F_SETFL, sock_flags|O_NONBLOCK);
    }
    //使得地址可以重用
    int reuse = 1;
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    //服务器地址结构体
    struct sockaddr_in server_addr;
    //设置地址结构体
    server_addr.sin_family = AF_INET;//设置为IP通信
    if(ip == "0.0.0.0"){
        server_addr.sin_addr.s_addr = INADDR_ANY;//设置服务器IP为本地所有IP
        std::cout << "当前绑定的网卡为本地所有网卡" << std::endl;
    }
    else{
        if(!inet_aton(ip.c_str(), &server_addr.sin_addr)){//设置服务器IP为指定服务器
            std::cout << "地址有误" << std::endl;
            return -1;
        }
        std::cout << "当前绑定的网卡IP: " << ip << std::endl;
    }
    server_addr.sin_port = htons(port);
    //将套接字绑定到服务器的IP上
    if(bind(server_sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) < 0)
	{
		perror("bind error");
		return -1;
	}
    //开启监听
    if(listen(server_sock, waiting_num) < 0)
	{
		perror("listen error");
		return -1;
	}
    return server_sock;
}

/******************************************
 * 函数功能: 初始化Client端
 * 函数参数: 3个
 * ip: 所需要绑定的ip地址
 * port: 所需要绑定的服务器端口号
 * myport: 所需要绑定的客户端端口号
 * 返回值: client端的socket的句柄
 *****************************************/
int init_client(std::string ip, int port, int myport, bool nonblock)
{
    //创建socket对象
    int client_sock;
    if((client_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{  
		perror("socket error");
		return -1;
	}
    if(nonblock){
        //设置客户端socket为非阻塞方式
        int sock_flags = fcntl(client_sock, F_GETFL, 0);
        fcntl(client_sock, F_SETFL, sock_flags|O_NONBLOCK);
    }
    //若需要客户端绑定固定端口
    if(myport != -1){
        //客户端地址结构体
        struct sockaddr_in client_addr;
        client_addr.sin_family = AF_INET;//设置为IP通信
        client_addr.sin_addr.s_addr=INADDR_ANY;//设置服务器
        client_addr.sin_port=htons(myport);
        //绑定客户端端口以及IP
        bind(client_sock, (const sockaddr*) &client_addr, sizeof(client_addr));
    }
    //服务器地址结构体
    struct sockaddr_in server_addr;
    //设置服务器地址结构体
    server_addr.sin_family = AF_INET;//设置为IP通信
    server_addr.sin_addr.s_addr=inet_addr(ip.c_str());//设置服务器
    server_addr.sin_port=htons(port);
    //将套接字绑定到服务器的IP上
    int connect_ret;
    if((connect_ret = connect(client_sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr))) < 0)
	{
        std::cout << "errno: " << errno << std::endl;
        if(errno == EINPROGRESS){  // 需要检查
            /* 与select相关变量 */
            //句柄集合
            fd_set fds; 
            FD_ZERO(&fds);  // 清空描述符
            // 添加描述符 
            FD_SET(client_sock, &fds);  // 添加客户端的socket
            int select_ret = select(client_sock+1, NULL, &fds, NULL, NULL);
            if(FD_ISSET(client_sock, &fds)){
                int socket_err;
                socklen_t int_size = sizeof(int);
                getsockopt(client_sock, SOL_SOCKET, SO_ERROR, &socket_err, &int_size);
                if(socket_err != 0){
                    perror("connect error");
                    return -1;
                }
            }
            else{
                perror("connect error");
                return -1;
            }
        }
        else{
            perror("connect error");
	        return -1;
        }
	}
    std::cout << "连接服务器成功" << std::endl;
    return client_sock;
}


