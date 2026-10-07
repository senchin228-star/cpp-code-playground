#include <string>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <nlohmann/json.hpp>

int main(){
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "Error creating socket" << std::endl;
        return 1;
    }

    struct sockaddr_in server_addr = {};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(5050);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Error connecting to server" << '\n';
        close(sock);
        return 1;
    }
    std::cout << "Connected to server!, Enter message: " << std::endl;

    std::string buffer;
    std::getline(std::cin, buffer);
    nlohmann::json json_message;
    json_message["message"] = buffer;
    json_message["type"] = "text";

    std::string message = json_message.dump() + "\n";
    std::size_t sent_total = 0;
    while (sent_total < message.size()) {
        ssize_t sent = send(sock,
             message.data() + sent_total,
             message.size() - sent_total,
             0
    );
        if (sent < 0) {
            std::cerr << "Error sending data" << std::endl;
            close(sock);
            return 1;
        }
        sent_total += static_cast<std::size_t>(sent);
    }
    close(sock);
    return 0;
}