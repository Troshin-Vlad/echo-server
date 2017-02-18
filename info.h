void help();
void version();

/*	print help page		*/
void help(){
	printf("Usage: server-echo [options]\n");
	printf("  -p, --port <port>   : setup number port\n");
	printf("  -l, --listen <count>: setup max listener\n");
	printf("  -G, --noput         : no print receive string\n");
	printf("  -d, --debug         : print debug information\n");
	printf("  -q, --noquit        : no quit if receive exit string\n");
	printf("  -h, --help          : print this help page\n");
	exit(0);
}

/*	print version program	*/
void version(){
	printf("echo-server: %.2f\n", VERSION);
	exit(0);
}