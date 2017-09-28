/**
 * @file    Options.h
 * @brief   mbed CoAP Options (immutable OptionsBuilder instance) class header
 * @author  Doug Anson/Chris Paola
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

#ifndef __OPTIONS_H__
#define __OPTIONS_H__

// Static Resources
#include "mbed-connector-interface/StaticResource.h"

// Dynamic Resources
#include "mbed-connector-interface/DynamicResource.h"

// include the mbed connector resource list
#include "mbed-connector-interface/mbedConnectorInterface.h"

// include the resource observer includes here so that they are not required in main.cpp
#include "mbed-connector-interface/EventQueueResourceObserver.h"
#include "mbed-connector-interface/ThreadedResourceObserver.h"
#include "mbed-connector-interface/TickerResourceObserver.h"
#include "mbed-connector-interface/MinarResourceObserver.h"

// Vector support
#include <vector>

// Resources list
typedef vector<StaticResource *> StaticResourcesList;
typedef vector<DynamicResource *> DynamicResourcesList;
typedef vector<ResourceObserver *> ResourceObserversList;

// Default CoAP Port for mbed Cloud/Connector
#define DEF_COAP_PORT		5684

// WiFi Security types (maps to wifi_security_t)
typedef enum {
	WIFI_NONE = 0,
	WIFI_WEP,
    WIFI_WPA_PERSONAL,
    WIFI_WPA2_PERSONAL
} WiFiAuthTypes;

// Connection Types for CoAP
typedef enum {
    COAP_TCP,
    COAP_UDP,
    COAP_NUM_TYPES
} CoAPConnectionTypes;

// IP Address Types
typedef enum {
    IP_ADDRESS_TYPE_IPV4,
    IP_ADDRESS_TYPE_IPV6,
    IP_ADDRESS_TYPE_NUM_TYPES
} IPAddressTypes;

namespace Connector {

/** Options class
 */
class Options
{
public:
    /**
    Default constructor
    */
    Options();

    /**
    Copy constructor
    */
    Options(const Options &opt);

    /**
    Destructor
    */
    virtual ~Options();

    /**
    Get the node lifetime
    */
    int getLifetime();

    /**
    Get the NSP domain
    */
    string getDomain();

    /**
    Get the node name
    */
    string getEndpointNodename();

    /**
    Get the node type
    */
    string getEndpointType();
    
    /**
    Get the endpoint Connector URL
    */
    char *getConnectorURL();

	/**
	Get the connector connection port from the URL
	*/
	uint16_t getConnectorPort();
	
	/**
    Get the Device Resources Object Instance
    */
    void *getDeviceResourcesObject();
    
    /**
    Get the Firmware Resources Object Instance
    */
    void *getFirmwareResourcesObject();

    /**
    Get the list of static resources
    */
    StaticResourcesList *getStaticResourceList();

    /**
    Get the list of dynamic resources
    */
    DynamicResourcesList *getDynamicResourceList();

    /**
    Get the WiFi SSID
    */
    string getWiFiSSID();

    /**
    Get the WiFi Auth Type
    */
    WiFiAuthTypes getWiFiAuthType();

    /**
    Get the WiFi Auth Key
    */
    string getWiFiAuthKey();
    
    /**
    Get the CoAP Connection Type
    */
    CoAPConnectionTypes getCoAPConnectionType();
    
     /**
    Get the IP Address Type
    */
    IPAddressTypes getIPAddressType();

    /**
    Enable/Disable Immediate Observationing
    */
    bool immedateObservationEnabled();

    /**
    Enable/Disable Observation control via GET
    */
    bool enableGETObservationControl();

	 /**
    Get the Server Certificate
    */
    uint8_t *getServerCertificate();
    
    /**
    Get the Server Certificate Size (bytes)
    */
    int getServerCertificateSize();
    
     /**
    Get the Client Certificate
    */
    uint8_t *getClientCertificate();
    
    /**
    Get the Client Certificate Size (bytes)
    */
    int getClientCertificateSize();
    
     /**
    Get the Client Key
    */
    uint8_t *getClientKey();
    
    /**
    Get the Client Key Size (bytes)
    */
    int getClientKeySize();
    
    /**
    Get Our Endpoint
    */
    void *getEndpoint();
    
protected:
    // mDS Resources
    int                     	    m_lifetime;
    string               			m_domain;
    string               			m_node_name;
    string               			m_endpoint_type;
    string							m_connector_url;

    // WiFi Resources
    string               			m_wifi_ssid;
    string               			m_wifi_auth_key;
    WiFiAuthTypes        			m_wifi_auth_type;
    
    // CoAP Connection Types
    CoAPConnectionTypes 			m_coap_connection_type;
    
    // IP Address Types
    IPAddressTypes 					m_ip_address_type;
    
    // DTLS/TLS Resources
    uint8_t 					   *m_server_cert;
    int 				    		m_server_cert_length;
    uint8_t 					   *m_client_cert;
    int 				    		m_client_cert_length;
    uint8_t 					   *m_client_key;
    int 				    		m_client_key_length;

    // CoAP behavior adjustments
    bool                		 	m_enable_immediate_observation;
    bool                 		 	m_enable_get_obs_control;

    // Endpoint Resources
    void						   *m_device_resources_object;
    void						   *m_firmware_resources_object;
    StaticResourcesList   	        m_static_resources;
    DynamicResourcesList  	        m_dynamic_resources;
    ResourceObserversList 	        m_resource_observers;
    
    // Our Endpoint
    void 						   *m_endpoint;
};

} // namespace Connector

#endif // __OPTIONS_H__