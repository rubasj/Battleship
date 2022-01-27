#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/un.h>
#include <unistd.h>
#include <netinet/in.h>


#include "manager.h"


void connect_client(Players **array_clients, games *all_games, wanna_play *wanna_plays, char *tok, int fd, Player *new_client, fd_set c_s) {

    if(tok == NULL){
        // za CONNECT| není nic 
        char *message = "CONNECT|ERR\n";
		send_message(fd, message);
        return;
    }

    char *name = tok;
    int name_len = (int)strlen(name);

    if(name_len > 10){
        char *message = "CONNECT|ERR\n";
		send_message(fd, message); 
        return;
    }

    if (player_name_exists((*array_clients), name) == 0) {         // pokud jmeno neexistuje
        if (((*array_clients)->players_count) < MAX_PLAYERS) {
            if (new_client == NULL) {
                player_add(array_clients, name, fd);
				new_client = (*array_clients)->players[(*array_clients)->players_count - 1];    // ulozeni clienta na posledni misto
//				pthread_create(&(new_client -> client_thread), NULL, &check_connectivity, new_client);
				printf("Name: %s\n", (*array_clients)->players[(*array_clients)->players_count - 1]->name);
				char *message = "CONNECT|OK\n";
				send_message(fd, message); 
            }
            else {
                Player *old_client = get_player_by_socket_ID(*array_clients, fd);
                inform_opponent_about_leave(array_clients, all_games, old_client->socket_ID);
                player_remove(array_clients, old_client->socket_ID);

                player_add(array_clients, name, fd);
                new_client = get_player_by_socket_ID(*array_clients, fd);

	            new_client = (*array_clients) -> players[(*array_clients) -> players_count - 1];
	            pthread_create(&(new_client -> client_thread), NULL, &check_connectivity, new_client);
	            printf("Name: %s\n", (*array_clients) -> players[(*array_clients) -> players_count - 1] -> name);
                char *message = "CONNECT|OK\n";
				send_message(fd, message);
            }
        }
        else {
            //prilis mnoho hracu
            char *message = "CONNECT|ERR|MAX_PLAYERS\n";
			send_message(fd, message); 
        }        
    }
    else { // jmeno existuje
        Player *old_client = get_player_by_name(*array_clients, name);
        if (old_client->connected == 0) {
            // takovy hrac existuje a neni pripojeny
            if(old_client -> state != 3){
                if(old_client != NULL){
                    player_remove(array_clients, old_client->socket_ID);
                }
                player_add(array_clients, name, fd);
                new_client = get_player_by_socket_ID(*array_clients, fd);

	            new_client = (*array_clients) -> players[(*array_clients) -> players_count - 1];
	            pthread_create(&(new_client -> client_thread), NULL, &check_connectivity, new_client); 
	            printf("Name: %s\n", (*array_clients) -> players[(*array_clients) -> players_count - 1] -> name);
                char *message = "CONNECT|OK\n";
				send_message(fd, message);
            }
            process_reconnect_new_connect(array_clients, all_games, name, fd, new_client, c_s);	// reconnect
        }
        else {
            //takvovy hrac jiz existuje
            char *message = "CONNECT|ERR|ALREADY_USED\n";
            send_message(fd, message); 
        }
    }

}

void play(Players **array_clients, wanna_play **wanna_plays, games **all_games, int fd, Player **cl){
    if((*cl)->state == WANNA_PLAY) {
        send_message(fd, "PLAY|ERR\n");
        return;
    }
    else if((*cl)->state == PLAYING) {
        send_message(fd, "PLAY|ERR\n");
        return;
    }

    add_wanna_play(wanna_plays, fd);
    (*cl) -> state = WANNA_PLAY;

    send_message(fd, "PLAY|WAIT\n");

    //ve fronte je mene nez 2 hraci
    if (((*wanna_plays) -> size) < 2)
        return;

    int socket_ID_1 = fd;
    int socket_ID_2; // druhy hrac
    do {
			socket_ID_2 = (*wanna_plays) -> socket_IDs[rand() % ((*wanna_plays) -> size)];// nahodne vzbrani protihrace
	}
	while(socket_ID_2 == socket_ID_1);

    remove_wanna_play(wanna_plays, socket_ID_1);
	remove_wanna_play(wanna_plays, socket_ID_2);

    Player *player_1 = get_player_by_socket_ID(*array_clients, socket_ID_1);
    Player *player_2 = get_player_by_socket_ID(*array_clients, socket_ID_2);

    game *this_game = create_game(all_games, player_1->name, player_2->name); // vytvoreni hry

    player_1->state = 3;    // nastaveni state na PLAYING
    player_2->state = 3;


    printf("Game: [%d] On turn: %s\n", this_game->game_ID, player_1->name);

    char message_1[512];
    char message_2[512];


    // posilame jmeno oponenta a vygenerovanou board           opponent name
    sprintf(message_1, "PLAY|START|%s|%s|%d|%d|%d\n", player_2->name, this_game->b1->board_array, this_game->player_1_on_turn, this_game->b1->ship_alive, this_game->b2->ship_alive);
    printf("Player name: %s, board: \n",player_1->name);
    board_print(this_game->b1);
    sprintf(message_2, "PLAY|START|%s|%s|%d|%d|%d\n", player_1->name, this_game->b2->board_array, this_game->player_2_on_turn, this_game->b2->ship_alive, this_game->b1->ship_alive);       // OK, funkcni
    printf("Player name: %s, board: \n",player_2->name);
    board_print(this_game->b2);


    send_message(socket_ID_1, message_1);
    send_message(socket_ID_2, message_2);


    return;
}

