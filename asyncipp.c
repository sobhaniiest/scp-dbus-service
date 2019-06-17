#include "asyncipp.h"

static void IPPAuthConnection(void(*reply_handler)(), 
							  void(*error_handler)(), 
							  void(*auth_handler)(), 
							  char *host, 
							  int port, 
							  http_encryption_t encryption, 
							  bool try_as_root, 
							  bool prompt_allowed)
{
	char *user = "\0";
	dict *creds = lookup_auth_info(host, port);
	if(creds != NULL)
	{
		if(strcmp(creds->hname, "root") || try_as_root)
			user = creds->hname;
		free(creds);
	}

	//IPPAuthOperation();

	IPPConnection(reply_handler, 
				  op_error_handler, 
				  op_auth_handler,
				  user, 
				  host, 
				  port,
				  encryption);
}

static void IPPConnection(void(*reply_handler)(), 
						  void(*error_handler)(), 
						  void(*auth_handler)(),
						  char *user, 
						  char *host, 
						  int port, 
						  int encryption)
{
	
}