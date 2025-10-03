# Reversi Online Game - 黑白棋連線遊戲

C++ 寫的黑白棋（Reversi）連線對戰遊戲。

# 目錄
* [專案結構](#專案結構)
* [Prerequisite](#prerequisite)
* [編譯方式](#編譯方式)
* [使用方式](#使用方式)
* [遊戲規則](#遊戲規則)
* [遊戲畫面](#遊戲畫面)
* [常見問題](#常見問題)
* [範例遊戲流程](#範例遊戲流程)
* [技術細節](#技術細節)
* [心得](#心得)


## 專案結構
```
.
├── game.hpp       # 遊戲邏輯類別
├── server.cpp     # 伺服器程式
├── client.cpp     # 客戶端程式
├── Makefile       # 編譯設定
└── README.md      # 說明文件
```

## Prerequisite
- **作業系統**: Linux/Unix（已在 Raspberry Pi 4 測試通過）
- **編譯器**: g++（支援 C++11）
- **終端**: 支援 ANSI 顏色碼的終端（大部分 Linux 終端都支援）
- **網路**: TCP/IP 連線能力

## 編譯方式
```bash
# 編譯所有程式
make
# 清除編譯檔案
make clean
```
編譯後會產生兩個執行檔：`server` 和 `client`

### 設定執行權限（如果需要）

```bash
chmod +x server client
```

## 使用方式
#### 1. 啟動伺服器

在伺服器端執行：

```bash
./server <ip> <port>

範例：
./server 192.168.0.222 8888
```

#### 2. 玩家連線

在客戶端執行：

```bash
./client <server_ip> <server_port>

範例：
./client 192.168.0.222 8888
```

連線後會要求輸入名字：
```
Connected to server 192.168.0.222:8888
Enter your name: Ariel
```
第一個玩家會看到：
```
Waiting for another player...
```
當第二個玩家連線後，遊戲自動開始！

## 遊戲規則
1. 黑白棋是一個 8x8 的棋盤遊戲
2. 初始時棋盤中央有 4 顆棋子（2 黑 2 白）
   ```
   5 * * * X O * * *
   4 * * * O X * * *
   ```
3. X 代表黑棋，O 代表白棋
4. 玩家輪流下棋，每次下棋必須：
   - 在空格位置下棋
   - 至少讓對手一顆棋子被你的棋子夾在中間（八個方向任一）
5. 被夾住的所有棋子會翻面變成你的顏色
6. 如果沒有合法位置可下，則自動跳過該玩家
7. 當雙方都無法下棋時遊戲結束
8. **棋子數多的一方獲勝**

## 遊戲畫面
### 輪到你時

```
Ariel(you): X    Bob: O
X: 4    O: 1
now it's your turn.
8 * * * * * * * *
7 * * * * * * * *
6 * * * * + * * *
5 * * * X X + * *
4 * * * O X * * *
3 * * * + * * * *
2 * * * * * * * *
1 * * * * * * * *
  a b c d e f g h

Enter your step. (ex. a1): 
```

- 🔴 紅色 X：你的黑棋
- 🟢 綠色 O：對手的白棋
- 🟡 黃色 +：可以下棋的位置

### 對手回合時

```
Ariel(you): X    Bob: O
X: 4    O: 1
The opponent is thinking.
8 * * * * * * * *
7 * * * * * * * *
6 * * * * X * * *
5 * * * X X * * *
4 * * * O X * * *
3 * * * * * * * *
2 * * * * * * * *
1 * * * * * * * *
  a b c d e f g h
```

### 遊戲結束

```
===================
Game Over!
X wins!
Black (X): 35
White (O): 29
===================
```

## 常見問題
### Q: 遊戲中途斷線怎麼辦？

**A:** 
- 對手會立即收到斷線通知
- Server 會自動結束遊戲
- 重新啟動 Server 和 Client 即可開始新遊戲

## 範例遊戲流程

```bash
# ===== Terminal 1: 伺服器 =====
$ ./server 192.168.0.222 8888
Server started on 192.168.0.222:8888
Waiting for players...
Player 1 connected: Ariel
Player 2 connected: Bob
Ariel (X) goes first!
Ariel (X) played f5
Bob (O) played f6
Ariel (X) played e6
...
Game over: X wins!

# ===== Terminal 2: 玩家 1 =====
$ ./client 192.168.0.222 8888
Connected to server 192.168.0.222:8888
Enter your name: Ariel
Waiting for another player...
Game started!
You are playing as X
Opponent: Bob
Waiting for game to begin...

Ariel(you): X    Bob: O
X: 2    O: 2
now it's your turn.
[棋盤顯示，+ 標示可下位置]

Enter your step. (ex. a1): f5

# ===== Terminal 3: 玩家 2 =====
$ ./client 192.168.0.222 8888
Connected to server 192.168.0.222:8888
Enter your name: Bob
Game started!
You are playing as O
Opponent: Ariel
Waiting for game to begin...

Bob(you): O    Ariel: X
X: 4    O: 1
The opponent is thinking.
[等待對手下棋...]
```

## 技術細節

### 使用的技術

- **Socket 程式設計**: POSIX TCP Socket
- **非阻塞 I/O**: 使用 `select()` 監控連線狀態
- **ANSI Escape Codes**: 終端顏色和清屏
- **C++11 標準**: STL 容器和字串處理

### 架構設計

```
Server (單執行緒，順序處理)
  ├── 監聽連線
  ├── 等待兩個玩家
  ├── 遊戲主迴圈
  │   ├── 檢查連線狀態
  │   ├── 驗證移動合法性
  │   ├── 同步棋盤狀態
  │   └── 切換回合
  └── 結束遊戲

Client (阻塞式，依序處理訊息)
  ├── 連線到伺服器
  ├── 訊息處理迴圈
  │   ├── 接收伺服器訊息
  │   ├── 更新棋盤顯示
  │   ├── 等待玩家輸入
  │   └── 發送移動給伺服器
  └── 斷線處理

Game 類別（遊戲邏輯）
  ├── 棋盤管理
  ├── 移動驗證
  ├── 棋子翻轉
  └── 遊戲狀態檢查
```

## 心得
我第一次用 C++ 寫專案，學到了些新東西  
#### Makefile  
方便的編譯工具

#### argc argv
使用 main 函數的 `argc` 與 `argv` 兩個參數，來取得執行程式時所輸入的指令參數。

#### C++ 的 socket 實作
C++ 的 socket 實作比起 Python 更複雜，例如要自己設定 `socket FD`，我還因此去了解了 File descriptor。
網路用的位元組順序是 Big-Endian 而電腦主機用的是 Little-Endian，所以需要 htons() 來轉換。

#### .hpp
`.hpp` 是標頭檔(header files)，用來存放程式碼的宣告，有了這個寫遊戲簡單多了

#### 使用 claude AI
這次我使用 claude 協助我開發專案。目前覺得 claude 程式寫又好又完整，還能管理版本，雖然偶爾會因 token 限制被迫暫停開發，但這仍然瑕不掩瑜，總體來說非常好用。