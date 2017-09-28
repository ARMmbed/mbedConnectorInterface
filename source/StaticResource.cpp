/**
 * @file    StaticResource.h
 * @brief   mbed CoAP Endpoint Static Resource class
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
#include "mbed-connector-interface/StaticResource.h"

// Endpoint 
#include "mbed-connector-interface/ConnectorEndpoint.h"

// Constructor
StaticResource::StaticResource(const Logger *logger,const char *obj_name,const char *res_name, const char *value) : Resource<string>(logger,string(obj_name),string(res_name),string(value)), m_data_wrapper()
{
	this->m_data_wrapper = NULL;
	this->m_res = NULL;
}

// Constructor
StaticResource::StaticResource(const Logger *logger,const char *obj_name,const char *res_name,const string value) : Resource<string>(logger,string(obj_name),string(res_name),string(value))
{
    this->m_data_wrapper = NULL;
    this->m_res = NULL;
}

// Constructor with buffer lengths
StaticResource::StaticResource(const Logger *logger,const string obj_name,const string res_name,const string value) : Resource<string>(logger,string(obj_name),string(res_name),string(value))
{
    this->m_data_wrapper = NULL;
    this->m_res = NULL;
}

// Copy constructor
StaticResource::StaticResource(const StaticResource &resource) : Resource<string>((const Resource<string> &)resource)
{
    this->m_data_wrapper = resource.m_data_wrapper;
    this->m_res = resource.m_res;
}

// Destructor
StaticResource::~StaticResource() {
}

// bind CoAP Resource..
void StaticResource::bind(void *ep) {
	// Static Resources nailed to STRING type...
	int type = (int)M2MResourceInstance::STRING;
	
	// check our Endpoint instance...
	if (ep != NULL) {
		// cast
		Connector::Endpoint *endpoint = (Connector::Endpoint *)ep;
		
		// get our ObjectInstanceManager
		ObjectInstanceManager *oim = endpoint->getObjectInstanceManager();
		
		if (this->getDataWrapper() != NULL) {
			// wrap the data...
            this->getDataWrapper()->wrap((uint8_t *)this->getValue().c_str(),(int)this->getValue().size());
            
			// Create our Resource
			this->m_res = (M2MResource *)oim->createStaticResourceInstance((char *)this->getObjName().c_str(),(char *)this->getResName().c_str(),(char *)"StaticResource",(int)type,(void *)this->getDataWrapper()->get(),(int)this->getDataWrapper()->length());
			if (this->m_res != NULL) {
				// Record our Instance Number
				this->setInstanceNumber(oim->getLastCreatedInstanceNumber());
			
				// DEBUG
				this->logger()->log("StaticResource: [%s] value: [%s] bound",this->getFullName().c_str(),this->getDataWrapper()->get());
			}
		}
		else {
			// Create our Resource
			this->m_res = (M2MResource *)oim->createStaticResourceInstance((char *)this->getObjName().c_str(),(char *)this->getResName().c_str(),(char *)"StaticResource",(int)type,(void *)this->getValue().c_str(),(int)this->getValue().size());
			if (this->m_res != NULL) {
				// Record our Instance Number
				this->setInstanceNumber(oim->getLastCreatedInstanceNumber());
			
				// DEBUG
				this->logger()->log("StaticResource: [%s] value: [%s] bound",this->getFullName().c_str(),this->getValue().c_str());
			}
		}
	}
	else {
		// no instance pointer to our endpoint
      	this->logger()->log("%s: NULL endpoint instance pointer in bind() request...",this->getFullName().c_str());
    }
}