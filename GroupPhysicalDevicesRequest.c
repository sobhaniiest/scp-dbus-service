#include "GroupPhysicalDevicesRequest.h"

void GPDRequest(GDBusConnection *connection,
                dict_devices devices,
                void(*reply_handler)(), 
                void(*error_handler)())
{
    add_hold();
    
    
}