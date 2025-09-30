#ifndef GAME_HPP
#define GAME_HPP

#include <iostream>
#include <vector>
#include <string>
#include <sstream>

class Game {
private:
    char board[8][8];
    char current_player;
    int black_count;
    int white_count;
    
    // 八個方向：上、下、左、右、左上、右上、左下、右下
    const int dx[8] = {-1, 1, 0, 0, -1, -1, 1, 1};
    const int dy[8] = {0, 0, -1, 1, -1, 1, -1, 1};
    
    bool is_valid_pos(int row, int col) {
        return row >= 0 && row < 8 && col >= 0 && col < 8;
    }
    
    // 檢查某個方向是否可以翻轉棋子
    bool check_direction(int row, int col, int dir, char player) {
        char opponent = (player == 'X') ? 'O' : 'X';
        int r = row + dx[dir];
        int c = col + dy[dir];
        
        // 至少要有一個對手的棋子
        if (!is_valid_pos(r, c) || board[r][c] != opponent) {
            return false;
        }
        
        // 繼續往這個方向找
        r += dx[dir];
        c += dy[dir];
        
        while (is_valid_pos(r, c)) {
            if (board[r][c] == '*') {
                return false;  // 遇到空格
            }
            if (board[r][c] == player) {
                return true;  // 找到自己的棋子
            }
            r += dx[dir];
            c += dy[dir];
        }
        
        return false;
    }
    
    // 翻轉某個方向的棋子
    void flip_direction(int row, int col, int dir, char player) {
        char opponent = (player == 'X') ? 'O' : 'X';
        int r = row + dx[dir];
        int c = col + dy[dir];
        
        while (is_valid_pos(r, c) && board[r][c] == opponent) {
            board[r][c] = player;
            r += dx[dir];
            c += dy[dir];
        }
    }
    
    void count_pieces() {
        black_count = 0;
        white_count = 0;
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                if (board[i][j] == 'X') black_count++;
                else if (board[i][j] == 'O') white_count++;
            }
        }
    }

public:
    Game() {
        // 初始化棋盤
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                board[i][j] = '*';
            }
        }
        
        // 設置初始四顆棋子（從上到下是第8列到第1列）
        board[3][3] = 'X';  // d5
        board[3][4] = 'O';  // e5
        board[4][3] = 'O';  // d4
        board[4][4] = 'X';  // e4
        
        current_player = 'X';  // X 先手
        black_count = 2;
        white_count = 2;
    }
    
    // 檢查某個位置是否可以下棋
    bool is_valid_move(int row, int col, char player) {
        if (!is_valid_pos(row, col) || board[row][col] != '*') {
            return false;
        }
        
        // 檢查八個方向是否至少有一個可以翻轉
        for (int dir = 0; dir < 8; dir++) {
            if (check_direction(row, col, dir, player)) {
                return true;
            }
        }
        
        return false;
    }
    
    // 下棋
    bool make_move(int row, int col, char player) {
        if (!is_valid_move(row, col, player)) {
            return false;
        }
        
        board[row][col] = player;
        
        // 翻轉所有可以翻轉的方向
        for (int dir = 0; dir < 8; dir++) {
            if (check_direction(row, col, dir, player)) {
                flip_direction(row, col, dir, player);
            }
        }
        
        count_pieces();
        current_player = (player == 'X') ? 'O' : 'X';
        return true;
    }
    
    // 將字串座標轉換為行列（例如 "a1" -> row=7, col=0）
    bool parse_move(const std::string& move, int& row, int& col) {
        if (move.length() != 2) return false;
        
        col = move[0] - 'a';
        row = 8 - (move[1] - '0');  // '1' 對應 row 7，'8' 對應 row 0
        
        return is_valid_pos(row, col);
    }
    
    // 檢查某個玩家是否有合法的移動
    bool has_valid_moves(char player) {
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                if (is_valid_move(i, j, player)) {
                    return true;
                }
            }
        }
        return false;
    }
    
    // 檢查遊戲是否結束
    bool is_game_over() {
        return !has_valid_moves('X') && !has_valid_moves('O');
    }
    
    // 顯示棋盤
    void print_board(const std::string& player_name, const std::string& opponent_name, 
                     char your_piece, bool is_your_turn) {
        // ANSI 顏色代碼
        const std::string RED = "\033[31m";
        const std::string GREEN = "\033[32m";
        const std::string YELLOW = "\033[33m";
        const std::string RESET = "\033[0m";
        
        std::cout << "\n" << player_name << "(you): " << your_piece << "    " 
                  << opponent_name << ": " << (your_piece == 'X' ? 'O' : 'X') << "\n";
        
        // 顯示棋子數量
        std::cout << "X: " << black_count << "    O: " << white_count << "\n";
        
        if (is_your_turn) {
            std::cout << "now it's your turn.\n";
        } else {
            std::cout << "The opponent is thinking.\n";
        }
        
        for (int i = 0; i < 8; i++) {
            std::cout << (8 - i) << " ";
            for (int j = 0; j < 8; j++) {
                if (board[i][j] == 'X') {
                    std::cout << RED << "X" << RESET << " ";
                } else if (board[i][j] == 'O') {
                    std::cout << GREEN << "O" << RESET << " ";
                } else if (is_your_turn && is_valid_move(i, j, your_piece)) {
                    // 顯示可下的位置
                    std::cout << YELLOW << "+" << RESET << " ";
                } else {
                    std::cout << board[i][j] << " ";
                }
            }
            std::cout << "\n";
        }
        std::cout << "  a b c d e f g h\n";
    }
    
    // 獲取棋盤狀態（用於網路傳輸）
    std::string get_board_state() {
        std::stringstream ss;
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                ss << board[i][j];
            }
        }
        return ss.str();
    }
    
    // 設置棋盤狀態（用於網路傳輸）
    void set_board_state(const std::string& state) {
        if (state.length() != 64) return;
        
        int idx = 0;
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                board[i][j] = state[idx++];
            }
        }
        count_pieces();
    }
    
    char get_current_player() { return current_player; }
    void set_current_player(char player) { current_player = player; }
    int get_black_count() { return black_count; }
    int get_white_count() { return white_count; }
    
    // 獲取遊戲結果
    std::string get_result() {
        count_pieces();
        if (black_count > white_count) {
            return "X wins!";
        } else if (white_count > black_count) {
            return "O wins!";
        } else {
            return "Draw!";
        }
    }
};

#endif // GAME_HPP