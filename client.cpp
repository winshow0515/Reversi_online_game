#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sstream>
#include <vector>
#include "game.hpp"

#define BUFFER_SIZE 1024

class Client {
private:
    int sock;
    Game* game;
    std::string player_name;
    std::string opponent_name;
    char my_piece;
    
    std::vector<std::string> split(const std::string& s, char delimiter) {
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream tokenStream(s);
        while (std::getline(tokenStream, token, delimiter)) {
            tokens.push_back(token);
        }
        return tokens;
    }
    
    void send_message(const std::string& msg) {
        send(sock, msg.c_str(), msg.length(), 0);
    }
    
    std::string receive_message() {
        char buffer[BUFFER_SIZE] = {0};
        int valread = read(sock, buffer, BUFFER_SIZE);
        if (valread <= 0) {
            return "";
        }
        return std::string(buffer);
    }
    
    void clear_screen() {
        // 嘗試多種清屏方式
        std::cout << "\033[2J";      // 清除整個螢幕
        std::cout << "\033[3J";      // 清除 scrollback buffer
        std::cout << "\033[H";       // 移動游標到左上角
        std::cout << "\033[0;0H";    // 另一種移動游標方式
        std::cout << std::flush;
        
        // 如果上面都不行，用換行來推開舊內容
        for (int i = 0; i < 3; i++) {
            std::cout << "\n";
        }
    }
    
    void display_board(bool is_my_turn) {
        clear_screen();
        game->print_board(player_name, opponent_name, my_piece, is_my_turn);
    }
    
public:
    Client() {
        sock = -1;
        game = new Game();
        my_piece = ' ';
    }
    
    ~Client() {
        if (game) delete game;
        if (sock != -1) close(sock);
    }
    
    bool connect_to_server(const std::string& server_ip, int server_port) {
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
            std::cerr << "Socket creation failed\n";
            return false;
        }
        
        struct sockaddr_in serv_addr;
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(server_port);
        
        if (inet_pton(AF_INET, server_ip.c_str(), &serv_addr.sin_addr) <= 0) {
            std::cerr << "Invalid address\n";
            return false;
        }
        
        if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
            std::cerr << "Connection failed\n";
            return false;
        }
        
        std::cout << "Connected to server " << server_ip << ":" << server_port << "\n";
        
        std::cout << "Enter your name: ";
        std::getline(std::cin, player_name);
        send_message(player_name);
        
        return true;
    }
    
    void play() {
        while (true) {
            std::string msg = receive_message();
            if (msg.empty()) {
                std::cout << "Connection lost\n";
                break;
            }
            
            std::vector<std::string> parts = split(msg, ':');
            if (parts.empty()) continue;
            
            std::string cmd = parts[0];
            
            if (cmd == "WAIT") {
                std::cout << parts[1] << "\n";
            }
            else if (cmd == "START") {
                opponent_name = parts[1];
                my_piece = parts[2][0];
                
                std::cout << "\nGame started!\n";
                std::cout << "You are playing as " << my_piece << "\n";
                std::cout << "Opponent: " << opponent_name << "\n";
                std::cout << "Waiting for game to begin...\n";
            }
            else if (cmd == "YOUR_TURN") {
                game->set_board_state(parts[1]);
                display_board(true);
                
                // 讀取玩家輸入並發送
                bool move_sent = false;
                while (!move_sent) {
                    std::cout << "\nEnter your step. (ex. a1): ";
                    std::string move;
                    std::getline(std::cin, move);
                    
                    send_message(move);
                    
                    // 等待 server 回應
                    std::string response = receive_message();
                    std::vector<std::string> resp_parts = split(response, ':');
                    
                    if (resp_parts[0] == "INVALID") {
                        std::cout << "Error: " << resp_parts[1] << ". Please try again.\n";
                    } else if (resp_parts[0] == "MOVE_OK") {
                        move_sent = true;
                    }
                }
            }
            else if (cmd == "OPPONENT_TURN") {
                game->set_board_state(parts[1]);
                display_board(false);
            }
            else if (cmd == "SKIP") {
                std::cout << "\nYou have no valid moves. Skipping your turn...\n";
                game->set_board_state(parts[1]);
                sleep(2);
            }
            else if (cmd == "OPPONENT_SKIP") {
                std::cout << "\n" << opponent_name << " has no valid moves. Skipping...\n";
                game->set_board_state(parts[1]);
                sleep(2);
            }
            else if (cmd == "END") {
                game->set_board_state(parts[2]);
                
                clear_screen();
                game->print_board(player_name, opponent_name, my_piece, false);
                
                std::cout << "\n===================\n";
                std::cout << "Game Over!\n";
                std::cout << parts[1] << "\n";
                std::cout << "Black (X): " << game->get_black_count() << "\n";
                std::cout << "White (O): " << game->get_white_count() << "\n";
                std::cout << "===================\n";
                break;
            }
            else if (cmd == "OPPONENT_DISCONNECT") {
                std::cout << "\nOpponent disconnected. You win!\n";
                break;
            }
        }
    }
};

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " <server_ip> <server_port>\n";
        return 1;
    }
    
    std::string server_ip = argv[1];
    int server_port = atoi(argv[2]);
    
    Client client;
    if (!client.connect_to_server(server_ip, server_port)) {
        return 1;
    }
    
    client.play();
    
    return 0;
}