void attack_position(Players **array_clients, games **all_games, int fd, Player **cl, char *position) {

    if (!position || !cl || !*cl || !array_clients || !*array_clients) {
        return;
    }

    int att_position = atoi(position);          // OK

    if (att_position > 100 || att_position < 0) {
        char *message = "ATTACK|ERR|POSITION\n";
        send_message(fd, message);
        return;
    }

    game *this_game = get_game_by_player_name(*array_clients, all_games, cl);

    Player *player_1 = get_player_by_name(*array_clients, this_game->name_1);
    Player *player_2 = get_player_by_name(*array_clients, this_game->name_2);

    if (this_game->player_1_on_turn == 1) {
        printf("Utoci player1> jmeno: %s\n", player_1->name);
    } else {
        printf("Utoci player1> jmeno: %s\n", player_2->name);
    }
    // hrac co je na rade smi hrat jinak posle err
    // pokud hrac nebude -> odesle err

    if(strcmp((*cl)->name, player_1->name) == 0)
    {           // pokud utoci player 1
        if(this_game->player_1_on_turn == 0){
            char *message = "ATTACK|NOT_YOUR_TURN\n";
			send_message(fd, message);
            return;
        }

        char att_res = is_hit(this_game->b2, att_position);         // Utok na board2 hrace Player2

        if (att_res == HIT_ITEM) {
            char message_1[100];
            char message_2[100];
            this_game->player_1_on_turn = 0;
            this_game->player_2_on_turn = 1;
            printf("Player %s hit opponent's ship in pos: %d\n", player_1->name, att_position);
            sprintf(message_1, "ATTACK|HIT|OPP|%d|%d|%d|%d\n", att_position, this_game->b1->ship_alive, this_game->player_1_on_turn, this_game->b2->ship_alive); // Na P1 opponent se zmeni pole na cervenou
            sprintf(message_2, "ATTACK|HIT|YOU|%d|%d|%d|%d\n", att_position, this_game->b2->ship_alive, this_game->player_2_on_turn, this_game->b1->ship_alive); // Na P2 se zmeni na vlastnim poli pole na cervenou


            send_message(player_1->socket_ID, message_1);
            send_message(player_2->socket_ID, message_2);

        } else if (att_res == MISSED_ITEM) {
            char message_1[100];
            char message_2[100];

            this_game->player_1_on_turn = 0;
            this_game->player_2_on_turn = 1;

            printf("Player %s missed opponent's field in pos: %d\n", player_1->name, att_position);
            sprintf(message_1, "ATTACK|MISS|OPP|%d|%d|%d\n", att_position, this_game->player_1_on_turn, this_game->b2->ship_alive); // pro hrace, jenz zautocil
            sprintf(message_2, "ATTACK|MISS|YOU|%d|%d|%d\n", att_position, this_game->player_2_on_turn, this_game->b1->ship_alive);

            send_message(player_1->socket_ID, message_1);
            send_message(player_2->socket_ID, message_2);

        }
        else {          // pokud klikne na pole ktere je jiz trefene
            char message_1[100];

            printf("Player %s clicked on wrong field in pos: %d\n", player_1->name, att_position);
            sprintf(message_1, "ATTACK|INV"); // pouze pro hrace, jenz zautocil


            send_message(fd, message_1);
        }




    }
    else // pokud utoci player 2
    {
        if(this_game->player_2_on_turn == 0){
            char *message = "ATTACK|NOT_YOUR_TURN\n";
            send_message(fd, message);
            return;
        }

        char att_res = is_hit(this_game->b1, att_position);

        if (att_res == HIT_ITEM) {
            char message_1[100];
            char message_2[100];

            this_game->player_1_on_turn = 1;
            this_game->player_2_on_turn = 0;

            printf("Player %s hit opponent's ship in pos: %d\n", player_2->name, att_position);
            sprintf(message_1, "ATTACK|HIT|OPP|%d|%d|%d|%d\n", att_position, this_game->b2->ship_alive, this_game->player_2_on_turn, this_game->b1->ship_alive); // pro hrace, jenz zautocil
            sprintf(message_2, "ATTACK|HIT|YOU|%d|%d|%d|%d\n", att_position, this_game->b1->ship_alive, this_game->player_1_on_turn, this_game->b2->ship_alive);


            send_message(fd, message_1);
            send_message(player_1->socket_ID, message_2);

        } else if (att_res == MISSED_ITEM) {
            char message_1[100];
            char message_2[100];

            this_game->player_1_on_turn = 1;
            this_game->player_2_on_turn = 0;
            printf("Player %s missed opponent's field in pos: %d\n", player_2->name, att_position);
            sprintf(message_1, "ATTACK|MISS|OPP|%d|%d|%d\n", att_position, this_game->player_2_on_turn, this_game->b1->ship_alive); // pro hrace, jenz zautocil
            sprintf(message_2, "ATTACK|MISS|YOU|%d|%d|%d\n", att_position, this_game->player_1_on_turn, this_game->b2->ship_alive);

            send_message(fd, message_1);
            send_message(player_1->socket_ID, message_2);

        }
        else {          // pokud klikne na pole ktere je jiz trefene
            char message_1[100];

            printf("Player %s clicked on wrong field in pos: %d\n", player_2->name, att_position);
            sprintf(message_1, "ATTACK|INV|OPP"); // pouze pro hrace, jenz zautocil


            send_message(fd, message_1);
        }
    }
    printf("Player %s ship alive: %d\n Player %s ship alive: %d\n", player_1->name, this_game->b1->ship_alive, player_2->name, this_game->b2->ship_alive);
    check_game_end(array_clients, this_game, all_games);
}



