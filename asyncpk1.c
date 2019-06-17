#include "asyncpk1.h"

void PK1Connection(void(*reply_handler)(), 
				   void(*error_handler)(), 
				   char *host, 
				   int port, 
				   http_encryption_t encryption)
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