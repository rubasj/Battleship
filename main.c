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
#include <sys/ioctl.h>
#include <fcntl.h>
#include <signal.h>

#include "game.h"
#include "player.h"
#include "manager.h"
#include "header.h"

#define STAT_CONNECT  1 //1 pro on, 0 pro off
#define PING_INTERVAL 5
#define PING_TIMEOUT 60
#define MAX_TIME_WITHOUT_RECONNECT 2
#define DEFAULT_PORT 4757


int check_conn = STAT_CONNECT;

void *socket_handler(void *);

pthread_mutex_t my_mutex;

int sent_bytes;
int received_bytes;
int MAX_MESSAGE_LENGTH = 20;





Players *cls;
games *a_g;
wanna_play *w_p;
fd_set c_s, tests, errfd;

time_t server_started_time;
time_t server_ended_time;


struct thread_function_args {
    int fd;
    char *mess;
};

void die(char *info) {
    printf("Error: %s\r\n", info);
    exit(1);
}

void print_info() {
    printf("Server started \n");
    printf("\n");
    printf("+++++ GAME: BattleShips +++++\n");
    printf("\n");
}



void print_help() {
    printf("Usage:\n");
    printf("./server [-address <address>] [-port <port>]\n");
}


void send_message(int client_socket, char *message) {
    printf("Sending to client [%d]: %s\n", client_socket, message);
    send(client_socket, message, strlen(message) * sizeof(char), 0);
    sent_bytes += (int)(strlen(message));
    return;
}


void invalid_mess_process(int fd, fd_set c_s) {
    printf("Invalid message from Player [%d]\n\n", fd);
    pthread_mutex_lock(&my_mutex);
    Player *this_client = get_player_by_socket_ID(cls, fd);

    if(this_client != NULL){

        this_client->invalid_mess_number++;

//        if(this_client->invalid_mess_number >= 3){
//            exit_client(&cls, &w_p, &a_g, fd, &this_client, c_s);
//        }
    }
    else{
        exit_client(&cls, &w_p, &a_g, fd, &this_client, c_s);
    }
    pthread_mutex_unlock(&my_mutex);
}
void sigint_exit(int sig) {
    printf("\nServer turned off\n\n");


    int i;
    for(i = 0; i < cls->players_count; i++){
        send_message(cls->players[i]->socket_ID, "[SERVEROFF]");
    }

    free(cls->players);
    free(cls);

    free(w_p->socket_IDs);
    free(w_p);

    free(a_g->games);
    free(a_g);

    exit(0);
}


