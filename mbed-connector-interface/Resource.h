/**
 * @file    Resource.h
 * @brief   mbed CoAP Endpoint Resource base class template
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

#ifndef __RESOURCE_H__
#define __RESOURCE_H__

// logging facility
#include "mbed-connector-interface/Logger.h"

// mbed-client support
#include "mbed-client/m2minterfacefactory.h"
#include "mbed-client/m2minterfaceobserver.h"
#include "mbed-client/m2minterface.h"
#include "mbed-client/m2mobjectinstance.h"
#include "mbed-client/m2mresource.h"
#include "mbed-client/m2mdevice.h"
#include "mbed-client/m2mfirmware.h"

// string support
#include <string>

/** Resource class
 */
template <typename InnerType> class Resource
{
public:
    // Available Resource Types
    typedef enum {
        STRING,
        INTEGER,
        FLOAT,
        BOOLEAN,
        OPAQUE,
        TIME,
        OBJLINK
    }ResourceType;
    
    /**
    Default constructor
    @param logger input the Logger instance this Resource is a part of
    @param obj_name input the Object Name
    @param res_name input the Resource URI/Name
    @param value input the Resource value
    */
    Resource(const Logger *logger,const string obj_name,const string res_name,InnerType value)  {
        this->init(logger);
        this->m_obj_name = obj_name;
        this->m_res_name = res_name;
        this->m_value = value;
        this->m_implements_observation = false;
        this->m_instance_number = 0;
    }

    /**
    Copy constructor
    @param resource input the Resource that is to be deep copied
    */
    Resource(const Resource<InnerType> &resource) {
        this->init(resource.m_logger);
        this->m_endpoint = resource.m_endpoint;
        this->m_obj_name = resource.m_obj_name;
        this->m_res_name = resource.m_res_name;
        this->m_value = resource.m_value;
        this->m_implements_observation = resource.m_implements_observation;
        this->m_instance_number = resource.m_instance_number;
    }

    /**
    Destructor
    */
    virtual ~Resource() {
    }

    /**
    Get the Object name
    @return the name of the object
    */
    string getObjName() {
        return this->m_obj_name;
    }
    
    /**
    Get the Resource name
    @return the name of the resource
    */
    string getResName() {
        return this->m_res_name;
    }
    
    /**
    Get the Full  name
    @return the name of the object
    */
    string getFullName() {
        char buf[5];
        memset(buf,0,5);
        sprintf(buf,"/%d/",this->m_instance_number);
        return this->m_obj_name + buf + this->m_res_name;
    }

    /**
    Get the resource value
    @return the value of the resource
    */
    InnerType getValue() {
        return this->m_value;
    }

    /**
    Set the resource name
    @param name input the resource name
    */
    void setName(const string name) {
        this->m_name = name;
    }

    /**
    Set the resource value
    @param value input the resource value
    */
    void setValue(const InnerType value) {
        this->m_value = value;
    }

    /**
    Bind resource to endpoint
    @param ep input pointer to the Endpoint instance
    */
    virtual void bind(void *ep) = 0;

    // access the logger()
    Logger *logger() {
        return this->m_logger;
    }

    /**
    set the options
    */
    void setOptions(const void *options) {
        this->m_options = (void *)options;
    }
    
    /**
    set the endpoint
    */
    void setEndpoint(const void *endpoint) {
        this->m_endpoint = (void *)endpoint;
    }

    // this resource implements its own observation handler
    bool implementsObservation() { return this->m_implements_observation; }
    
    /**
    Get our Instance Number
    @return our instance number
    */
    int getInstanceNumber() { return this->m_instance_number; }
    
    /**
    Set our Instance Number
    @param instance_number input our designated instance number
    */
    void setInstanceNumber(int instance_number) { this->m_instance_number = instance_number; }

protected:
    // initialize internals to Resource
    void init(const Logger *logger) {
        this->m_logger = (Logger *)logger;
        this->m_endpoint = NULL;
        this->m_obj_name = "";
        this->m_res_name = "";
        this->m_value = "";
        this->m_instance_number = 0;
    }

    // get our options
    void *getOptions() {
        return this->m_options;
    }

    Logger         *m_logger;
    void           *m_endpoint;
    string          m_obj_name;
    string          m_res_name;
    InnerType       m_value;
    bool            m_implements_observation;
    void           *m_options;
    int             m_instance_number;
};

#endif // __RESOURCE_H__

