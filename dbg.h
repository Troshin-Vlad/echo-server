void error(const char *);
void error_quit(const char*);
void debug(const char *,...);

/*	print error msg	*/
void error(const char *msg){
	fprintf(stderr, "[%serror%s]: %s\n", CLR_ERR, CLR_DEF, msg);
	exit(1);
}

/*	print error msg and exit	*/
void error_quit(const char *msg){
	error(msg);
	exit(1);
}

/*	print message in debug format 	*/
void debug(const char *fmt,...){

	if( server_option.debug ){
		va_list arg;
		va_start(arg, fmt);

		fprintf(stderr, "[%sdebug%s]: ", CLR_DBG, CLR_DEF);
		vfprintf(stderr, fmt, arg);
		fprintf(stderr, "\n");

		va_end(arg);
	}

}