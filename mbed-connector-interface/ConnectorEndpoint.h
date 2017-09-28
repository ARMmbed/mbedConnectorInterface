/**
 * @file    Endpoint.h
 * @brief   mbed CoAP Endpoint base class
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

#ifndef __CONNECTOR_ENDPOINT_H__
#define __CONNECTOR_ENDPOINT_H__

// Support for Logging/Debug output
#include "mbed-connector-interface/Logger.h"

// mbed-client support
#include "mbed-client/m2minterfacefactory.h"
#include "mbed-client/m2minterfaceobserver.h"
#include "mbed-client/m2minterface.h"
#include "mbed-client/m2mobjectinstance.h"
#include "mbed-client/m2mresource.h"
#include "mbed-client/m2mdevice.h"
#include "mbed-client/m2mfirmware.h"

// Options support
#include "mbed-connector-interface/Options.h"

// ConnectionStatusInterface support
#include "mbed-connector-interface/ConnectionStatusInterface.h"

// ObjectInstanceManager support
#include "mbed-connector-interface/ObjectInstanceManager.h"

// Connector namespace
namespace Connector  {

/** Endpoint class
 */
#ifdef ENABLE_MBED_CLOUD_SUPPORT
class Endpoint : public MbedCloudClientCallback, public M2MInterfaceObserver  {
#else
class Endpoint : public M2MInterfaceObserver  {
#endif

public:
    /**
    Default Constructor
    */
    Endpoint(const Logger *logger,const Options *ob);

    /**
    Copy Constructor
    @param ob input endpoint instance to deep copy
    */
    Endpoint(const Endpoint &ep);

    /**
    Destructor
    */
    virtual ~Endpoint();

    /**
    Build out the endpoint.
    */
    void buildEndpoint();

    /**
    Plumb the lower RF network stack
    @param device_manager input optional device manager (DeviceManager type)
    @param canActAsRouterNode input boolean indicating whether this node can act as a router node or not.
    */
    static void plumbNetwork(void *device_manager = NULL,bool canActAsRouterNode = false);

    /**
    Initialize the endpoint's configuration and begin the endpoint's main even loop
    */
    static void start();
    
    /**
    Set the ConnectionStatusInterface instance
    @param csi input instance pointer to the ConnectionStatusInterface implementation to be used
    */
    static void setConnectionStatusInterface(ConnectionStatusInterface *csi);
	
	// re-register endpoint
	void re_register_endpoint();
	
	// de-register endpoint and stop scheduling
	void de_register_endpoint(void);
	
	// mbed-client : register the endpoint
	void register_endpoint(M2MSecurity *server_instance, M2MObjectList resources);
	
#ifdef ENABLE_MBED_CLOUD_SUPPORT
	//  mbed-cloud-client : object registered
    static void on_registered();

	//  mbed-cloud-client : registration updated
	static void on_registration_updated();

	//  mbed-cloud-client : object unregistered
	static void on_unregistered();
	
	// mbed-cloud-client: error
	static void on_error(int error_code);
	
	/**
	 * @brief Function for authorizing firmware downloads and reboots.
 	 * @param request The request under consideration.
 	 */
	static void update_authorize(int32_t request);
	
	/**
 	 * @brief Callback function for reporting the firmware download progress.
 	 * @param progress Received bytes.
 	 * @param total Total amount of bytes to be received.
 	 */
	static void update_progress(uint32_t progress, uint32_t total);
#endif

	//  mbed-client : object registered
    virtual void object_registered(M2MSecurity *security, const M2MServer &server);
    void object_registered(void *security = NULL,void *server = NULL);

	//  mbed-client : registration updated
	virtual void registration_updated(M2MSecurity *security, const M2MServer &server);
	void registration_updated(void *security = NULL,void *server = NULL);

	//  mbed-client : object unregistered
	virtual void object_unregistered(M2MSecurity *security = NULL);

    //  mbed-client : bootstrap done
	virtual void bootstrap_done(M2MSecurity *security = NULL);

	//  mbed-client : resource value updated
	virtual void value_updated(M2MBase *resource, M2MBase::BaseType type) ;
	
	//  mbed-client : error handler
	virtual void error(M2MInterface::Error error);
	
	// get our Options
	void setOptions(Options *options);
	
	// get our Options
	Options *getOptions();
	
#ifdef ENABLE_MBED_CLOUD_SUPPORT 
	// get our Endpoint Interface
	MbedCloudClient *getEndpointInterface();
#else	
	// get our Endpoint Interface
	M2MInterface *getEndpointInterface();
#endif

	// get our Endpoint Security
	M2MSecurity *getSecurityInstance();
	
	// get our Endpoint Object List
	M2MObjectList  getEndpointObjectList();
	
	// configure to act as router node
	void asRouterNode(bool canActAsRouterNode);
	
	// access the logger
    Logger *logger();
	
	// set the device manager
    void setDeviceManager(void *device_manager);
    
    // get the device manager
    void *getDeviceManager(void);
    
    // underlying network is connected (SET)
    void isConnected(bool connected); 
    
    // underlying network is connected (GET)
    bool isConnected();
    
    // Registered with mDC/mDS
    bool isRegistered();
    
    /**
    Set the ConnectionStatusInterface instance
    @param csi input instance pointer to the ConnectionStatusInterface implementation to be used
    */
    void setConnectionStatusInterfaceImpl(ConnectionStatusInterface *csi); 

	// Set ObjectInstanceManager
	void setObjectInstanceManager(ObjectInstanceManager *oim);
	
	// Get ObjectInstanceManager
	ObjectInstanceManager *getObjectInstanceManager();
	
private:
    Logger            			*m_logger;
    Options           			*m_options;
    bool			   			 m_canActAsRouterNode;
    bool               			 m_connected;
    bool			   			 m_registered;
    
    // mbed-client support
#ifdef ENABLE_MBED_CLOUD_SUPPORT 
	MbedCloudClient				*m_endpoint_interface;
#else
    M2MInterface				*m_endpoint_interface;
#endif
	M2MSecurity       			*m_endpoint_security;
    M2MObjectList      			 m_endpoint_object_list;
    
    // Device Manager
    void			  			*m_device_manager;
	
	// ConnectionStatusInterface implementation
	ConnectionStatusInterface	*m_csi;
	
	// ObjectInstanceManager
	ObjectInstanceManager		*m_oim;

	// create our endpoint interface
	void 			 createEndpointInterface();
	
#ifdef ENABLE_MBED_CLOUD_SUPPORT
	// mbed-cloud methods (R1.2+)
	bool			 initializeStorage();
	bool			 initializeFactoryFlow();
    void 			 createCloudEndpointInterface();
#else
	// mbed-client methods
    void 			 createConnectorEndpointInterface();
#endif
	
	// create our endpoint security instance
    M2MSecurity 	*createEndpointSecurityInstance();
    
	// DynamicResource Lookup
	DynamicResource *lookupDynamicResource(M2MBase *base);
	
	// stop underlying observers
    void stopObservations();
    
    // set our endpoint security instance
    void setSecurityInstance(M2MSecurity *security);
};

} // namespace Connector

#endif // __CONNECTOR_ENDPOINT_H__
