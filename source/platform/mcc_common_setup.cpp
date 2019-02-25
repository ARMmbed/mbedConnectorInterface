#include "mbed.h"
#include "simple-mbed-cloud-client.h"
#include "FATFileSystem.h"
#include "LittleFileSystem.h"

// Default network interface object. Don't forget to change the WiFi SSID/password in mbed_app.json if you're using WiFi.
NetworkInterface *net = NetworkInterface::get_default_instance();

// Default block device available on the target board
BlockDevice *bd = BlockDevice::get_default_instance();

#if COMPONENT_SD || COMPONENT_NUSD
// Use FATFileSystem for SD card type blockdevices
FATFileSystem fs("fs", bd);
#else
// Use LittleFileSystem for non-SD block devices to enable wear leveling and other functions
LittleFileSystem fs("fs", bd);
#endif

extern "C" int mcc_platform_init(void) {
   printf("Initializing underlying platform...\n");
   return 0;
}

extern "C" int mcc_platform_storage_init(void) {
   // handled automatically now...
   printf("Initializing Storage...\n");
   return 0;
}

extern "C" int mcc_platform_reformat_storage(void) {
   printf("Formatting the storage...\n");
   int storage_status = StorageHelper::format(&fs, bd);
   if (storage_status != 0) {
       printf("ERROR: Failed to reformat the storage (%d).\n", storage_status);
   }
   return storage_status;
}

extern "C" int mcc_platform_sw_build_info(void) {
   // no longer used
   return 0;
}

extern "C" int mcc_platform_init_connection(void) {
    nsapi_error_t net_status = NSAPI_ERROR_NO_CONNECTION;
    while ((net_status = net->connect()) != NSAPI_ERROR_OK) {
        printf("Unable to connect to network (%d). Retrying...\n", net_status);
    }
    printf("Connected to the network successfully. IP address: %s\n", net->get_ip_address());
    return 0;
}

extern "C" void *mcc_platform_get_network_interface(void) {
   return (void *)net;
}
