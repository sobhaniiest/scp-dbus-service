#include "asyncconn.h"


void Async_Connection(void(*reply_handler)(), 
					 void(*error_handler)(), 
					 void(*auth_handler)(), 
					 char *host, 
					 int port, 
					 http_encryption_t encryption, 
					 bool try_as_root, 
					 bool prompt_allowed)
{
	bool use_pk,
		 destroyed = false;

	/* Decide whether to use direct IPP or PolicyKit. */

	if(!(strcmp(host,"\0")))
		host = cupsServer();

	use_pk = (((host[0] == '/') | !(strcmp(host,"localhost"))) & getuid() != 0);
	

	if(use_pk && try_as_root)
	{
		PK1Connection(subst_reply_handler,
					  subst_error_handler,
					  host,
					  port,
					  encryption);
	}
	else
	{
		IPPAuthConnection(subst_reply_handler,
						  subst_error_handler,
						  subst_auth_handler,
						  host,
						  port,
						  encryption,
						  try_as_root,
						  prompt_allowed);
	}

}

