#include <string>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

int main(){
    int listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock < 0) {
        std::cerr << "Error creating socket" << std::endl;
        return 1;
    }
    struct sockaddr_in server_addr = {};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(5050);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(listen_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Error binding socket" << std::endl;
        close(listen_sock);
        return 1;
    }

    if (listen(listen_sock, 1) < 0) {
        std::cerr << "Error listening on socket" << std::endl;
        close(listen_sock);
        return 1;
    }
    std::cout << "Server is listening on port 5050..." << std::endl; 

    struct sockaddr_in client_addr = {};
    socklen_t client_addr_len = sizeof(client_addr);

    int client_sock = accept(listen_sock, (struct sockaddr*)&client_addr, &client_addr_len);
    if (client_sock < 0) {
        std::cerr << "Error accepting connection" << std::endl;
        close(listen_sock);
        return 1;
    }

    std::cout << "Client connected!" << std::endl;

    std::string buffer(1024, '\0');
    std::string full_message;
    std::string::size_type endpos = -1;
    while (true) { // While full json message is not received, keep receiving data
        buffer.resize(1024, '\0'); // Reset buffer size for each recv call
        ssize_t bytes_received = recv(client_sock, &buffer[0], buffer.size(), 0);
        if (bytes_received < 0) {
            std::cerr << "Error receiving data" << std::endl;
            close(client_sock);
            close(listen_sock);
            return 1;
        } 
        if (bytes_received == 0) {
            std::cout << "Client disconnected" << std::endl;
            break;
        }
        buffer.resize(bytes_received); // Resize buffer to actual received size
        if ((endpos = buffer.find('\n')) != std::string::npos) { // Check if full message is received
            buffer.erase(endpos); // Remove the newline character from the buffer
            full_message += buffer; // Append received data to full message
            break;
        } else {
            full_message += buffer; // Append received data to full message
        }
    }
    std::cout << "Received message: " << full_message << std::endl;
    close(client_sock);
    close(listen_sock);
}