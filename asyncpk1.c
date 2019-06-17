#include "asyncpk1.h"

static void PK1Connection(void(*reply_handler)(), 
						  void(*error_handler)(), 
						  char *host, 
						  int port, 
						  int encryption)
{
	IPPAuthConnection(reply_handler, 
					  error_handler, 
					  NULL, 
					  host, 
					  port, 
					  encryption,
					  true,
					  true);

	
}