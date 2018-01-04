/**
 * @file    DeviceManagementResponder.h
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

#ifndef __DEVICE_MANAGEMENT_RESPONDER_H__
#define __DEVICE_MANAGEMENT_RESPONDER_H__

// Logger
#include "mbed-connector-interface/Logger.h"

// Authenticator Support
#include "mbed-connector-interface/Authenticator.h"

// DeviceManagement Responder 
class DeviceManagementResponder {
    public:
        /**
        Default constructor
        @param logger input logger instance
        @param authenticator input authentication instance
        */
        DeviceManagementResponder(const Logger *logger,const Authenticator *authenticator);
        
        /**
        Copy constructor
        @param resource input the DeviceManagementResponder that is to be deep copied
        */
        DeviceManagementResponder(const DeviceManagementResponder &manager);
    
        /**
        Destructor
        */
        virtual ~DeviceManagementResponder();
        
        /**
        Set the Endpoint instance
        @param ep input the endpoint instance pointer
        */
        void setEndpoint(const void *ep);
        
        /**
        ACTION: Deregister device
        @param challenge input the input authentication challenge
        */
        virtual void deregisterDevice(const void *challenge);
        
        /**
        ACTION: Reboot device
        @param challenge input the input authentication challenge
        */
        virtual void rebootDevice(const void *challenge);
        
    private:
        Logger                      *m_logger;
        Authenticator               *m_authenticator;
        void                        *m_endpoint;
        
        bool                         authenticate(const void *challenge);
};

#endif // __DEVICE_MANAGEMENT_RESPONDER_H__
