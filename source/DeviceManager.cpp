/**
 * @file    DeviceManager.cpp
 * @brief   mbed CoAP Endpoint Device Management class
 * @author  Doug Anson
 * @version 1.0
 * @see
 *
 * Copyright (c) 2016
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
 
 // configuration
 #include "mbed-connector-interface/mbedConnectorInterface.h"

 // BaseClass 
 #include "mbed-connector-interface/DeviceManager.h"
 
 // Endpoint Class
 #include "mbed-connector-interface/ConnectorEndpoint.h"
 
 // Options Builder
 #include "mbed-connector-interface/OptionsBuilder.h"
 
 // Device Management Responder
 #include "mbed-connector-interface/DeviceManagementResponder.h"
 
// Constructor
DeviceManager::DeviceManager(const Logger *logger,const void *dm_responder,const char *endpoint_type) {
    // record the data management responder if we have one
    this->m_dm_responder = (void *)dm_responder;
    
    // save off for later
    this->m_logger = (Logger *)logger;
    this->m_deregister_resource = NULL;
    this->m_reboot_resource = NULL;
    this->m_endpoint = NULL;
    this->m_config = NULL;
    this->m_endpoint_type = (char *)endpoint_type;
}

// Copy constructor
DeviceManager::DeviceManager(const DeviceManager &manager) {
    this->m_dm_responder = manager.m_dm_responder;
    this->m_logger = manager.m_logger;
    this->m_endpoint = manager.m_endpoint;
    this->m_config = manager.m_config;
    this->m_deregister_resource = manager.m_deregister_resource;
    this->m_reboot_resource = manager.m_reboot_resource;
    this->m_endpoint_type = manager.m_endpoint_type;
}

// Destructor
DeviceManager::~DeviceManager() {
}

// bind device deregistration resource
void DeviceManager::bindDeRegisterResource() {
    // our Endpoint configuration
    Connector::OptionsBuilder *cfg = (Connector::OptionsBuilder *)this->m_config;
    
    // DeRegistration Resource
    this->m_deregister_resource = new DeviceDeRegisterResource(this->m_logger,LWM2M_DM_OBJ_ID,LWM2M_DEV_DEREGISTER_ID,this->m_dm_responder);
    if (this->m_deregister_resource != NULL) {
        cfg->addResource(this->m_deregister_resource);              // de-registration action resource added
    }
}

// bind device reboot resource
void DeviceManager::bindRebootResource() {
    // our Endpoint configuration
    Connector::OptionsBuilder *cfg = (Connector::OptionsBuilder *)this->m_config;
   
    // Reboot Resource
    this->m_reboot_resource = new DeviceRebootResource(this->m_logger,LWM2M_DM_OBJ_ID,LWM2M_DEV_REBOOT_ID,this->m_dm_responder);
    if (this->m_reboot_resource != NULL) {
        cfg->addResource(this->m_reboot_resource);              // reboot action resource added
    }
}

// setup the Device Manager
void DeviceManager::bind() {
    // Bind our device DeRegistation resource
    this->bindDeRegisterResource();

    // Bind our device reboot resource
    this->bindRebootResource();
}

// Install the device manager into the Connector Endpoint
void DeviceManager::install(const void *endpoint,const void *config) {
    // record the configuration
    this->m_config = (void *)config;
    
    // record the endpoint
    this->m_endpoint = (void *)endpoint;
    
    // set the endpoint type
    Connector::OptionsBuilder *cfg = (Connector::OptionsBuilder *)this->m_config;
    if (cfg != NULL) {
        // set our endpoint type
        cfg->setEndpointType(this->m_endpoint_type);
    }
            
    // establish connection to our responder
    if (this->m_dm_responder != NULL) {
        ((DeviceManagementResponder *)this->m_dm_responder)->setEndpoint(this->m_endpoint);
    }        
}

// get our device management responder
void *DeviceManager::getResponder() { 
    return this->m_dm_responder; 
}