void exit_client(Players **array_clients, wanna_play **wanna_plays, games **all_games, int fd, Player **cl, fd_set c_s){
    Player *my_cl = get_player_by_socket_ID(*array_clients, fd);

    if(my_cl != NULL)
        cl = &my_cl;
    else{
        char *message = "DISCONNECT\n";
	    send_message(fd, message);
        close(fd);
        FD_CLR(fd, &c_s);
        return;
    }

    if((*cl)->state == WANNA_PLAY) {
        remove_wanna_play(wanna_plays, fd);
        (*cl)->socket_ID == 0;
    }

    //klient je ve hre
    else if((*cl)->state == 3) {
        int game_id = get_game_by_player_name(*array_clients, all_games, cl)->game_ID;
        
        char *name1 = get_game_by_player_name(*array_clients, all_games, cl)->name_1;
        char *name2 = get_game_by_player_name(*array_clients, all_games, cl)->name_2;

        char *second_player_name;

        if(strcmp(get_player_by_name(*array_clients, name1)->name, (*cl)->name) == 0) {
            second_player_name = name2;
        }
        else {
            second_player_name = name1;
        }

        int second_player_socketID = get_player_by_name(*array_clients, second_player_name)->socket_ID;
        char *message = "LEAVE\n";
		send_message(second_player_socketID, message);

        remove_game(all_games, game_id);

        get_player_by_name(*array_clients, name1)->state = IN_LOBBY;
        get_player_by_name(*array_clients, name2)->state = IN_LOBBY;
    }

    char *message_2 = "DISCONNECT\n";
	send_message(fd, message_2);

    //odstraneni klienta a ukonceni spojeni
    close(fd); 
    FD_CLR(fd, &c_s);
    player_remove(array_clients, fd);
}

void process_roundend_mess(Players **array_clients, char *tok, games **all_games, int fd, Player **cl){
    tok = strtok(NULL, "|");
    char *result = tok;

    if(strcmp(result, "OK") == 0){
        return;
    }
    else if(strcmp(result, "ERR") == 0){
        player_reconnect(array_clients, fd, all_games);
    }
    return;
}

