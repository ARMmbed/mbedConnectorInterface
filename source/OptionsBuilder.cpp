/**
 * @file    OptionsBuilder.cpp
 * @brief   mbed CoAP OptionsBuilder class implementation
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

// Class support
#include "mbed-connector-interface/OptionsBuilder.h"

// ResourceObserver support
#include "mbed-connector-interface/EventQueueResourceObserver.h"
#include "mbed-connector-interface/ThreadedResourceObserver.h"
#include "mbed-connector-interface/TickerResourceObserver.h"
#include "mbed-connector-interface/MinarResourceObserver.h"

// Connector namespace
namespace Connector {

// Constructor
OptionsBuilder::OptionsBuilder()
{
    this->m_endpoint 				= NULL;
    this->m_domain           		= DEFAULT_DOMAIN;
    this->m_endpoint_type    	 	= DEFAULT_ENDPOINT_TYPE;
    this->m_node_name        	 	= NODE_NAME;
    this->m_lifetime  				= REG_LIFETIME_SEC;
    this->m_connector_url  		 	= CONNECTOR_URL;
    this->m_server_cert				= NULL;
    this->m_server_cert_length		= 0;
    this->m_client_cert 			= NULL;
    this->m_client_cert_length		= 0;
    this->m_client_key				= NULL;
    this->m_client_key_length		= 0;
    this->m_device_resources_object = NULL;
    this->m_firmware_resources_object = NULL;
    this->m_static_resources.clear();
    this->m_dynamic_resources.clear();
    this->m_resource_observers.clear();
}

// Copy Constructor
OptionsBuilder::OptionsBuilder(const OptionsBuilder &ob)  : Options(ob)
{
	this->m_endpoint = ob.m_endpoint;
	this->m_domain = ob.m_domain;
    this->m_endpoint_type = ob.m_endpoint_type;
    this->m_node_name = ob.m_node_name;
	this->m_lifetime = ob.m_lifetime; 
	this->m_connector_url  = ob.m_connector_url;  
	this->m_server_cert	= ob.m_server_cert;
    this->m_server_cert_length = ob.m_server_cert_length;
    this->m_client_cert = ob.m_client_cert;
    this->m_client_cert_length = ob.m_client_cert_length;
    this->m_client_key = ob.m_client_key;
    this->m_client_key_length= ob.m_client_key_length;
    this->m_device_resources_object = ob.m_device_resources_object;
    this->m_firmware_resources_object = ob.m_firmware_resources_object;
    this->m_static_resources = ob.m_static_resources;
    this->m_dynamic_resources = ob.m_dynamic_resources;
    this->m_resource_observers = ob.m_resource_observers;
    this->m_wifi_ssid = ob.m_wifi_ssid;
    this->m_wifi_auth_key = ob.m_wifi_auth_key;
    this->m_wifi_auth_type = ob.m_wifi_auth_type;
    this->m_coap_connection_type = ob.m_coap_connection_type;
    this->m_ip_address_type = ob.m_ip_address_type;
    this->m_enable_immediate_observation = ob.m_enable_immediate_observation;
    this->m_enable_get_obs_control = ob.m_enable_get_obs_control;
    this->m_endpoint = ob.m_endpoint;
}

// Destructor
OptionsBuilder::~OptionsBuilder()
{
	this->m_device_resources_object = NULL;
	this->m_firmware_resources_object = NULL;
    this->m_static_resources.clear();
    this->m_dynamic_resources.clear();
    this->m_resource_observers.clear();
}

// set lifetime
OptionsBuilder &OptionsBuilder::setLifetime(int lifetime)
{
    this->m_lifetime = lifetime;
    return *this;
}

// set domain
OptionsBuilder &OptionsBuilder::setDomain(const char *domain)
{
    this->m_domain = string(domain);
    return *this;
}

// set endpoint nodename
OptionsBuilder &OptionsBuilder::setEndpointNodename(const char *node_name)
{
    this->m_node_name = string(node_name);
    return *this;
}

// set lifetime
OptionsBuilder &OptionsBuilder::setEndpointType(const char *endpoint_type)
{
    this->m_endpoint_type = string(endpoint_type);
    return *this;
}

// set Connector URL
OptionsBuilder &OptionsBuilder::setConnectorURL(const char *connector_url)
{
	if (connector_url != NULL) {
		this->m_connector_url  = string(connector_url);
	}
    return *this;
}

// add the device resources object
OptionsBuilder &OptionsBuilder::setDeviceResourcesObject(const void *device_resources_object) 
{
	if (device_resources_object != NULL) {
        this->m_device_resources_object = (void *)device_resources_object;
    }
    return *this;
}

// add the firmware resources object
OptionsBuilder &OptionsBuilder::setFirmwareResourcesObject(const void *firmware_resources_object) 
{
	if (firmware_resources_object != NULL) {
        this->m_firmware_resources_object = (void *)firmware_resources_object;
    }
    return *this;
}

// add static resource
OptionsBuilder &OptionsBuilder::addResource(const StaticResource *resource)
{
    if (resource != NULL) {
        ((StaticResource *)resource)->setOptions(this);
        this->m_static_resources.push_back((StaticResource *)resource);
    }
    return *this;
}

// add dynamic resource
OptionsBuilder &OptionsBuilder::addResource(const DynamicResource *resource)
{
    // ensure that the boolean isn't mistaken by the compiler for the obs period...
    return this->addResource(resource,DEFAULT_OBS_PERIOD,!(((DynamicResource *)resource)->implementsObservation()));
}

// add dynamic resource
OptionsBuilder &OptionsBuilder::addResource(const DynamicResource *resource,const int sleep_time)
{
    // ensure that the boolean isn't mistaken by the compiler for the obs period...
    return this->addResource(resource,sleep_time,!(((DynamicResource *)resource)->implementsObservation()));
}

// add dynamic resource
OptionsBuilder &OptionsBuilder::addResource(const DynamicResource *resource,const bool use_observer)
{
    // ensure that the boolean isn't mistaken by the compiler for the obs period...
    return this->addResource(resource,DEFAULT_OBS_PERIOD,use_observer);
}

// add dynamic resource
OptionsBuilder &OptionsBuilder::addResource(const DynamicResource *resource,const int sleep_time,const bool use_observer)
{
    if (resource != NULL) {
        this->m_dynamic_resources.push_back((DynamicResource *)resource);
        ((DynamicResource *)resource)->setOptions(this);
        ((DynamicResource *)resource)->setEndpoint((const void *)this->getEndpoint());
        if (((DynamicResource *)resource)->isObservable() == true && use_observer == true) {
        	//
        	// Establish the appropriate ResourceObserver
			//
#if defined (MCI_MINAR_SCHEDULER) 
			// mbedOS3 RTOS Minar-based Scheduler ResourceObserver
			MinarResourceObserver *observer = new MinarResourceObserver((DynamicResource *)resource,(int)sleep_time);
#endif

#ifdef CONNECTOR_USING_THREADS
			// mbedOS RTOS Thread ResourceObserver
            ThreadedResourceObserver *observer = new ThreadedResourceObserver((DynamicResource *)resource,(int)sleep_time);
#endif

#ifdef CONNECTOR_USING_TICKER
			// mbedOS RTOS Ticker ResourceObserver
            TickerResourceObserver *observer = new TickerResourceObserver((DynamicResource *)resource,(int)sleep_time);
#endif

#ifdef CONNECTOR_USING_EVENT_QUEUES
			// mbedOS RTOS EventQueue ResourceObserver
            EventQueueResourceObserver *observer = new EventQueueResourceObserver((DynamicResource *)resource,(int)sleep_time);
#endif

			// If no observer type is set in mbed-connector-interface/configuration.h (EndpointNetwork lib), then "observer" will be unresolved
            this->m_resource_observers.push_back(observer);

            // immedate observation enablement option
            if (this->immedateObservationEnabled()) {
                observer->beginObservation();
            }
        }
    }
    return *this;
}

// set WiFi SSID
OptionsBuilder &OptionsBuilder::setWiFiSSID(char *ssid)
{
    this->m_wifi_ssid = string(ssid);
    return *this;
}

// set WiFi AuthType
OptionsBuilder &OptionsBuilder::setWiFiAuthType(WiFiAuthTypes auth_type)
{
    this->m_wifi_auth_type = auth_type;
    return *this;
}

// set WiFi AuthKey
OptionsBuilder &OptionsBuilder::setWiFiAuthKey(char *auth_key)
{
    this->m_wifi_auth_key = string(auth_key);
    return *this;
}

// set the CoAP Connection Type
OptionsBuilder &OptionsBuilder::setCoAPConnectionType(CoAPConnectionTypes coap_connection_type)
{
    this->m_coap_connection_type = coap_connection_type;
    return *this;
}

// set the IP Address Type
OptionsBuilder &OptionsBuilder::setIPAddressType(IPAddressTypes ip_address_type)
{
    this->m_ip_address_type = ip_address_type;
    return *this;
}

// build out our immutable self
Options *OptionsBuilder::build()
{
    return (Options *)this;
}

// Enable/Disable immediate observationing
OptionsBuilder &OptionsBuilder::setImmedateObservationEnabled(bool enable) {
    this->m_enable_immediate_observation = enable;
    return *this;
}

// Enable/Disable GET-based control of observations
OptionsBuilder &OptionsBuilder::setEnableGETObservationControl(bool enable) {
    this->m_enable_get_obs_control = enable;
    return *this;
}

// set the server certificate
OptionsBuilder &OptionsBuilder::setServerCertificate(uint8_t *cert,int cert_size) {
    this->m_server_cert = cert;
    this->m_server_cert_length = cert_size;
	return *this;
}

// set the client certificate
OptionsBuilder &OptionsBuilder::setClientCertificate(uint8_t *cert,int cert_size) {
	this->m_client_cert = cert;
    this->m_client_cert_length = cert_size;
	return *this;
}

// set the client key
OptionsBuilder &OptionsBuilder::setClientKey(uint8_t *key,int key_size) {
	this->m_client_key = key;
    this->m_client_key_length = key_size;
	return *this;
}

// set our endpoint
void OptionsBuilder::setEndpoint(void *endpoint) {
	this->m_endpoint = endpoint;
}

} // namespace Connector
