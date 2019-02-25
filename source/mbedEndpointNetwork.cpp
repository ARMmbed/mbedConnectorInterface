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

// MCC support
#include "mcc_common_setup.h"

// Network Selection
#define NETWORK_TYPE  (char *)"Automatic"

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
	}
	
	// ready to shutdown...
	logger.log("mbedEndpointNetwork(%s): endpoint shutdown. Bye!",NETWORK_TYPE);
	while(true) {ThisThread::sleep_for(10000);}
}
	
// setup shutdown button
#if MBED_CONF_APP_SHUTDOWN_BUTTON_ENABLE == true
InterruptIn shutdown_button(MBED_CONF_APP_SHUTDOWN_PIN);
void configure_deregistration_button(void) {
	logger.log("mbedEndpointNetwork(%s): configuring de-registration button...",NETWORK_TYPE); 
	shutdown_button.fall(&net_shutdown_endpoint);
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
		ThisThread::sleep_for(_main_loop_iteration_wait_ms);
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
    
    // save our endpoint for later...
    _endpoint_instance = p;
    
    // get our endpoint..
    if (p != NULL) {
		ep = (Connector::Endpoint *)p;
	}
    
    // call MCC to get us connected
    if (!mcc_platform_init_connection()) 
    {
    	// connect (MCC-integrated in mbedOS 5.10+)
    	if (__network_interface == NULL) 
    	{
        	__network_interface = (NetworkInterface *)mcc_platform_get_network_interface();
    	}

		// if we are connected, we are ready... 
		if (__network_interface != NULL) 
		{
			if (ep != NULL) 
			{
    			ep->isConnected(true);
    	
    			// Debug
   				logger.log("mbedEndpointNetwork(%s): IP Address: %s",NETWORK_TYPE,__network_interface->get_ip_address());
   			}
   		}
	}
	else 
	{
		logger.log("mbedEndpointNetwork(%s): Connection to network FAILED",NETWORK_TYPE);
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
	ep->register_endpoint(NULL,ep->getEndpointObjectList());
	       
    // Begin the endpoint's main loop
    begin_main_loop();
}

/************************ END PUBLIC FUNCTIONS **************************/

}
