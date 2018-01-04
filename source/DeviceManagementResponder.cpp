/**
 * @file    DeviceManagementResponder.cpp
 * @brief   mbed CoAP Endpoint Device Management Responder class
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
 
 // Class support
 #include "mbed-connector-interface/DeviceManagementResponder.h"
 
 // Endpoint support
 #include "mbed-connector-interface/ConnectorEndpoint.h"
 
 // DeviceManager Support
 #include "mbed-connector-interface/DeviceManager.h"

 // Reboot support
 extern "C" void NVIC_SystemReset(void);
 
 // constructor
 DeviceManagementResponder::DeviceManagementResponder(const Logger *logger,const Authenticator *authenticator) {
     this->m_logger = (Logger *)logger;
     this->m_authenticator = (Authenticator *)authenticator;
     this->m_endpoint = NULL;
}

// copy constructor
DeviceManagementResponder::DeviceManagementResponder(const DeviceManagementResponder &responder) {
    this->m_logger = responder.m_logger;
    this->m_authenticator = responder.m_authenticator;
    this->m_endpoint = responder.m_endpoint;
}

// destructor
DeviceManagementResponder::~DeviceManagementResponder() {
}

// set the Endpoint
void DeviceManagementResponder::setEndpoint(const void *ep) {
    this->m_endpoint = (void *)ep;
}

// authenticate
bool DeviceManagementResponder::authenticate(const void *challenge) {
    if (this->m_authenticator != NULL) {
        return this->m_authenticator->authenticate((void *)challenge);
    }
    return false;
}

// ACTION: deregister device
void DeviceManagementResponder::deregisterDevice(const void *challenge) {
    // check endpoint
    if (this->m_endpoint != NULL) {        
        // authenticate
        if (this->authenticate(challenge)) {
            // DEBUG
            this->m_logger->log("DeviceManagementResponder(deregister): de-registering device...");

            // act
            ((Connector::Endpoint *)this->m_endpoint)->de_register_endpoint();
        }
        else {
            // authentication failure
            this->m_logger->log("DeviceManagementResponder(deregister): authentication failed. No action taken.");
        }
    }
    else {
        // no endpoint
        this->m_logger->log("DeviceManagementResponder(deregister): No endpoint instance. No action taken.");
    }
}

// ACTION: reboot device
void DeviceManagementResponder::rebootDevice(const void *challenge) {
    // check endpoint
    if (this->m_endpoint != NULL) {
        // authenticate
        if (this->authenticate(challenge)) {
	    // deregister first...
            this->m_logger->log("DeviceManagementResponder(reboot): re-registering device...");
            ((Connector::Endpoint *)this->m_endpoint)->de_register_endpoint();

	    // then reboot...
            this->m_logger->log("DeviceManagementResponder(reboot): rebooting device...");
	    NVIC_SystemReset();
        }
        else {
            // authentication failure
            this->m_logger->log("DeviceManagementResponder(reboot): authentication failed. No action taken.");
        }
    }
    else {
        // no endpoint
        this->m_logger->log("DeviceManagementResponder(reboot): No endpoint instance. No action taken.");
    }
}
