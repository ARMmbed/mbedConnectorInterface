/**
 * @file    mbedConnectorInterface.h
 * @brief   mbed Connector Inteface configuration header file
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

#ifndef __MBED_CONNECTOR_INTERFACE_H__
#define __MBED_CONNECTOR_INTERFACE_H__

// mbed
#include "mbed.h"
#include "rtos.h"

// Enable/Disable mbed Cloud support
#include "security.h"

/************** DEFAULT CONFIGURATION PARAMETERS  ************************/

// Network Selection Definitions (must match easy-connect.h)
#define ETHERNET        					1
#define WIFI_ESP8266    					2
#define MESH_LOWPAN_ND  					3
#define MESH_THREAD     					4
#define WIFI_ODIN       					5

//
// ResourceObserver type: EventQueue, Threading, or Ticker (only ONE may be uncommented)
//
//#define CONNECTOR_USING_EVENT_QUEUES  1	// currently broken... please do not use
#define CONNECTOR_USING_THREADS         1	// Threads used
//#define CONNECTOR_USING_TICKER        1	// Tickers - resource's get() method called from within ISR!!! 

// mbedOS5 uses LWIP
#define MCI_LWIP_INTERFACE                  true

// mbed-client utilizes execute parameters
#define HAS_EXECUTE_PARAMS                  true

// Default node name
#define NODE_NAME_LENGTH         			128
#define NODE_NAME                			"mbed-endpoint"
 
// Connection URL to mbed Connector (not used for R1.2+ mbed Cloud)
#if MBED_CONF_APP_NETWORK_INTERFACE == MESH_LOWPAN_ND || MBED_CONF_APP_NETWORK_INTERFACE == MESH_THREAD
	// IPv6 URL Configuration
 	#define CONNECTOR_URL 					"coaps://[2607:f0d0:2601:52::20]:5684"
 	
 	// OVERRIDE (until patched in mbed-client)
 	#define IPV4_OVERRIDE					true 
#else
 	// IPv4 URL Configuration
 	#define CONNECTOR_URL					"coap://api.connector.mbed.com:5684"
#endif
  
// Default Endpoint Type specification
#define ENDPOINT_TYPE_LENGTH 				128
#define DEFAULT_ENDPOINT_TYPE       		"mbed-device"
 
// Default Domain used
#define DOMAIN_LENGTH        				128
#define DEFAULT_DOMAIN              		"domain"
 
// Main loop iteration wait time (ms)
#define MAIN_LOOP_WAIT_TIME_MS				1000
 
// mbed-client endpoint lifetime
#define REG_LIFETIME_SEC					100  										// Lifetime of the endpoint in seconds

// default observation period (ms)
#define DEFAULT_OBS_PERIOD					0											// 0 - disabled (manual invocation), otherwise "n" in ms...

// Maximum CoAP URL length
#define MAX_CONN_URL_LENGTH					128											// Maximum Connection URL length

// DynamicResource Configuration
#define MAX_VALUE_BUFFER_LENGTH  			1024                                        // largest "value" a dynamic resource may assume as a string (max CoAP packet length)

// Logger buffer size
#define LOGGER_BUFFER_LENGTH     		 	128                                         // largest single print of a given debug line

// WiFi Configuration
#define WIFI_SSID_LENGTH         			64
#define WIFI_DEFAULT_SSID       			"changeme"
#define WIFI_AUTH_KEY_LENGTH     			64
#define WIFI_DEFAULT_AUTH_KEY    			"changeme"

// Default context address string for mbed-client
#define	CONTEXT_ADDRESS_STRING				""

// Uncomment (and implement) for Update authorization and progress handlers
//#define ENABLE_UPDATE_AUTHORIZE_HANDLER		true	// MUST go into ConnectorEndpoint.cpp and implement... otherwise, update request will not get serviced.
//#define ENALBE_UPDATE_PROGRESS_HANDLER		true
 
/************** DEFAULT CONFIGURATION PARAMETERS  ************************/

#endif // __MBED_CONNECTOR_INTERFACE___

