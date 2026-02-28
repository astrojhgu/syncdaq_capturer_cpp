#include <syncdaq.h>
#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <complex>
using namespace syncdaq;
int main() {
    // 1. 创建 socket
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return 1;
    }

    // 2. 绑定本地端口
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(4002);           // 监听端口
    addr.sin_addr.s_addr = INADDR_ANY;      // 监听所有网卡

    if (bind(sockfd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(sockfd);
        return 1;
    }

    std::cout << "Listening on UDP port 4000...\n";

    // 3. 接收数据
    
    Payload payload{};
    char* buffer=(char*)&payload;
    std::complex<int16_t>* raw_iq_data=(std::complex<int16_t>*)payload.data;
    while (true) {
        sockaddr_in src_addr{};
        socklen_t addr_len = sizeof(src_addr);

        ssize_t n = recvfrom(sockfd,
                             buffer,
                             sizeof(payload),
                             0,
                             (sockaddr*)&src_addr,
                             &addr_len);

        if (n < 0) {
            perror("recvfrom");
            break;
        }

        if (n!=sizeof(Payload)){
            continue;
        }

        // 打印来源信息
        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &src_addr.sin_addr, ip, sizeof(ip));

        std::cout << "Received " << n
                  << " bytes from "
                  << ip << ":"
                  << ntohs(src_addr.sin_port)
                  << " pkt_cnt: "<<payload.pkt_cnt
                  << " port_id: "<<payload.port_id
                  << " first data in this frame: "<<raw_iq_data[0]
                  << std::endl;

        //use raw_iq_data here
    }

    close(sockfd);
    return 0;
}
