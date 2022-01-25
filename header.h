#ifndef HEADER
#define HEADER

#include <pthread.h>
#include "game.h"



typedef enum {
    IN_LOBBY, 			//0
    WANNA_PLAY,			//1
    DISCONNECT,			//2
    PLAYING,			//3
} STATES;


typedef struct the_player {
    char *name;
    int socket_ID;
    STATES state;
    pthread_t client_thread;
    int connected; //0 false, 1 true
    int disconnected_time;
    int invalid_mess_number;
    int check_ping;
} Player;

typedef struct the_clients {
    int players_count;
    Player **players;
} Players;


typedef struct the_wanna_play {
    int size;
    int *socket_IDs;
}wanna_play;




typedef struct {
    size_t x_size;
    size_t y_size;
    char *board_array;
    size_t ship_alive;
} Board;

typedef struct the_game {
    Board *b1;          // Board for 1st player
    Board *b2;          // board for 2nd player
    int game_ID;        // game ID
    char *name_1;       // 1st player name
    char *name_2;       // 2nd player name
    int player_1_on_turn;
    int player_2_on_turn;
    int player_1_ships;
    int player_2_ships;


} game;

typedef struct the_games {
    int games_count;
    game **games;
}games;

/** ***************************************
 * @brief Send message to Player socket
 * @param client_socket
 * @param message
 * @param info
 *  ***************************************
 */
void send_message(int client_socket, char *message);


#endif