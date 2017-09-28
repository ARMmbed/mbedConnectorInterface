/**
 * @file    Utils.cpp
 * @brief   mbed CoAP Endpoint misc utils collection
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

// mbed Endpoint includes
#include "mbed-connector-interface/ConnectorEndpoint.h"
#include "mbed-connector-interface/OptionsBuilder.h"
#include "mbed-connector-interface/mbedEndpointNetwork.h"
#include "mbed-connector-interface/DeviceManager.h"
#include "mbed-connector-interface/ObjectInstanceManager.h"

// External references (defined in main.cpp)
Connector::Options *configure_endpoint(Connector::OptionsBuilder &builder);
extern Logger logger;

// Our Endpoint
Connector::Endpoint *__endpoint = NULL;

// Our Endpoint configured Options
Connector::OptionsBuilder config;
Connector::Options *options = NULL;

// initialize the Connector::Endpoint instance
void *utils_init_endpoint(bool canActAsRouterNode) {
	// alloc Endpoint
    logger.log("Endpoint: allocating endpoint instance...");
	Connector::Endpoint *ep = new Connector::Endpoint(&logger,options);
	if (ep != NULL) {
		// link to config object
		config.setEndpoint((void *)ep);
		
		// not sure if we need this anymore...
		ep->asRouterNode(canActAsRouterNode);
		
		// Add ObjectInstanceManager
		ObjectInstanceManager *oim = new ObjectInstanceManager(&logger,(void *)ep);
		ep->setObjectInstanceManager(oim);
    }
    return (void *)ep;
}

// further simplifies the endpoint main() configuration by removing the final initialization details of the endpoint...
void utils_configure_endpoint(void *p)
{   
	// our Endpoint 
	Connector::Endpoint *ep = (Connector::Endpoint *)p;
	
    // default configuration - see mbedConnectorInterface.h for definitions... 
    logger.log("Endpoint: setting defaults...");
    config.setEndpointNodename(NODE_NAME);
    config.setEndpointType(DEFAULT_ENDPOINT_TYPE);
    config.setLifetime(REG_LIFETIME_SEC);

    // WiFi defaults
    config.setWiFiSSID((char *)WIFI_DEFAULT_SSID);          			// default: changeme
    config.setWiFiAuthType(WIFI_NONE);      							// default: none
    config.setWiFiAuthKey((char *)WIFI_DEFAULT_AUTH_KEY);   			// default: changeme
    
#ifdef ENABLE_MBED_CLOUD_SUPPORT
    // Default CoAP Connection Type (mbed Cloud R1.2+)
    config.setCoAPConnectionType(COAP_TCP);								// default CoAP Connection Type - TCP
#else
    // Default CoAP Connection Type (Connector/mDS)
    config.setCoAPConnectionType(COAP_UDP);								// default CoAP Connection Type - UDP
#endif

	// Set the default IP Address Type
#if MBED_CONF_APP_NETWORK_INTERFACE == MESH_LOWPAN_ND || MBED_CONF_APP_NETWORK_INTERFACE == MESH_THREAD
    config.setIPAddressType(IP_ADDRESS_TYPE_IPV6);						// IPv6 is the default for mesh
#else    
    config.setIPAddressType(IP_ADDRESS_TYPE_IPV4);						// IPv4 is the default for all but mesh
#endif
                 
    // Establish default CoAP observation behavior
    config.setImmedateObservationEnabled(true);    

    // Establish default CoAP GET-based observation control behavior
    config.setEnableGETObservationControl(false);    

	// Device Manager installation
    DeviceManager *device_manager = (DeviceManager *)ep->getDeviceManager();
    if (device_manager != NULL) {
    	logger.log("Endpoint: installing and setting up device manager and its resources...");
    	device_manager->install((void *)ep,(void *)&config);
    }
    else {
    	logger.log("Endpoint: no device manager installed...");
    }
    
    // main.cpp can override or change any of the above defaults...
    logger.log("Endpoint: gathering configuration overrides...");
    options = configure_endpoint(config);
    
    // set our options
	ep->setOptions(options);
	
    // DONE
    logger.log("Endpoint: endpoint configuration completed.");
}

// build out the endpoint and its resources
void utils_build_endpoint(void *p)
{
    if (p != NULL) {
    	// Build the Endpoint
    	logger.log("Endpoint: building endpoint and its resources...");
		Connector::Endpoint *ep = (Connector::Endpoint *)p;
	    ep->buildEndpoint();
	}
}

// parse out the CoAP port number from the connection URL
uint16_t extract_port_from_url(char *url,uint16_t default_port) 
{
	uint16_t port = default_port;
	
	if (url != NULL && strlen(url) > 0) {
		char buffer[MAX_CONN_URL_LENGTH+1];
		char uri[MAX_CONN_URL_LENGTH+1];
		char path[MAX_CONN_URL_LENGTH+1];
		
		// initialize the buffer
		memset(buffer,0,MAX_CONN_URL_LENGTH+1);
		memset(uri,0,MAX_CONN_URL_LENGTH+1);
		memset(path,0,MAX_CONN_URL_LENGTH+1);
		int length = strlen(url); 
		
		// truncate if needed
		if (length >MAX_CONN_URL_LENGTH) length = MAX_CONN_URL_LENGTH;
		
		// make a copy...
		memcpy(buffer,url,length);
		
		// remove the forward slashes and colons
		for(int i=0;i<length;++i) {
			if (buffer[i] == ':') buffer[i] = ' ';
			if (buffer[i] == '/') buffer[i] = ' ';
			if (buffer[i] == '[') buffer[i] = ' ';
			if (buffer[i] == ']') buffer[i] = ' ';
		}
		
		// parse
		sscanf(buffer,"%s%s%d",uri,path,(int *)&port);
		
		// IPv6 kludge until we parse it properly...
		if (strcmp(uri,"coaps") == 0 && strcmp(path,"2607") == 0) {
			// IPv6 in use - so nail to Connector's IPv6 address
			strcpy(uri,"coap");
			strcpy(path,"2607:f0d0:2601:52::20");
			port = default_port;
			
			// DEBUG
			logger.log("Endpoint: Connector IPV6 uri: %s path: %s port: %d",uri,path,port);
		}
	}
	
	// DEBUG
	logger.log("Endpoint: Connector URL: %s CoAP port: %u",url,port);
	
	// return the port
	return port; 
}