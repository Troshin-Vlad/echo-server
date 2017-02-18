int sock_d;
int connect_d;

void closed_socket();
void receive(int,char*,int);
void server_shutdown(int);
void quit();


/*	shutdown server on signal SIGINT (Ctrl+C)	*/
void server_shutdown(int sig){
	closed_socket();
	fprintf(stderr, "\033[2Dserver:[%sshutdown%s]\n", CLR_OK, CLR_DEF);
	exit(0);
}

/*	receive message sending users	*/
void receive(int sock, char *buf, int size_buf){
	char get[256];
	int bytes;

	bytes = recv(sock, get, 255, 0);
	get[bytes] = '\0';

	snprintf(buf, size_buf, "%s", get);
}

/*	quit program	*/
void quit(){
	closed_socket();
	fprintf(stderr, "server:[%squit%s]\n", CLR_OK, CLR_DEF);
	exit(0);
}

/*	closed alway sockets	*/
void closed_socket(){
	if(sock_d)
		close(sock_d);

	if(connect_d)
		close(connect_d);
}