int main(int argc , char *argv[]) {
    server_started_time = time(NULL);

    int port = DEFAULT_PORT;

    struct sockaddr_in my_addr, peer_addr;


    memset(&my_addr, 0, sizeof(struct sockaddr_in));

    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(port);
    my_addr.sin_addr.s_addr = INADDR_ANY;

    switch(argc){
        case 1:
            break;
        case 3:
            if (strcmp(argv[1], "-address") == 0) {
                printf("Setting address to: %s\n", argv[2]);
                my_addr.sin_addr.s_addr = inet_addr(argv[2]);
            }
            else if (strcmp(argv[1], "-port") == 0) {
                port = atoi(argv[2]);
                printf("Setting port to: %d\n", port);
                my_addr.sin_port = htons(port);
            }
            break;
        case 5:
            if (strcmp(argv[1], "-address") == 0) {
                printf("Setting address to: %s\n", argv[2]);
                my_addr.sin_addr.s_addr = inet_addr(argv[2]);
            }
            if (strcmp(argv[3], "-port") == 0) {
                port = atoi(argv[4]);
                printf("Setting port to: %d\n", port);
                my_addr.sin_port = htons(port);
            }
            if (strcmp(argv[3], "-address") == 0) {
                printf("Setting address to: %s\n", argv[4]);
                my_addr.sin_addr.s_addr = inet_addr(argv[4]);
            }
            if (strcmp(argv[1], "-port") == 0) {
                port = atoi(argv[2]);
                printf("Setting port to: %d\n", port);
                my_addr.sin_port = htons(port);
            }
            break;
        default:
            print_help();
            die("Parameters are wrong!");
            break;
    }

    players_create(&cls);
    create_games(&a_g);
    create_wanna_play(&w_p);

    int server_socket, client_socket, len_addr, a2read, return_value, fd;
    pthread_t thr;
    int cbuf_size = 1024;
    char cbuf[cbuf_size];

    sent_bytes = 0;
    received_bytes = 0;

    printf("Starting server...\n");

    /**** inicializace socketu ****/
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    if(!server_socket) {
        die("Server socket initialization failed");
    }

    /**** setsockopt ****/
    int param = 1;
    return_value = setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&param, sizeof(int));

    if(return_value == -1)
        die("Setsockopt - Error");


    /**** bind socketu ****/
    return_value = bind(server_socket, (struct sockaddr *) &my_addr, sizeof(my_addr));

    if (return_value == 0)
        printf("Bind - OK\n");
    else
        die("Bind - Error");


    /**** inicializace fronty spojení ****/
    return_value = listen(server_socket, 50);
    if(return_value == 0)
        printf("Listen - OK\n");
    else
        die("Listen - Error");


    /**** print info ****/
    print_info();
    printf("> Listening on port %d...\n\n", port);


    FD_ZERO(&c_s);
    FD_ZERO(&errfd);
    FD_SET(server_socket, &c_s);
    signal(SIGINT, sigint_exit); // ukonceni programu ctrl+c


    while(1) {
        tests = c_s;

        return_value = select(FD_SETSIZE, &tests, (fd_set *) NULL, &errfd, (struct timeval *) 0);

        for(fd = 3; fd < FD_SETSIZE; fd++) {
            if (FD_ISSET(fd, &tests)) {
                if (fd == server_socket) {
                    len_addr = sizeof(peer_addr);
                    //
                    int flags = fcntl(server_socket, F_GETFL, 0);
                    fcntl(server_socket, F_SETFL, flags | O_NONBLOCK);

                    client_socket = accept(server_socket, (struct sockaddr *) &peer_addr, &len_addr);
                    FD_SET(client_socket, &c_s);
                    printf("New Player added to set of sockets\n\n");
                }
                else {
                    int int_ioctl = ioctl(fd, FIONREAD, &a2read);
                    if (int_ioctl >= 0) {
                        Player *cl = get_player_by_socket_ID(cls, fd);

                        if (a2read > 0) {
                            if (FD_ISSET(fd, &errfd)) {
                                close(fd);
                                FD_CLR(fd, &c_s);
                            }
                            memset(cbuf, 0, cbuf_size);
                            int int_recv = recv(fd, &cbuf, cbuf_size*sizeof(char), 0);

                            if (int_recv <= 0) {
                                printf("The Player has disconnected\n");
                                close(fd);
                                FD_CLR(fd, &c_s);
                                continue;
                            }

                            if(return_value == 0){
                                close(fd);
                                FD_CLR(fd, &c_s);
                            }

                            //samotne zpracovani zpravy
                            char *new_message = NULL;
                            new_message = (char *)malloc(strlen(cbuf) + 1);
                            strncpy(new_message, cbuf, strlen(cbuf) + 1 );
                            received_bytes += (int)(strlen(new_message));

                            struct thread_function_args *infoptr = malloc(sizeof(struct thread_function_args));
                            infoptr->fd = fd;
                            infoptr->mess = new_message;

                            pthread_create(&thr, NULL, (void*) &socket_handler, (void *)infoptr);
                            pthread_detach(thr);
                            continue;
                        }
                        else {
                            close(fd);
                            FD_CLR(fd, &c_s);

                            if(cl != NULL) {
                                cl->check_ping = 0;
                                cl->connected = 0;
                                inform_opponent_about_disconnect(&cls, a_g, fd);
                            }
                        }
                    }
                    else {
                        printf("Ioctl failed and returned: %s\n", strerror(errno));
                        //
                        close(fd);
                        FD_CLR(fd, &c_s);
                    }
                }
            }
        }

    }

    return 0;


}
/*
 * Get number of tokens
 */
