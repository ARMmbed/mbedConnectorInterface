/**
 * @file    DeviceManager.h
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

#ifndef __DEVICE_MANAGER_H__
#define __DEVICE_MANAGER_H__

// Action Resource: Device DeRegistration
#include "mbed-connector-interface/DeviceDeRegisterResource.h"

// Action Resource: Device Reboot
#include "mbed-connector-interface/DeviceRebootResource.h"

// LWM2M Object ID for our DM resources
#define LWM2M_DM_OBJ_ID                     "86"

// LWM2M Device DeRegistration Resource ID
#define LWM2M_DEV_DEREGISTER_ID             "1"

// LWM2M Device Reboot Resource ID
#define LWM2M_DEV_REBOOT_ID                 "2"

/** DeviceManager is the endpoint management class
 */
class DeviceManager
{    
    public:
        /**
        Default constructor
        @param logger input logger instance
        @param dm_responder input data management responder/processor
        @param mfg input endpoint manufacturer
        @param dev_type input the endpoint type
        @param model input the model of the endpoint
        @param serial input the serial of the endpoint
        @param fw_vers input the current firmware version
        @param hw_vers input the current hardware version
        @param sw_vers input the current software version
        */
        DeviceManager(const Logger *logger,const void *dm_responder=NULL,const char *endpoint_type="mbed-enpoint");
        
        /**
        Copy constructor
        @param resource input the DeviceManager that is to be deep copied
        */
        DeviceManager(const DeviceManager &manager);
    
        /**
        Destructor
        */
        virtual ~DeviceManager();
        
        /**
        install the device manager into the endpoint
        @param endpoint input the endpoint instance
        @param config input the endpoint configuration instance
        */
        void install(const void *endpoint,const void *config);
        
        /**
        get the DeviceManagementResponder
        @return the DeviceManagementResponder or NULL
        */
        void *getResponder();
        
        // bind our device resources   
        void bind();
        
    private:
        Logger                              *m_logger;
        void                                *m_endpoint;
        void                                *m_config;
        void                                *m_dm_responder;
        char			            *m_endpoint_type;
        
        // Reboot Resource
	DeviceRebootResource	            *m_reboot_resource;

        // DeRegistation Resource
        DeviceDeRegisterResource            *m_deregister_resource; 
        
        // Bind our DeRegister Resource
        void bindDeRegisterResource();

        // Bind our Reboot Resource
        void bindRebootResource();
};

#endif // __DEVICE_MANAGER_H__
