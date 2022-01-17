#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>
#include <arpa/inet.h>
#include <string.h>
#include <ctype.h>

#include "game.h"
#include "player.h"
#include "board.h"

#define BOOL int
#define TRUE 1
#define FALSE 0


BOOL running = TRUE;


void *socket_handler(void *);


/* Players array */
player **players = NULL;
/* Games array */
game **games = NULL;
/* Number of players created */
int players_count = 0;
/* Number of games created */
int game_count = 0;
/* Player position in array waiting for game */
int game_queue = -1;

#define MAX_PLAYERS 1000

int main(int argc , char *argv[]) {

    players = (player **) malloc(sizeof(player) * MAX_PLAYERS); // up to 1k players can join
    games = (game **)malloc(sizeof(game) * MAX_PLAYERS/2 + 1);
    int skt, res, i, inlen;
    char *message;
    int is_ipv4 = -1;
    int correct_port = -1;
    struct sockaddr_in server, client;
    if(argc != 3){
        return 0;
    }


    for(i = 0; i < strlen(argv[2]); i++){
        if(!isdigit(argv[2][i]))
        {
            printf("Port is not a digit\n");
            return EXIT_FAILURE;
        }

    }

    skt = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (skt < 0) {
        printf("Nelze inicializovat socket");
        return EXIT_FAILURE;
    }



    server.sin_family = AF_INET;


    for(i = 0; i < strlen(argv[1]); i++){
        if(!isdigit(argv[2][i]))
        {
            printf("Port is not a digit\n");
            return EXIT_FAILURE;
        }

    }

    server.sin_port = htons(atoi(argv[2]));
    if(bind(server_socket,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        puts("Bind failed");
        return EXIT_FAILURE;
    }

    puts("Bind OK");

    res = listen(skt, 5); // 5 spojeni

    if (res != 0) {
        printf("Nelze vytvorit frontu spojeni");
    }

    puts("Waiting for incoming connections...");
    inlen = sizeof(struct sockaddr_in);
    while(running)
    {
        int inskt = accept(skt, (struct sockaddr*)&client, &inlen);
        puts("Connection accepted");

        pthread_t thr;
        new_sock = malloc(1);
        *new_sock = client_socket;

        if( pthread_create(&thr , NULL ,  socket_handler , (void*) new_sock) < 0)
        {
            perror("Thread: Could not create thread");
            return EXIT_FAILURE;
        }

        pthread_detach(thr);
        puts("Handler assigned");
    }

    if (skt < 0)
    {
        perror("Accept failed");
        return 1;
    }


    return EXIT_SUCCESS;
}


/**
 * Split message from client
 * @param message
 * @param message_len
 * @return
 */
char **split_message(char *message, int *message_len) {
    char *buf = malloc(strlen(message) * sizeof(message));
    strcpy(buf, message);
    int i = 0;

    char *p = strtok(buf, "|");

    char **array = malloc(200 * sizeof(char *)); // max 200 znaku by melo stacit

    while (p != NULL) {
        array[i] = malloc(strlen(p) * sizeof(char));
        array[i++] = p;
        p = strtok(NULL, "|");
    }
    *message_len = i;

    return array;
}





int get_game_pos_by_player_id(int sock) {
    int i = 0;
    for (i = 0; i < game_count; i++) {
        if ((games[i]->player1->socket) == sock || (games[i]->player2->socket) == sock) {
            return i;
        }

    }
    return -1;
}


int add_game_to_array(player *p1, player *p2){ //TODO misto socketu se tam da player
    printf("Creating new game with game_state: %d\n", game_count);
    /* Create boards */
    board *temp1 = board_create();
    board *temp2 = board_create();
    board_set(temp1);
    sleep(1);
    board_set(temp2);
    /* Create game */
    games[game_count] = init_game(temp1, temp2, p1, p2, game_count);
    game_count++;
    return (game_count-1);
}

/**
 * Add new player to array, and if player name is in array,
 * check game state and auto reconnect this player to game
 * @param socket player socket
 * @param nickname player nickname
 * @return
 *
 */
player *add_player_to_array(int socket, char *nickname){
    char *nick = nickname;

    for (int i = 0; i < players_count; i++) {
        if (strcmp(players[i]->nick, nick) == 0) { // player nick is in array
            if (players[i]->player_state == DISCONNECTED) { // disconnected
                printf("Player is already created.");

                printf("Socket old: %d\nSocket new: %d\n", players[i]->socket, socket);

                players[i]->socket = socket;
                return players[i];
            } else {
                printf("The same nickname is in game... Please change your nick.");
                return NULL;
            }
        }
    }

    if (players_count < MAX_PLAYERS) {
        printf("Create player task: %s Socket id %d\n", nick, socket);
        printf("Player count before adding player: %d\n", players_count);
        players[players_count] = player_create(socket, nick);
        ++players_count;
        printf("Player count after adding player: %d\n", players_count);

        return players[players_count - 1];
    }
    printf("MAX players was REACHED");
    return NULL;


}

/**
 * Finding game for player by nickname
 * @param nickname
 * @return
 */
int get_game_pos_in_array_by_nickname(char *nickname, int *player_idx_game){
    int i;

    if(!nickname){
        return -1;
    }
    for(i = 0; i < game_count; i++) {
        if (games[i] != NULL) {
            if (!strcmp(games[i]->player1->nick, nickname)) {
                *player_idx_game = 1;
                return i;
            }
            if (!strcmp(games[i]->player2->nick, nickname)) {
                *player_idx_game = 2;
                return i;
            }
        }
    }

        return -1;
}

int get_player_state_by_socket_id(int socket){
    int i;

    for(i = 0; i < players_count; i++){

        if(players[i] != NULL){
            if(players[i]->socket == socket){
                return players[i]->player_state;
            }
        }

    }
    return -1;
}

int get_player_array_position_by_socket_id(int socket){
    int i;

    for(i = 0; i < players_count; i++){

        if(players[i] != NULL){
            if(players[i]->socket == socket){
                return i;
            }
        }
    }
    return -1;
}

int get_game_pos_in_array_by_player_id(int sock) {
    int i;
    for (i = 0; i < game_count; i++) {
        if (games[i] != NULL) {
            if ((games[i]->player1->socket) == sock || (games[i]->player2->socket) == sock) {
                return i;
            }

        }

        return -1;
    }
}

void *socket_handler(void *skt) {

    int sock = *(int *) skt;
    int read_size;
    int message_param_count = 0;
    char attack_result;
    char **split_msg = NULL;
    char *message, client_message[2000], server_answer[2000], attack_pos_buffer[2000];

    int attack_pos;
    char *nick = NULL;
    int socket_state = 0;
    int player_position_in_array = -1;
    int game_idx = -1;
    int game_position_in_array = -1;
    int player1_pos = 0, player2_pos = 0;
    int game_player_idx = 0; // bude nabyvat hodnoty 1 pro playera 1, a 2 pro playera 2
    int is_valid = 0;

    player *p1;          // about player

    message = "200|CONNECTED\n";

    write(sock, message, strlen(message));

    memset(client_message, 0, strlen(client_message) * sizeof(char));
    while ((read_size = recv(sock, client_message, 2000, 0)) > 0) {


        split_msg = split_message(client_message, &message_param_count);

        /**
         *
         * AFTER CONNECT -  create new player
         * RECONNECT is
         */


        if (strcmp(split_msg[1], "CONNECT") == 0 && strcmp(split_msg[2], "NICK") == 0 &&
            message_param_count == 4) { //Expected format - BTS|CONNECT|NICK|<name>

            nick = split_msg[3];

            p1 = add_player_to_array(sock, nick);


            if (!p1) { // pokud bude hrac p1 null -> nefunkcni
                write(sock, "406|NICKNAME|INVALID_NICK\n", strlen("406|NICKNAME|INVALID_NICK\n"));
                close(sock);


            }

            player_change_state(p1, CONNECTED);


            if (p1->in_game == IN_GAME) {
                game_idx = get_game_pos_in_array_by_nickname(p1->nick, &game_player_idx);

                // Kontrola aktualnich pozic hracu ve hre.
                if (game_player_idx == 1) {
                    /* Pokud reconnecting player je na tahu, nastav jeho socket na tah */
                    if (games[game_idx]->player2->socket != games[game_idx]->player_turn_socket)
                        games[game_idx]->player_turn_socket = sock;  // pokud je socket player2 roy

                }


                memset(server_answer, 0, 2000);

                strcpy(server_answer, "201|GAME_STATE|RECONNECT|");
                strcat(server_answer, get_current_board_state(games[game_idx], split_msg[3]));
                strcat(server_answer, "\n");
                printf("%s", server_answer);

            } else {
                /*                    2                     */
                printf("Nickname acknowledged, player created!\n");
                write(sock, "200|NICKNAME|OK\n", strlen("200|NICKNAME|OK\n"));
                add_player_to_array(sock, split_msg[1]);
            }

        } else {
            write(sock, "405|NICKNAME|INVALID_MESSAGE\n", strlen("405|NICKNAME|INVALID_MESSAGE\n"));
            close(sock);
        }




        /**
         * DISCONNECT
         */
        if (message_param_count > 4 && strcmp(split_msg[1], "DISCONNECT") == 0 &&
            strcmp(split_msg[2], "NICK") == 0) { //Expected format - BTS|DISCONNECT|NICK|<name>
            nick = split_msg[3];

            if (sock == game_queue) {
                printf("Removing a disconnected client %d from queue!\n", game_queue);
                game_queue = -1;
            }

            write(sock, "200|DISCONNECT|OK\n", strlen("200|DISCONNECT|OK\n"));
            printf("200|DISCONNECT|OK\n");
            players[get_player_array_position_by_socket_id(sock)] = NULL;
            close(sock);

        }






        /**
         * FIND GAME
         */
        if (message_param_count > 4 && strcmp(split_msg[1], "CONNECT") == 0 &&
            strcmp(split_msg[2], "FIND_GAME") == 0 && p1 != NULL) { //Expected format - BTS|CONNECT|FIND_GAME|



            /* Zjisti stav hráče */
            socket_state = p1->player_state;

            if (socket_state == CONNECTED) {

                /* Pokud je fronta prázdná, přidej socket do fronty */
                if (game_queue == -1) {
                    game_queue = sock;
                    printf("Player added to queue: %d\n", game_queue);
                    player_change_state(p1, IN_QUEUE);
                    write(sock, "201|ADDED_TO_QUEUE\n", strlen("201|ADDED_TO_QUEUE\n"));

                    printf("Player %d state: %d\n", sock, players[player_position_in_array]->player_state);
                } else {
                    /**
                     * New game
                    */
                    printf("New game created!\n");
                    /* Create new game */
                    /* Change player state to STATE_IN_GAME */
                    player_position_in_array = get_player_array_position_by_socket_id(sock);
                    player_change_state(players[player_position_in_array], IN_GAME);
                    player_position_in_array = get_player_array_position_by_socket_id(game_queue);
                    player_change_state(players[player_position_in_array], IN_GAME);
                    /* Create game */
                    game_position_in_array = add_game_to_array(sock, game_queue);

                    strcpy(server_answer, "200|FIND_GAME|");
                    strcat(server_answer, games[game_position_in_array]->board2->items);
                    strcat(server_answer, "\n");
                    printf("%s", server_answer);
                    write(sock, server_answer, strlen(server_answer));
                    strcpy(server_answer, "200|FIND_GAME|");
                    strcat(server_answer, games[game_position_in_array]->board1->items);
                    strcat(server_answer, "\n");
                    printf("%s", server_answer);
                    write(game_queue, server_answer, strlen(server_answer));

                    /* Print game position in array for current players */
                    //printf("Game id for players new players: %d\n", get_game_pos_in_array_by_nickname(get_player_nickname_in_array_by_socket_id(sock)));


                    /* Remove player from queue */
                    game_queue = -1;
                }


            } else {
                if (socket_state == IN_GAME || socket_state == IN_QUEUE) {
                    if (socket_state == IN_QUEUE) {
                        write(sock, "400|ALREADY_IN_QUEUE\n", strlen("400|ALREADY_IN_QUEUE\n"));
                    }
                } else {
                    close(sock);
                }

            }
        }


        /**
        *           GAME_STATE
        *
        */
        if (!strcmp(split_msg[1], "GAME_STATE")) {

            write(sock, "GAME_STATE|OK\n", strlen("GAME_STATE|OK\n"));
        }


        /**
         *
         * ATTACK
         *
         */

        if (message_param_count > 6 || strcmp(split_msg[1], "ATTACK") == 0 &&
                                       strcmp(split_msg[2], "POSITION") ==
                                       0) { //Expected format - BTS|ATTACK|POSITION|<cislo>

            socket_state = get_player_state_by_socket_id(sock);
            if (socket_state != IN_GAME || socket_state == -1) {
                /*      Pokud ne                 */
                /*                1              */
                printf("Player not in game or player not found!\n");
                strcpy(server_answer, "402|ATTACK|NOT_IN_GAME\n");
                write(sock, server_answer, strlen(server_answer));
            } else {
                game_idx = get_game_pos_in_array_by_player_id(sock);

                if (games[game_idx]->player_turn_socket == sock) {
                    /* Přidat logiku na tah */
                    attack_pos = atoi(split_msg[3]);
                    attack_result = player_turn(games[game_idx], sock, attack_pos);
                    printf("Attacked position: %d\nAttack result: %c", attack_pos, attack_result);
//
                    if (attack_result != '4') {
                        if (attack_result == '3') {
                            /*  4  */
                            printf("ATTACK acknowledged\n");
                            strcpy(attack_pos_buffer, "201|ATTACK|YOU|SHIP_HIT|");
                            snprintf(server_answer, 2000, "%s%d\n", attack_pos_buffer, attack_pos);
                            printf("Message sent: %s", server_answer);

                            if (games[game_idx]->player1->socket == sock) {
                                /* 				ATTACK|YOU                */
                                write(games[game_idx]->player1->socket, server_answer, strlen(server_answer));


                                /*              ATTACK|ENEMY              */
                                strcpy(attack_pos_buffer, "201|ATTACK|ENEMY|SHIP_HIT|");
                                snprintf(server_answer, 2000, "%s%d\n", attack_pos_buffer, attack_pos);
                                write(games[game_idx]->player2->socket, server_answer, strlen(server_answer));

                            } else {
                                /* 				ATTACK|YOU                */
                                write(games[game_idx]->player2->socket, server_answer, strlen(server_answer));


                                /*              ATTACK|ENEMY              */
                                strcpy(attack_pos_buffer, "201|ATTACK|ENEMY|SHIP_HIT|");
                                snprintf(server_answer, 2000, "%s%d\n", attack_pos_buffer, attack_pos);
                                write(games[game_idx]->player1->socket, server_answer, strlen(server_answer));

                            }

                            /*	   CHECK GAME OVER        */
                            if (is_game_over(games[game_idx])) {
                                printf("GAMEOVER");
                                strcpy(server_answer, "200|GAME_STATE|GAME_OVER\n");


                                write(games[game_idx]->player1->socket, server_answer, strlen(server_answer));
                                write(games[game_idx]->player2->socket, server_answer, strlen(server_answer));

                                /*     SET PLAYERS STATE TO CONNECTED	   */
                                player1_pos = get_player_array_position_by_socket_id(
                                        games[game_idx]->player1->socket);
                                player2_pos = get_player_array_position_by_socket_id(
                                        games[game_idx]->player2->socket);
                                players[player1_pos]->player_state = CONNECTED;
                                players[player2_pos]->player_state = CONNECTED;
                                /*         REMOVE GAME FROM ARRAY          */
                                free_game(&games[game_idx]);
                            }
                        } else {
                            /* NO_HIT */

                            printf("ATTACK acknowledged\n");
                            strcpy(attack_pos_buffer, "201|ATTACK|YOU|NO_HIT|");

                            snprintf(server_answer, 2000, "%s%d\n", attack_pos_buffer, attack_pos);
                            printf("Message sent: %s", server_answer);


                            if (sock == games[game_idx]->player1->socket) {
                                /* 				ATTACK|YOU                */
                                write(games[game_idx]->player1->socket, server_answer, strlen(server_answer));


                                /*              ATTACK|ENEMY              */
                                strcpy(attack_pos_buffer, "201|ATTACK|ENEMY|NO_HIT|");
                                snprintf(server_answer, 2000, "%s%d\n", attack_pos_buffer, attack_pos);
                                write(games[game_idx]->player2->socket, server_answer, strlen(server_answer));

                            } else {
                                /* 				ATTACK|YOU                */
                                write(games[game_idx]->player2->socket, server_answer, strlen(server_answer));


                                /*              ATTACK|ENEMY              */
                                strcpy(attack_pos_buffer, "201|ATTACK|ENEMY|NO_HIT|");
                                snprintf(server_answer, 2000, "%s%d\n", attack_pos_buffer, attack_pos);
                                write(games[game_idx]->player1->socket, server_answer, strlen(server_answer));
                            }


                        }
                    } else {


                        /*             ATTACK BAD         */
                        /*                4               */
                        printf("Player tried to attacked already attacked spot\n");
                        strcpy(server_answer, "400|ATTACK|YOU|ALREADY_ATTACKED\n");
                        write(sock, server_answer, strlen(server_answer));

                    }
                } else {
                    /*      Pokud ne, fuck off        */
                    /*                2               */
                    strcpy(server_answer, "404|ATTACK|YOU|NOT_YOUR_TURN\n");
                    printf("Player tried to fuck the system!\n");
                    write(sock, server_answer, strlen(server_answer));
                }
            }
        }

    }


    if(read_size == 0)
    {
        puts("Client disconnected");
        if(sock == game_queue){
            game_queue = -1;
        }
        /* Check if disconnected player was in game */
        if(players[get_player_array_position_by_socket_id(sock)]->player_state == IN_GAME){
            if(sock == games[get_game_pos_by_player_id(sock)]->player1){
                write(games[get_game_pos_by_player_id(sock)]->player2->socket, "409|GAME_STATE|GAME_ENDED|OPPONENT_DISCONNECTED\n", strlen("409|GAME_STATE|GAME_ENDED|OPPONENT_DISCONNECTED\n"));
            }
            else
            {
                write(games[get_game_pos_by_player_id(sock)]->player1->socket, "409|GAME_STATE|GAME_ENDED|OPPONENT_DISCONNECTED\n", strlen("409|GAME_STATE|GAME_ENDED|OPPONENT_DISCONNECTED\n"));
            }

        }
        players[get_player_array_position_by_socket_id(sock)]->player_state = CONNECTED;
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
        if(sock == game_queue){
            game_queue = -1;
            printf("Removing disconnected client from queue!\nGame queue: %d\n", game_queue);
        }
        players[get_player_array_position_by_socket_id(sock)]->player_state = CONNECTED;
        if(get_player_state_by_socket_id(sock) == IN_GAME){

            if(sock == games[get_game_pos_by_player_id(sock)]->player1){
                write(games[get_game_pos_by_player_id(sock)]->player2->socket, "409|GAME_STATE|GAME_ENDED|OPPONENT_DISCONNECTED\n", strlen("409|GAME_STATE|GAME_ENDED|OPPONENT_DISCONNECTED\n"));
            }
            else
            {
                write(games[get_game_pos_by_player_id(sock)]->player1->socket, "409|GAME_STATE|GAME_ENDED|OPPONENT_DISCONNECTED\n", strlen("409|GAME_STATE|GAME_ENDED|OPPONENT_DISCONNECTED\n"));
            }
        }
    }


    free(skt);
}
