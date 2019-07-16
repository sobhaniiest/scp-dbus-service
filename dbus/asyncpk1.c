#include "asyncpk1.h"

http_t *PK1Connection(void(*reply_handler)(), 
                      void(*error_handler)(), 
                      const char *host, 
                      int port, 
                      http_encryption_t encryption)
{
	http_t *status = IPPAuthConnection(reply_handler, 
                                       error_handler, 
                                       NULL, 
                                       host, 
                                       port, 
                                       encryption,
                                       true,
                                       true);
	if(!status && error_handler != NULL)
      error_handler();
    else if(reply_handler != NULL)
      reply_handler();
	
  	/*


  	*/

	return status;
}