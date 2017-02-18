struct {
	unsigned int debug:1;
	unsigned int noquit:1;
	unsigned int noput:1;
	unsigned int listen;
	unsigned int port;
	char *password;
} server_option;

void options_default(){
	server_option.debug 	= 0;
	server_option.noquit 	= 0;
	server_option.noput 	= 0;
	server_option.listen	= 10;
	server_option.port 		= 7;
	server_option.password  = "admin";
}