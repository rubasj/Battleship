#include "player.h"
#include "game.h"


void players_create(Players **array_players) {
    (*array_players) = calloc(1, sizeof(Players));
    (*array_players) -> players_count = 0;
    (*array_players) -> players = calloc(1, sizeof(Player));
    return;
}


void player_create(Player **pl, char *name, int socket_ID) {
    (*pl) = calloc(1, sizeof(Player));
    (*pl) -> name = malloc((strlen(name) + 1) * sizeof(char));
    strcpy((*pl) -> name, name);
    (*pl)->socket_ID = socket_ID;
    (*pl)->connected = 1;
    (*pl)->disconnected_time = 0;
    (*pl)->state = 0;
    (*pl)->invalid_mess_number = 0;
    (*pl)->check_ping = 1;
    return;
}


Player *get_player_by_socket_ID(Players *array_players, int socket_ID) {
    int i;
    int count = array_players -> players_count;
    int socket;
    for (i = 0; i < count; i++) {
        socket = array_players -> players[i] -> socket_ID;
        if (socket == socket_ID) {
            return array_players -> players[i];
        }
    }
    return NULL;
}


Player *get_player_by_name(Players *array_players, char *name) {
    int i;
    int count = array_players -> players_count;
    char *nam;
    for (i = 0; i < count; i++) {
        nam = array_players -> players[i] -> name;
        if (strcmp(nam, name) == 0) {
            return array_players -> players[i];
        }
    }
    return NULL;
}


int player_name_exists(Players *array_players, char *name) {
    int i;
    for (i = 0; i < array_players -> players_count; i++) {
        if (strcmp(array_players -> players[i] -> name, name) == 0) {
            return 1;
        }
    }
    return 0;
}


void player_add(Players **array_players, char *name, int socket_ID) {
    (*array_players)->players_count++;
    printf("Number of Players: %d\n", (*array_players) -> players_count);
    (*array_players) -> players = realloc((*array_players) -> players, (*array_players) -> players_count * sizeof(Player));
    Player *client = NULL;
    player_create(&client, name, socket_ID);
    (*array_players) -> players[((*array_players) -> players_count) - 1] = client;
    return;
}


void player_remove(Players **array_player, int socket_ID) {
    int i;
    int count = (*array_player)->players_count;
    int socket;
    for (i = 0; i < count; i++) {
        socket = (*array_player) -> players[i] -> socket_ID;
        if (socket == socket_ID) {
            (*array_player) -> players_count--;
            if (i < (count - 1)) {
                free((*array_player) -> players[i]);
                (*array_player) -> players[i] = (*array_player) -> players[((*array_player) -> players_count)];
            }
            (*array_player) -> players[((*array_player) -> players_count)] = NULL;
            (*array_player) -> players = realloc((*array_player) -> players, (*array_player) -> players_count * sizeof(Player));

            printf("Player removed. Number of Players: %d\n\n", (*array_player) -> players_count);
            return;
        }
    }
    return;
}

void player_reconnect(Players **array_players, int socketID, games **all_games){
//    if(get_player_by_socket_ID((*array_players), socketID) == NULL)
//        return;
//
//    int client_state = get_player_by_socket_ID((*array_players), socketID)->state;
//    Player *this_client = get_player_by_socket_ID((*array_players), socketID);
//
//
//    if(client_state == 0) {
//        char *message = "RECONNECT|1\n";
//        send_message(socketID, message);
//        return;
//    }
//    else if(client_state == 3){
//        game *this_game;
//        this_game = get_game_by_player_name(all_games, &this_client);
//        if(this_game == NULL){
//            return;
//        }
//
//        if(strcmp(this_game->name_1, get_player_by_socket_ID((*array_players), socketID)->name) == 0){
//            char message[512];
//            sprintf(message, "RECONNECT|%d|%s|%d|%s|%d|%s\n", this_game->player_1_on_turn, this_game->name_2, this_game->player_1_ships, this_game->b1->board_array, this_game->player_2_ships,
//                    get_reduced_items(this_game->b2));
//
//            char message2[100];
//            sprintf(message2, "OPP|RECONNECTED\n");
//            send_message(get_player_by_name(*array_players, this_game->name_2)->socket_ID, message2);
//        }
//        else {
//            char message[512];
//            sprintf(message, "RECONNECT|%d|%s|%d|%s|%d|%s\n", this_game->player_2_on_turn, this_game->name_1, this_game->player_2_ships, this_game->b2->board_array, this_game->player_1_ships,
//                    get_reduced_items(this_game->b1));
//            send_message(get_player_by_name(*array_players, this_game->name_2)->socket_ID, message);
//
//            char message2[100];
//            sprintf(message2, "OPP|RECONNECTED\n");
//            send_message(get_player_by_name(*array_players, this_game->name_1)->socket_ID, message2);
//        }
//    }
//    else{
//        char *message = "RECONNECT|ERR\n";
//        send_message(socketID, message);
//        player_remove(array_players, socketID);
//    }
//

}

