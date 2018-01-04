/**
 * @file    RebootResource.h
 * @brief   mbed CoAP Endpoint Device Reboot Resource
 * @author  Doug Anson
 * @version 1.0
 * @see
 *
 * Copyright (c) 2017
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

#ifndef __DEVICE_REBOOT_RESOURCE_H__
#define __DEVICE_REBOOT_RESOURCE_H__

// Base class
#include "mbed-connector-interface/DynamicResource.h"

// Device Management Responder
#include "mbed-connector-interface/DeviceManagementResponder.h"

/** RebootResource class
 */
class DeviceRebootResource : public DynamicResource
{
public:
    /**
     * Default constructor
     * @param logger input logger instance for this resource
     * @param obj_name input the sample object name
     * @param res_name input the sample resource name
     * @param observable input the resource is Observable (default: FALSE)
     */
    DeviceRebootResource(const Logger *logger,const char *obj_name,const char *res_name,void *dm_responder,const bool observable = false) :
        DynamicResource(logger,obj_name,res_name,"Reboot",M2MBase::POST_ALLOWED,observable) {
        this->m_dm_responder = (DeviceManagementResponder *)dm_responder;
        this->m_value = string("OK");
    }

    /**
     * Post the value of the Sample Resource
     */
    virtual void post(void *args) {
            if (this->m_dm_responder != NULL) {
                    // perform our action
                    this->m_dm_responder->rebootDevice(args);
            }
            else {
                    this->logger()->log("RebootResource: No DM Responder registered. Ignoring reboot request.");
            }
    }

protected:
    DeviceManagementResponder     *m_dm_responder;
};

#endif // __DEVICE_REBOOT_RESOURCE_H__