int get_tokens_num(char *mess) {
    int mess_len = (int)strlen(mess);
    int i;
    int count = 0;

    for(i = 0; i < mess_len; i++){
        if(mess[i] == '|')
            count++;
    }
    return (count + 1);
}

int check_input(char *input) {
    int i, j;
    int start_found = 0;
    int end_found = 0;

    for(i = 0; i < strlen(input); i++){
        if(input[i] == '['){
            start_found = 1;
            break;
        }
    }

    for(j = i; j < strlen(input); j++){
        if(input[j] == ']'){
            end_found = 1;
            break;
        }
    }


    if(start_found == 0 || end_found == 0)
        return 0;
    else {
        if((j - i - 1) > MAX_MESSAGE_LENGTH)
            return 0;
        else
            return 1;
    }
}

void *socket_handler(void *skt) {


    struct thread_function_args *args = (struct thread_function_args *)skt;
    int fd = args->fd;
    char *new_mess = args->mess;

    if (FD_ISSET(fd, &c_s) == 0){
        free(new_mess);
        free(skt);
        return NULL;
    }

    Player *client = get_player_by_socket_ID(cls, fd);


    int validate_message = check_input(new_mess);
    if(validate_message == 0)
    {
        invalid_mess_process(fd, c_s);
        free(new_mess);
        free(skt);
        return NULL;
    }

    int i, j;


    printf("%s\n", new_mess);
    for(i = 0; i < strlen(new_mess); i++){
        if(new_mess[i] == '[')
            break;
    }

    for(j = i; j < strlen(new_mess); j++) {
        if(new_mess[j] == ']')
            break;
    }

    int new_mess_len = j - i - 2;
    char *mess = NULL;
    mess = (char *)malloc(new_mess_len + 2);
    strncpy(mess, new_mess + i + 1, j - i - 1);
    mess[new_mess_len + 1] = '\0';
    free(skt);

    int mess_len = strlen(mess); //delka zpravy
    int mess_tokens_num = get_tokens_num(mess);

    char *tok = strtok(mess, "|");
    char *type_message = tok;

    free(new_mess);

    if (strcmp(type_message, "CONNECT") == 0)
    {
        if(mess_tokens_num != 2){
            invalid_mess_process(fd, c_s);
            free(mess);
            return NULL;
        }
        pthread_mutex_lock(&my_mutex);
        connect_client(&cls, a_g, w_p, tok, fd, client, c_s);
        pthread_mutex_unlock(&my_mutex);
    }
    else if (strcmp(type_message, "PLAY") == 0)
    {
        if (client != NULL) {
            if(mess_tokens_num != 1){
                invalid_mess_process(fd, c_s);
                free(mess);
                return NULL;
            }

            if (client->state == IN_LOBBY) {
                pthread_mutex_lock(&my_mutex);
                play(&cls, &w_p, &a_g, fd, &client); // osetreni kriticke sekce
                pthread_mutex_unlock(&my_mutex);
            }
            else{
                invalid_mess_process(fd, c_s);
                free(mess);
                return NULL;
            }
        }
        else{
            invalid_mess_process(fd, c_s);
        }
    }
    else if (strcmp(type_message, "ATTACK") == 0)
    {
        if (client != NULL) {
            if(mess_tokens_num != 2){           // Ocekavame PLAY|<position>
                invalid_mess_process(fd, c_s);
                free(mess);
                return NULL;
            }

            if(client->state == PLAYING) {
                pthread_mutex_lock(&my_mutex);
                attack_position(&cls, &a_g, fd, &client, tok);
                pthread_mutex_unlock(&my_mutex);
            }
            else {
                invalid_mess_process(fd, c_s);
            }
        }
        else {
            invalid_mess_process(fd, c_s);
        }
    }

    else if (strcmp(type_message, "PING") == 0) {
        if(mess_tokens_num == 1){
            pthread_mutex_lock(&my_mutex);
            get_player_by_socket_ID(cls, fd)->connected = 1;
            printf("Client [%d] pinged\n\n", fd);
            pthread_mutex_unlock(&my_mutex);
        }
    }

    else if (strcmp(type_message, "EXIT") == 0) {
        if(client != NULL) {
            if(mess_tokens_num != 1){
                invalid_mess_process(fd, c_s);
                free(mess);
                return NULL;
            }
            FD_CLR(fd, &c_s);
            pthread_mutex_lock(&my_mutex);
            exit_client(&cls, &w_p, &a_g, fd, &client, c_s);
            pthread_mutex_unlock(&my_mutex);
        }
        else {
            invalid_mess_process(fd, c_s);
        }
    }
    else if (strcmp(type_message, "ROUND") == 0 || strcmp(type_message, "ROUNDEND") == 0 || strcmp(type_message, "GAMEEND") == 0)
    {
        if(client != NULL) {
            if(mess_tokens_num != 2){
                invalid_mess_process(fd, c_s);
                free(mess);
                return NULL;
            }
            pthread_mutex_lock(&my_mutex);
            process_roundend_mess(&cls, tok, &a_g, fd, &client);
            pthread_mutex_unlock(&my_mutex);
        }
        else {
            invalid_mess_process(fd, c_s);
        }
    }
    else if (strcmp(type_message, "RECONNECT") == 0) {
        if(client != NULL) {
            if(mess_tokens_num != 2){
                invalid_mess_process(fd, c_s);
                free(mess);
                return NULL;
            }
            pthread_mutex_lock(&my_mutex);
            process_reconnect_mess(tok, &cls, &w_p, &a_g, fd, c_s);
            pthread_mutex_unlock(&my_mutex);
        }
        else {
            invalid_mess_process(fd, c_s);
        }
    }
    else if (strcmp(type_message, "PING") == 0) {
        if(mess_tokens_num == 1){
            pthread_mutex_lock(&my_mutex);
            get_player_by_socket_ID(cls, fd)->connected = 1;
            printf("Player [%d] pinged\n\n", fd);
            pthread_mutex_unlock(&my_mutex);
        }
    }
    else {
        invalid_mess_process(fd, c_s);
    }
    free(mess);
}


