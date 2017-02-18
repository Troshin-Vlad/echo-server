#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <stdarg.h>
#include <getopt.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "const.h"
#include "options.h"
#include "server.h"
#include "info.h"
#include "dbg.h"

void input(char*, char*,int);
void socket_listen(int sock, int max);
int create_socket();
void socket_bind(int);
void catch_signal();
void socket_accept(struct sockaddr_storage *client);
void server_print(const char*);

void gettm(char *, int, char *);
void gettime(char *, int);
void getdate(char *, int);

int main(int argc, char *argv[]){

	// catching signals
	catch_signal();

	// setup options default
	options_default();

	struct option long_opt[] = {
		{ "port",		1, 0, 'p' },
		{ "listen",		1, 0, 'l' },
		{ "password",	1, 0, 'P' },
		{ "noquit",		0, 0, 'q' },
		{ "noput",		0, 0, 'G' },
		{ "debug",		0, 0, 'd' },
		{ "help",		0, 0, 'h' }
	};

	char opt;
	int optid;
	while(1){

		if( (opt = getopt_long(argc, argv, "p:l:P:qGdh", long_opt, &optid))  == -1)
			break;

		switch(opt){
			case 'p':
				server_option.port = atoi(optarg);
				break;
			case 'l':
				server_option.listen = atoi(optarg);
				break;
			case 'P':
				server_option.password = optarg;
				break;
			case 'q':
				server_option.noquit = 1;
				break;
			case 'G':
				server_option.noput = 1;
				break;
			case 'd':
				server_option.debug = 1;
				break;
			case 'h':
				help();
		}
	}
	debug("password:[%s]", server_option.password);

	// created socket
	sock_d = create_socket();


	// binding socket
	debug("PORT[%i]", server_option.port);
	socket_bind(sock_d);

	// queue listen socket
	debug("max queue listen[%i]", server_option.listen);
	socket_listen(sock_d, server_option.listen );



		/* Connecting 		*/


	/*
	BEGIN: acceptings connects
	*/
	while(1){

		struct sockaddr_storage client;
		socket_accept(&client);

		int pid;
		if( (pid = fork()) == 0 ){
			
			int stat_serv = GET_MSG;
			while(1){

				char 	get[BUF_SIZE] = "",
						buf[BUF_SIZE] = "";


				receive(connect_d, buf, BUF_SIZE - 1);
				strncpy(get, buf, strlen(buf)-2);


				// parser geting string
				if( strstr(get, "server::") != NULL ){
					if( strstr(get, "server::shutdown") != NULL ){
						stat_serv = GET_PASS;
						send(connect_d, "password: ", 10, 0);
					}
				}
				else if( strstr(get, "client::") != NULL ){
					if( strstr(get, "client::quit") != NULL ){
						stat_serv = SERVER_QUIT;
						send(connect_d, "quit\n", 5, 0);
					}
				}

				// resend message
				if(stat_serv < 0){
					server_print("client quit");
					closed_socket();
					exit(0);
				}
				else if(stat_serv == GET_MSG){
					send(connect_d, buf, strlen(buf), 0);
				}
				else if(stat_serv == GET_PASS){
					if( !strcmp(get, server_option.password) ){
						fprintf(stderr, "password [%sOK%s]\n", CLR_OK, CLR_DEF);
						quit();
					}
				}

				// put string on stdin server
				if( !server_option.noput ){

					/*	print simple message or exec command	*/
					if(stat_serv == 0)
						printf("GET> %s\n", get);
					else
						printf("[exec]: %s\n", get);

					if(server_option.debug){
						for(int i = 0;i < strlen(get); i++){
							if( (i != 0) && ((i % 16) == 0) )
								putchar('\n');

							printf("%.2X ", get[i]);
						}
						putchar('\n');
					}
				}
			}// while

			exit(0);
		}// child process

		// close connect with client
		close(connect_d);
	}
	/*
	FINISH: connectings
	*/


	closed_socket();
	return 0;
}

/*	get time	*/
void gettime(char *buf, int len){
	gettm(buf, len, "%H:%M:%S");
}

/*	get date	*/
void getdate(char *buf, int len){
	gettm(buf, len, "%d.%m.%y");
}

/*	functoin for get time by specific format	*/
void gettm(char *buf, int len, char *fmttime){
	long int s_time;
	struct tm *m_time;

	s_time = time(NULL);
	m_time = localtime(&s_time);

	strftime(buf, len, fmttime, m_time);
}

// print fmt message on stdout server
void server_print(const char *msg){
	char 	time[128] = "",
			date[128] = "";

	gettime(time, 127);
	getdate(date, 127);

	fprintf(stdout, "[%s%s%s][%s%s%s]: (%s)\n", CLR_YEL, date, CLR_DEF, CLR_YEL, time, CLR_DEF, msg);
}

/*	connecting socket with client	*/
void socket_accept(struct sockaddr_storage *client){
	socklen_t size = sizeof(client);
	connect_d = accept(sock_d, (struct sockaddr*)&client, &size);
	if(connect_d == -1)
		error("connecting");
}

/*	catching signals	*/
void catch_signal(){
	if(signal( SIGINT, server_shutdown ) == SIG_ERR)
		error_quit("can't catch signal(SIGINT)");
}

/*	setup listen on socket 	*/
void socket_listen(int sock, int max){
	if( listen(sock, max) == -1 )
		error_quit("setup queue listen on socket");
}

/*	bind socket	*/
void socket_bind(int sock){
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons( server_option.port );
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if(bind(sock, (struct sockaddr*)&addr, sizeof(addr)) == -1)
		error("bind socket and port");
}

/*	create stream socket IPv4 (TCP) 	*/
int create_socket(){
	int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sock_d == -1)
		error_quit("open socket");

	int reuse = -1;
	if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse, sizeof(int)) == -1)
		error_quit("setup option reuse port");

	return sock;
}

/*	function input string	*/
void input(char *ps, char *buf, int len){
	printf("%s",ps);
	fgets(buf, len, stdin);
}