# Reversi Online Game - 黑白棋連線遊戲

這是一個基於 C++ 的黑白棋（Reversi/Othello）連線對戰遊戲。

## 專案結構

```
.
├── game.hpp       # 遊戲邏輯類別
├── server.cpp     # 伺服器程式
├── client.cpp     # 客戶端程式
├── Makefile       # 編譯設定
└── README.md      # 說明文件
```

## 編譯方式

```bash
make
```

這會產生兩個執行檔：`server` 和 `client`

清除編譯檔案：
```bash
make clean
```

## 使用方式

### 1. 啟動伺服器

```bash
./server <ip> <port>
```

例如：
```bash
./server 127.0.0.1 8888
```

伺服器會等待兩個玩家連線。

### 2. 啟動客戶端（玩家1）

在另一個終端機：
```bash
./client <server_ip> <server_port>
```

例如：
```bash
./client 127.0.0.1 8888
```

輸入你的名字後，會顯示等待另一個玩家。

### 3. 啟動客戶端（玩家2）

再開一個終端機：
```bash
./client 127.0.0.1 8888
```

輸入名字後，遊戲就會開始！

## 遊戲規則

1. 黑白棋是一個 8x8 的棋盤遊戲
2. 初始時棋盤中央有 4 顆棋子（2 黑 2 白）
3. X 代表黑棋，O 代表白棋
4. 玩家輪流下棋，每次必須讓對手至少一顆棋子被夾在中間
5. 被夾住的棋子會翻面變成自己的顏色
6. 如果沒有合法位置可下，則跳過該玩家
7. 當雙方都無法下棋時遊戲結束，棋子數多的一方獲勝

## 操作方式

遊戲中輸入座標來下棋，格式為：`列字母 + 行數字`

例如：
- `a1` - 左下角
- `h8` - 右上角
- `d4` - 中央偏左下
- `e5` - 中央偏右上

## 螢幕顯示範例

```
Ariel(you): X    Bob: O
now it's your turn.
8 * * * * * * * *
7 * * * * * * * *
6 * * * * * * * *
5 * * * X O * * *
4 * * * O X * * *
3 * * * * * * * *
2 * * * * * * * *
1 * * * * * * * *
  a b c d e f g h

Enter your step. (ex. a1): 
```

## 通訊協定

### Server -> Client

- `WAIT:<message>` - 等待另一個玩家
- `START:<opponent_name>:<your_piece>:<is_first>` - 遊戲開始
- `TURN:<board_state>` - 輪到你下棋
- `WAIT_OPPONENT:<board_state>` - 等待對手下棋
- `UPDATE:<move>:<board_state>` - 棋盤更新
- `INVALID:<reason>` - 無效的移動
- `SKIP:<board_state>` - 你沒有合法移動，跳過
- `OPPONENT_SKIP:<board_state>` - 對手跳過
- `END:<result>:<board_state>` - 遊戲結束
- `OPPONENT_DISCONNECT:` - 對手斷線

### Client -> Server

- `<player_name>` - 玩家名字（連線時）
- `<move>` - 移動座標（如 "a1", "h8"）

## 功能特色

✅ 完整的黑白棋規則實作
✅ 自動驗證移動合法性
✅ 即時棋盤更新
✅ 自動翻轉被夾住的棋子
✅ 檢測遊戲結束條件
✅ 處理無法移動的情況（跳過回合）
✅ 友善的終端介面
✅ 斷線處理

## 系統需求

- Linux/Unix 系統（使用 POSIX socket）
- g++ 編譯器（支援 C++11）
- 標準 C++ 函式庫

## 注意事項

1. 確保使用的 port 沒有被其他程式佔用
2. 如果在不同機器上執行，記得調整防火牆設定
3. Server 必須先啟動，Client 才能連線
4. 必須有兩個 Client 連線後遊戲才會開始

## 範例遊戲流程

```bash
# Terminal 1: 啟動伺服器
$ ./server 127.0.0.1 8888
Server started on 127.0.0.1:8888
Waiting for players...
Player 1 connected: Ariel
Player 2 connected: Bob
Ariel (X) goes first!

# Terminal 2: 玩家 1
$ ./client 127.0.0.1 8888
Connected to server 127.0.0.1:8888
Enter your name: Ariel
Waiting for another player...
Game started!
You are playing as X
Opponent: Bob
You go first!

# Terminal 3: 玩家 2  
$ ./client 127.0.0.1 8888
Connected to server 127.0.0.1:8888
Enter your name: Bob
Game started!
You are playing as O
Opponent: Ariel
Ariel goes first!
```

## 未來可能的改進

- [ ] 加入 AI 對手
- [ ] 支援多場遊戲同時進行
- [ ] 加入計時器
- [ ] 記錄遊戲歷史
- [ ] 加入觀戰模式
- [ ] 圖形化介面

## 授權

此專案為教育用途，可自由使用和修改。