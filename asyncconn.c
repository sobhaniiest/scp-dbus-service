#include "asyncconn.h"

void(*fn_asyn)();
void(*reply_handler_asyn)();
void(*error_handler_asyn)();
void(*auth_handler_asyn)();
void(*reply_data_asyn)();
void(*error_data_asyn)();
void(*auth_data_asyn)();
bool destroyed_asyn, destroyed_cnn;
const char *host;
int port;
??? parent
bool try_as_root;
bool prompt_allowed;

void AsyncMethodCall(void(*fn)(), void(*reply_handler)(), void(*error_handler)(), void(*auth_handler)())
{
	fn_asyn = fn;
	reply_handler_asyn = reply_handler;
	error_handler_asyn = error_handler;
	auth_handler_asyn = auth_handler;
	destroyed_asyn = false;
}

void destroy_asyn()
{
	if(destroyed_asyn)
		return;

	destroyed_asyn = true;
	reply_handler_asyn = NULL;
	error_handler_asyn = NULL;
	auth_handler_asyn = NULL;
	reply_data_asyn = NULL;
	error_data_asyn = NULL;
	auth_data_asyn = NULL;
}

void run()
{

}

void reply_handler()
{
	if()
}

void error_handler()
{

}

void auth_handler()
{

}













void Connection()
{
	bool use_pk;
	reply_handler = NULL;
	error_handler = NULL;
	auth_handler = NULL;
	host = "\0";
	port = 0;
	encryption = 0;
	parent = NULL;
	try_as_root = true;
	prompt_allowed = true;

	destroyed_cnn = false;
	if(!(strcmp(host,"\0")))
	{
		host = cupsServer();
		use_pk = (((host[0] == '/') | !(strcmp(host,"localhost"))) & getuid() != 0);

	}

	if(use_pk & try_as_root)
	{

	}
	else
	{
		
	}

}

void subst_reply_handler()
{

}

void subst_error_handler()
{

}

void subst_auth_handler()
{

}

void destroy_cnn()
{

}

void make_binding()
{

}
void call_function()
{

}

void subst_reply_handler_inner()
{

}

void subst_error_handler_inner()
{

}

void subst_auth_handler_inner()
{

}

void set_auth_info()
{
	
}