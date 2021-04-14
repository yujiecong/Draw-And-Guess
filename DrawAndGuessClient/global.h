
#ifndef GLOBAL_H
#define GLOBAL_H

#define PORT 12345
enum MESSAGE_TYPE{
    CHAT,
    DRAW,
    NEW_CLIENT_BROADCAST, //  有新client连接 广播给所有客户端知晓
    SERVER_BROADCAST,//服务端广播 例如出题
    REGESTER,
    POINT_TO_POINT,//单个客户端单 发送给服务端
};
enum {
  TYPE_POINT,
    TYPE_LINE,
    TYPE_CIRCLE,
    TYPE_ECLIPSE,
    TYPE_RECT,
    TYPE_ERASER,
    TYPE_ERASERALL,
};
enum GAME_STATE{
    WAIT_DRAWING,
    IS_DRAWING,
    FINISNED_DRAWING,
    GAME_BINGO,
    GAME_FAILED,
};
enum {
    POINT_PEN,
    LINE_PEN //这里很无奈做了一个line 的和point 画笔的区分 因为 这个想用 penwidth..
};
#endif // GLOBAL_H