void *check_connectivity(void *args) {
//    if(check_conn == 0){
//        pthread_exit(0);
//        return NULL;
//    }
//
//    Player *cli = (Player*) args;
//
//    if(cli == NULL){
//        pthread_exit(0);
//        return NULL;
//    }
//
//    int socket_ID = cli->socket_ID;
//    Player *cl = NULL;
//    int disconnected_time = 0;
//    char name[30];
//    strcpy(name, cli -> name);
//
//    while(1) {
//        sleep(PING_INTERVAL);
//        cl = get_player_by_socket_ID(cls, socket_ID);
//        if (cl == NULL) {
//            printf("Client with socket ID %d is null, deleting thread\n\n", socket_ID);
//            break;
//        }
//
//        if (strcmp(cl->name, name) != 0)
//            break; //neco se pokazilo
//        if (cl->connected == 1) {
//            pthread_mutex_lock(&my_mutex);
//            cl->connected = 0;
//            pthread_mutex_unlock(&my_mutex);
//            if (disconnected_time > MAX_TIME_WITHOUT_RECONNECT) {
//                player_reconnect(&cls, socket_ID, &a_g);
//            }
//            disconnected_time = 0;
//            cl->disconnected_time = disconnected_time;
//        }
//        else {
//            if(cl->state == 1) { //pokud je ve fronte na hru, tak ho z fronty odstranim
//                remove_wanna_play(&w_p, cl -> socket_ID);
//                cl->state = 0;
//            }
//
//            if(cl->state == 3){
//                inform_opponent_about_disconnect(&cls, a_g, socket_ID);
//            }
//
//            if (cl->disconnected_time >= PING_TIMEOUT) {
//                exit_client(&cls, &w_p, &a_g, socket_ID, &cl, c_s);
//                break;
//            }
//
//            disconnected_time += PING_INTERVAL;
//            cl->disconnected_time = disconnected_time;
//        }
//
//        if(cl->check_ping == 1)
//            send_message(cl->socket_ID, "[PING]\n");
//    }
//    pthread_exit(0);
}