void process_reconnect_mess(char* tok, Players **array_clients, wanna_play **wanna_plays, games **all_games, int fd, fd_set c_s){

    char *result = tok;

    Player *cl = get_player_by_socket_ID(*array_clients, fd);

    if(strcmp(result, "OK") == 0){
        return;
    }
    else if(strcmp(result, "ERR") == 0){
        exit_client(array_clients, wanna_plays, all_games, fd, &cl, c_s);
    }
    return;
}

void process_reconnect_new_connect(Players **array_clients, games *all_games, char *name, int fd, Player *new_client, fd_set c_s){
 
    Player *old_client = get_player_by_name(*array_clients, name);
    
    if(old_client == NULL)
        return;

    int old_client_fd = old_client->socket_ID;
      

    int player_num = 0;
    game *this_game = get_game_by_player_name(*array_clients, &all_games, &old_client);

    if(this_game == NULL)
        return;

    //najdu si jestli jde o playera 1 nebo 2
    if(strcmp(this_game->name_1, name) == 0){
        player_num = 1;
    }
    else{
        player_num = 2;
    }

    pthread_cancel((old_client->client_thread));
    player_remove(array_clients, old_client_fd);

    //vytvoreni add noveho klienta
    player_add(array_clients, name, fd);
    new_client = get_player_by_socket_ID(*array_clients, fd);

	new_client = (*array_clients) -> players[(*array_clients) -> players_count - 1];
	pthread_create(&(new_client -> client_thread), NULL, &check_connectivity, new_client);  
	printf("Name: %s\n", (*array_clients) -> players[(*array_clients) -> players_count - 1] -> name);

    //ve hre udealm zmenu pointeru jmena ze stareho na noveho klienta
    if(player_num == 1){
        this_game->name_1 = new_client->name;
        new_client->state = 3;
        new_client->connected = 1;

        char message[512];
		sprintf(message, "RECONNECT|%d|%s|%d|%s|%d|%s\n", this_game->player_1_on_turn, this_game->name_2, this_game->player_1_ships, this_game->b1->board_array, this_game->player_2_ships,
                get_reduced_items(this_game->b2));
        send_message(fd, message);

        char message2[256];
		sprintf(message2, "OPP|RECONNECTED\n");
        send_message(get_player_by_name(*array_clients, this_game->name_2)->socket_ID, message2);
    }
    else{
        this_game->name_2 = new_client->name;
        new_client->state = 3;
        new_client->connected = 1;

        char message[512];
		sprintf(message, "RECONNECT|%d|%s|%d|%s|%d|%s\n", this_game->player_2_on_turn, this_game->name_1, this_game->player_2_ships, this_game->b2->board_array, this_game->player_1_ships,
                get_reduced_items(this_game->b1));
        send_message(fd, message);

        char message2[256];
		sprintf(message2, "OPP|RECONNECTED\n");
        send_message(get_player_by_name(*array_clients, this_game->name_1)->socket_ID, message2);
    }

    return;
}


void inform_opponent_about_disconnect(Players **array_clients, games *all_games, int fd) {
    Player *this_client = get_player_by_socket_ID(*array_clients, fd);
    
    if(this_client == NULL)
        return;

    game* this_game = get_game_by_player_name(*array_clients, &all_games, &this_client);

    if(this_game == NULL)
        return;

    if(strcmp(this_game->name_1, this_client->name) == 0){
        char *message = "OPP|DISCONNECTED\n";
        send_message(get_player_by_name(*array_clients, this_game->name_2)->socket_ID, message);
    }
    else{
        char *message = "OPP|DISCONNECTED\n";
        send_message(get_player_by_name(*array_clients, this_game->name_1)->socket_ID, message);
    }
}


void inform_opponent_about_leave(Players **array_clients, games *all_games, int fd) {
    Player *this_client = get_player_by_socket_ID(*array_clients, fd);
    
    if(this_client == NULL)
        return;

    game* this_game = get_game_by_player_name(*array_clients, &all_games, &this_client);

    if(this_game == NULL)
        return;

    if(strcmp(this_game->name_1, this_client->name) == 0){
        char *message = "LEAVE\n";
        send_message(get_player_by_name(*array_clients, this_game->name_2)->socket_ID, message);
        get_player_by_name(*array_clients, this_game->name_2)->state = 0;
    }
    else{
        char *message = "LEAVE\n";
        send_message(get_player_by_name(*array_clients, this_game->name_1)->socket_ID, message);
        get_player_by_name(*array_clients, this_game->name_1)->state = 0;
    }

    remove_game(&all_games, this_game->game_ID);
}