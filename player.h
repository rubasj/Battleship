//
// Created by janru on 10.01.2022.
//

#ifndef BATTLESHIP_PLAYER_H
#define BATTLESHIP_PLAYER_H

#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>


#include "header.h"



/** ***************************************
 * @brief Create a Players object
 * @param array_players
 *  ***************************************
 */
void players_create(Players **array_players);

/** ***************************************
 * @brief Create a Player object
 * @param pl
 * @param name
 * @param socket_ID
 *  ***************************************
 */
void player_create(Player **pl, char *name, int socket_ID);

/** ***************************************
 * @brief Get the Player by socket ID
 * @param array_players
 * @param socket_ID
 * @return Player*
 *  ***************************************
 */
Player *get_player_by_socket_ID(Players *array_players, int socket_ID);

/** ***************************************
 * @brief Get the Player by name
 * @param array_players
 * @param name
 * @return Player*
 *  ***************************************
 */
Player *get_player_by_name(Players *array_players, char *name);

/** ***************************************
 * @brief check if name exists
 * @param array_players
 * @param name
 * @return int
 *  ***************************************
 */
int player_name_exists (Players *array_players, char *name);

/** ***************************************
 * @brief add Player to structure
 * @param array_players
 * @param name
 * @param socket_ID
 *  ***************************************
 */
void player_add(Players **array_players, char *name, int socket_ID);

/** ***************************************
 * @brief remove Player from the Players structure
 * @param array_player
 * @param socket_ID
 */
void player_remove(Players **array_player, int socket_ID);

/** ***************************************
 * @brief provide Player reconnect in the case of shor-term unavailability
 * @param array_players
 * @param socket_ID
 *  ***************************************
 */
void player_reconnect(Players **array_players, int socketID, games **all_games);
#endif //BATTLESHIP_PLAYER_H
