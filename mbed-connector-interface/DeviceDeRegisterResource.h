/**
 * @file    DeviceDeRegisterResource.h
 * @brief   mbed CoAP Endpoint Device DeRegister Resource
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

#ifndef __DEVICE_DEREGISTER_RESOURCE_H__
#define __DEVICE_DEREGISTER_RESOURCE_H__

// Base class
#include "mbed-connector-interface/DynamicResource.h"

// Device Management Responder 
#include "mbed-connector-interface/DeviceManagementResponder.h"

/** DeviceDeRegisterResource class
 */
class DeviceDeRegisterResource : public DynamicResource
{
public:
    /**
    Default constructor
    @param logger input logger instance for this resource
    @param obj_name input the Light Object name
    @param res_name input the Light Resource name
    @param dm_responder input the DM responder instance
    */
    DeviceDeRegisterResource(const Logger *logger,const char *obj_name,const char *res_name,const void *dm_responder,bool observable = false) : 
        DynamicResource(logger,obj_name,res_name,"Deregister",M2MBase::POST_ALLOWED,observable)
    {
        this->m_dm_responder = (DeviceManagementResponder *)dm_responder;
        this->m_value = string("OK");
    }
    
    /**
    POST handler
    @param value input input value (DM authentication challenge)
    */
    virtual void post(void *args) {
        // perform our action
        this->m_dm_responder->deregisterDevice(args); 
    }
    
protected:
    DeviceManagementResponder     *m_dm_responder; 
};

#endif // __DEVICE_DEREGISTER_RESOURCE_H__
