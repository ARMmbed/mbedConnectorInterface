/**
 * @file    mbedEndpointNetwork.cpp
 * @brief   mbed Connector Interface network low level functions and support (Ethernet, WiFi, Mesh (6LowPAN,Thread))
 * @author  Doug Anson
 * @version 1.0
 * @see     
 *
 * Copyright (c) 2014
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// Connector Endpoint
#include "mbed-connector-interface/ConnectorEndpoint.h"

// OptionsBuilder
#include "mbed-connector-interface/OptionsBuilder.h"

// Forward declarations of public functions in mbedEndpointNetwork
#include "mbed-connector-interface/mbedEndpointNetworkImpl.h"

// Enable/Disable easy-connect debugging
#define EASY_CONNECT_DEBUG			false

// Network Selection (via easy-connect library now...)
#include "easy-connect.h"
#if MBED_CONF_APP_NETWORK_INTERFACE == WIFI_ESP8266
#define NETWORK_TYPE			(char *)"WiFi-ESP8266"
#elif MBED_CONF_APP_NETWORK_INTERFACE == WIFI_ODIN
#define NETWORK_TYPE			(char *)"WiFi-ODIN"
#elif MBED_CONF_APP_NETWORK_INTERFACE == ETHERNET
#define NETWORK_TYPE			(char *)"Ethernet"
#elif MBED_CONF_APP_NETWORK_INTERFACE == CELL
#define NETWORK_TYPE			(char *)"Cellular"
#elif MBED_CONF_APP_NETWORK_INTERFACE == MESH_LOWPAN_ND
#define NETWORK_TYPE			(char *)"6LowPAN"
#elif MBED_CONF_APP_NETWORK_INTERFACE == MESH_THREAD
#define NETWORK_TYPE			(char *)"Thread"
else
#define NETWORK_TYPE			(char *)"UNKNOWN"
#endif

// Logger instance
extern Logger logger;

// endpoint instance 
static void *_endpoint_instance = NULL;

// LWIP network instance forward reference
extern NetworkInterface *__network_interface;

// main loop cycle period
static int _main_loop_iteration_wait_ms = MAIN_LOOP_WAIT_TIME_MS;

// endpoint shutdown indicator
static volatile bool _shutdown_endpoint = false;			

extern "C" {

/*********************** START LOCAL FUNCTIONS **************************/

// start shutting downt the endpoint
void start_endpoint_shutdown(void) {
	if (_shutdown_endpoint == true) {
		Connector::Endpoint *ep = (Connector::Endpoint *)_endpoint_instance;
		if (ep != NULL && ep->isRegistered() == true) {
			logger.log("mbedEndpointNetwork(%s): shutdown requested. De-registering the endpoint...",NETWORK_TYPE);
			ep->de_register_endpoint();
		}
		
		// Clean up
		if (ep != NULL) {
			delete ep;
			_endpoint_instance = NULL;
		}
	}
	
	// ready to shutdown...
	logger.log("mbedEndpointNetwork(%s): endpoint shutdown. Bye!",NETWORK_TYPE);
}
	
// setup shutdown button
#if MBED_CONF_APP_SHUTDOWN_BUTTON_ENABLE == true
#ifdef TARGET_K64F
InterruptIn shutdown_button(MBED_CONF_APP_SHUTDOWN_PIN);
#endif
void configure_deregistration_button(void) {
#ifdef TARGET_K64F
	logger.log("mbedEndpointNetwork(%s): configuring de-registration button...",NETWORK_TYPE); 
	shutdown_button.fall(&net_shutdown_endpoint);
#endif
}
#endif

// setup shutdown button
void setup_deregistration_button(void) {
#if MBED_CONF_APP_SHUTDOWN_BUTTON_ENABLE == true
	configure_deregistration_button();
#endif
}

// configure main loop parameters
void configure_main_loop_params(Connector::Endpoint *endpoint) {
	// set the initial shutdown state
	_shutdown_endpoint = false;
}

// perform an actvity in the main loop
void peform_main_loop_activity(void) {
	// empty for now...
	;
}

// begin the main loop for processing endpoint events
void begin_main_loop(void) 
{
	// DEBUG
	logger.log("mbedEndpointNetwork(%s): endpoint main loop beginning...",NETWORK_TYPE);
	
	// enter our main loop (until the shutdown condition flags it...)
	while(_shutdown_endpoint == false) {
		Thread::wait(_main_loop_iteration_wait_ms);
		peform_main_loop_activity();
	}
	
	// main loop has exited... start the endpoint shutdown...
	logger.log("mbedEndpointNetwork(%s): endpoint main loop exited. Starting endpoint shutdown...",NETWORK_TYPE);
	start_endpoint_shutdown();
}

/************************ END LOCAL FUNCTIONS ***************************/

/*********************** START PUBLIC FUNCTIONS *************************/

// get the network type
char *net_get_type() {
	return NETWORK_TYPE;
}

// shutdown the endpoint
void net_shutdown_endpoint() {
    _shutdown_endpoint = true;
}
	
// called after the endpoint is configured...
void net_plumb_network(void *p) 
{
    Connector::Endpoint *ep = NULL;
    //Connector::Options *options = NULL;
    
    // save 
    _endpoint_instance = p;
    
    // connected
    if (p != NULL) {
		ep = (Connector::Endpoint *)p;
		//options = ep->getOptions();
	}

    // connect (use easy-connect now...)
    __network_interface = easy_connect(EASY_CONNECT_DEBUG);

	// check the connection status..
	if (__network_interface != NULL) {
		// success
    	if (ep != NULL) {
    		ep->isConnected(true);
    	
    		// Debug
   			logger.log("mbedEndpointNetwork(%s): IP Address: %s",NETWORK_TYPE,__network_interface->get_ip_address());
   		}
   	}
   	else {
   		// connection error
   		__network_interface = NULL;
    	if (ep != NULL) {
    		ep->isConnected(false);
    	}
    	
    	// Debug
   		logger.log("mbedEndpointNetwork(%s): CONNECTION FAILED",NETWORK_TYPE);
   	}
}

// finalize and run the endpoint main loop
void net_finalize_and_run_endpoint_main_loop(void *p) 
{
	// cast
	Connector::Endpoint *ep = (Connector::Endpoint *)p;
	
	// Initialize our main loop... 
    configure_main_loop_params(ep);
    
    // setup the shutdown button (if enabled for a given platform...)
    setup_deregistration_button();

	// register the endpoint
	logger.log("mbedEndpointNetwork(%s): registering endpoint...",NETWORK_TYPE); 
#ifdef ENABLE_MBED_CLOUD_SUPPORT
	ep->register_endpoint(NULL,ep->getEndpointObjectList());
#else
	ep->register_endpoint(ep->getSecurityInstance(),ep->getEndpointObjectList());
#endif
	       
    // Begin the endpoint's main loop
    begin_main_loop();
}

/************************ END PUBLIC FUNCTIONS **************************/

}