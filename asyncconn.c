#include "asyncconn.h"

printer_uri *Async_Connection(void(*reply_handler)(), 
							  void(*error_handler)(), 
							  void(*auth_handler)(), 
							  const char *host, 
							  int port, 
							  http_encryption_t encryption, 
							  bool try_as_root, 
							  bool prompt_allowed)
{
	bool use_pk;
    printer_uri *status;

	/* Decide whether to use direct IPP or PolicyKit. */

	if(!(strcmp(host,"\0")))
		host = cupsServer();
	if(port == 0)
		port = ippPort();
	if(encryption == 0)
		encryption = cupsEncryption();

	use_pk = (((host[0] == '/') | !(strcmp(host,"localhost"))) & getuid() != 0);
	

	if(use_pk && try_as_root)
	{
		status = PK1Connection(subset_reply_handler,
							   subset_error_handler,
							   host,
							   port,
							   encryption);
		if(status && subset_reply_handler != NULL)
			subset_reply_handler();
		else if(error_handler != NULL)
			subset_error_handler();

		return status;
	}
	else
	{
		status = IPPAuthConnection(subset_reply_handler,
								   subset_error_handler,
								   subset_auth_handler,
								   host,
								   port,
								   encryption,
								   try_as_root,
								   prompt_allowed);
		if(status && subset_reply_handler != NULL)
			subset_reply_handler();
		else if(error_handler != NULL)
			subset_error_handler();

		return status;
	}
}

static void subset_reply_handler()
{

}

static void subset_error_handler()
{

}

static void subset_auth_handler()
{
	
}