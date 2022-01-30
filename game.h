#ifndef GAME
#define GAME

#include "player.h"
#include "header.h"
#define MAX_GAMES 8
#define BOARD_ITEMS 100 // rozmery 10x10

/** ***************************************
 * @brief Create game_finder
 * @param wanna_plays
 * *************************************** 
 */
void create_wanna_play(game_finder **wanna_plays);

/** ***************************************
 * @brief Add socket to game_finder
 * @param game_finder_db
 * @param socket_ID 
 *  ***************************************
 */
void add_wanna_play(game_finder **game_finder_db, int socket_ID);

/** ***************************************
 * @brief Remove Player from wanna plays
 * @param wanna_plays 
 * @param socket_ID 
 *  ***************************************
 */
void remove_wanna_play(game_finder **wanna_plays, int socket_ID);

/** ***************************************
 * @brief Create a games object
 * @param all_games 
 *  ***************************************
 */
void init_games(games **all_games);

/** ***************************************
 * @brief Create a game object
 * @param all_games 
 * @param name_1 
 * @param name_2 
 *  ***************************************
 */
game *create_game(games **all_games, char *name_1, char *name_2);

/** ***************************************
 * @brief Get the game by Player name object
 * @param array_players
 * @param all_games 
 * @param cl 
 * @return game* 
 *  ***************************************
 */
game *get_game_by_player_name(games **all_games, Player **cl);



/** ***************************************
 * @brief Checks if game is ended
 * @param array_clients 
 * @param this_game 
 *  ***************************************
 */
void check_game_end(Players **array_clients, game *this_game, games **all_games);


/** ***************************************
 * @brief remove game from game structure
 * @param pls
 * @param all_games 
 * @param game_ID 
 *  ***************************************
 */
void remove_game(games **all_games, int game_ID);


#define BOARD_SIZE 10
#define SHIP_COUNT 17
typedef unsigned int uint;

#define EMPTY_ITEM '0'
#define SHIP_ITEM '1'
#define HIT_ITEM '3'
#define MISSED_ITEM '2'
#define INVALID_HIT '4'



Board *board_create();
void board_fill(Board *bd);
void board_set(Board *bd);
char is_hit(Board *board, size_t pos);
void board_free(Board **poor);

void board_print(Board *b);

/**
 * For creating opponent array without ships.
 * @param bd
 * @return
 */
//char* get_reduced_items(Board *bd); // for opponent

#endif 