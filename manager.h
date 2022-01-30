#ifndef MANAGER
#define MANAGER

#include "player.h"
#include "game.h"
#include "header.h"


#define MAX_PLAYERS 20




void *check_connectivity(void *args);

/** ***************************************
 * @brief Processing of CONNECT request
 * @param array_clients 
 * @param all_games 
 * @param tok 
 * @param fd 
 * @param client 
 *  ***************************************
 */
void connect_client(Players **array_clients, games *all_games, game_finder *wanna_plays, char *tok, int fd, Player *new_client, fd_set c_s);

/** ***************************************
 * @brief Processing of PLAY request
 * @param array_clients 
 * @param wanna_plays 
 * @param all_games 
 * @param fd 
 * @param cl 
 *  ***************************************
 */
void play(Players **array_clients, game_finder **wanna_plays, games **all_games, int fd, Player **cl);

/** ***************************************
 * @brief Processing of ATTACK request
 * @param array_clients 
 * @param all_games 
 * @param fd 
 * @param cl \
 *
 */
void attack_position(Players **array_clients, games **all_games, int fd, Player **cl, char *position);



/** ***************************************
 * @brief processing of Player exit
 * @param array_clients 
 * @param wanna_plays 
 * @param all_games 
 * @param fd 
 * @param cl 
 *  ***************************************
 */
void exit_client(Players **array_clients, game_finder **wanna_plays, games **all_games, int fd, Player **cl, fd_set c_s);

/** ***************************************
 * @brief Processing message about endind of round
 * @param array_clients 
 * @param tok 
 * @param all_games 
 * @param fd 
 * @param cl 
 *  ***************************************
 */
void process_roundend_mess(Players **array_clients, char *tok, games **all_games, int fd, Player **cl);

/** ***************************************
 * @brief Processing of reconnect (shortterm)
 * @param tok 
 * @param array_clients 
 * @param wanna_plays 
 * @param all_games 
 * @param fd 
 * @param c_s 
 *  ***************************************
 */
void process_reconnect_mess(char* tok, Players **array_clients, game_finder **wanna_plays, games **all_games, int fd, fd_set c_s);

/** ***************************************
 * @brief Processing of reconnect (longterm)
 * @param array_clients 
 * @param all_games 
 * @param tok 
 * @param fd 
 * @param new_client
 *  ***************************************
 */
void reconnect_new_connect(Players **array_clients, games *all_games, char *name, int fd, Player *new_client, fd_set c_s);

/** ***************************************
 * @brief Informing player that his opponent disconnected
 * @param array_clients 
 * @param all_games 
 * @param fd 
 *  ***************************************
 */
void inform_opponent_about_disconnect(Players **array_clients, games *all_games, int fd);

/**
 * @brief Informing player that his opponent has left game
 * @param array_clients 
 * @param all_games 
 * @param fd 
 */
void inform_opponent_about_leave(Players **array_clients, games *all_games, int fd);
#endif