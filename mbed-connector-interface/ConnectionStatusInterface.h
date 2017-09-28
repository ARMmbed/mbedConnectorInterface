/**
 * @file    ConnectionStatusInterface.h
 * @brief   ConnectionStatusInterface (header)
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
 
#ifndef __CONNECTION_STATUS_INTERFACE_H__
#define __CONNECTION_STATUS_INTERFACE_H__

// mbedConnectorInterface configuration
#include "mbed-connector-interface/mbedConnectorInterface.h"

// mbed support
#if defined(MCI_USE_YOTTA)
    // mbed support
    #include "mbed-drivers/mbed.h"
#else
    // mbed support
    #include "mbed.h"
#endif
 
// Connector namespace
namespace Connector  {

class ConnectionStatusInterface {
    public:
        // Default constructor
        ConnectionStatusInterface();
        
        // Copy constructor
        ConnectionStatusInterface(ConnectionStatusInterface &csi);
        
        // Destructor
        virtual ~ConnectionStatusInterface();
        
        /**
        Registered
        */
        virtual void object_registered(void *ep,void *security = NULL,void *data = NULL);
        
        /**
        Re-registered
        */
        virtual void registration_updated(void *ep,void *security = NULL,void *data = NULL);
        
        /**
        Beginning de-registration
        */
        virtual void begin_object_unregistering(void *ep);
        
        /**
        De-registered
        */
        virtual void object_unregistered(void *ep,void *data = NULL);
        
        /**
        Bootstrapped
        */
        virtual void bootstrapped(void *ep,void *data = NULL);
        
        /**
        Value Updated
        */
        virtual void value_updated(void *ep,void *data,int type);
 };
 
 } // namespace Connector
 
 #endif // __CONNECTION_STATUS_INTERFACE_H__