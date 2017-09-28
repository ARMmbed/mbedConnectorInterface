/**
 * @file    DynamicResource.cpp
 * @brief   mbed CoAP Endpoint Dynamic Resource class
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
#include "mbed-connector-interface/DynamicResource.h"

// ResourceObserver help
#include "mbed-connector-interface/ResourceObserver.h"

// Options enablement
#include "mbed-connector-interface/Options.h"

// Endpoint 
#include "mbed-connector-interface/ConnectorEndpoint.h"

// GET option that can be used to Start/Stop Observations...
#define START_OBS 0
#define STOP_OBS  1

// MaxAge support for each DynamicResource
#define DEFAULT_MAXAGE 60

// ContentFormat defaults for each DynamicResource
#define DEFAULT_CONTENT_FORMAT 0

// default constructor
DynamicResource::DynamicResource(const Logger *logger,const char *obj_name,const char *res_name,const char *res_type,uint8_t res_mask,const bool observable,const ResourceType type) : Resource<string>(logger,string(obj_name),string(res_name),string(""))
{
    this->m_res_type = string(res_type);
    this->m_type = type;
    this->m_observable = observable;
    this->m_res_mask = res_mask;
    this->m_obs_number = 0;
    this->m_data_wrapper = NULL;
    this->m_observer = NULL;
    this->m_maxage = DEFAULT_MAXAGE;
    this->m_content_format = DEFAULT_CONTENT_FORMAT;
    this->m_ep = NULL;
    this->m_res = NULL;
}

// constructor (input initial value)
DynamicResource::DynamicResource(const Logger *logger,const char *obj_name,const char *res_name,const char *res_type,const string value,uint8_t res_mask,const bool observable,const ResourceType type) : Resource<string>(logger,string(obj_name),string(res_name),value)
{
    this->m_res_type = string(res_type);
    this->m_type = type;
    this->m_observable = observable;
    this->m_res_mask = res_mask;
    this->m_obs_number = 0;
    this->m_data_wrapper = NULL;
    this->m_observer = NULL;
    this->m_maxage = DEFAULT_MAXAGE;
    this->m_content_format = DEFAULT_CONTENT_FORMAT;
    this->m_ep = NULL;
    this->m_res = NULL;
}

// constructor (strings)
DynamicResource::DynamicResource(const Logger *logger,const string obj_name,const string res_name,const string res_type,const string value,uint8_t res_mask,const bool observable,const ResourceType type) : Resource<string>(logger,obj_name,res_name,value)
{
    this->m_res_type = res_type;
    this->m_type = type;
    this->m_observable = observable;
    this->m_res_mask = res_mask;
    this->m_obs_number = 0;
    this->m_data_wrapper = NULL;
    this->m_observer = NULL;
    this->m_maxage = DEFAULT_MAXAGE;
    this->m_content_format = DEFAULT_CONTENT_FORMAT;
    this->m_ep = NULL;
    this->m_res = NULL;
}

// copy constructor
DynamicResource::DynamicResource(const DynamicResource &resource) : Resource<string>((const Resource<string> &)resource)
{
    this->m_res_type = resource.m_res_type;
    this->m_type = resource.m_type;
    this->m_observable = resource.m_observable;
    this->m_res_mask = resource.m_res_mask;
    this->m_obs_number = resource.m_obs_number;
    this->m_data_wrapper = resource.m_data_wrapper;
    this->m_observer = resource.m_observer;
    this->m_maxage = resource.m_maxage;
    this->m_content_format = resource.m_content_format;
    this->m_ep = resource.m_ep;
    this->m_res = resource.m_res;
}

// destructor
DynamicResource::~DynamicResource() {
}

// bind CoAP Resource...
void DynamicResource::bind(void *ep) {
	if (ep != NULL) {
		// cast
		Connector::Endpoint *endpoint = (Connector::Endpoint *)ep;
		
		// get our ObjectInstanceManager
		ObjectInstanceManager *oim = endpoint->getObjectInstanceManager();
		
		// Create our Resource
		this->m_res = (M2MResource *)oim->createDynamicResourceInstance((char *)this->getObjName().c_str(),(char *)this->getResName().c_str(),(char *)this->m_res_type.c_str(),(int)this->m_type,this->m_observable);
		if (this->m_res != NULL) {
			// Record our Instance Number
			this->setInstanceNumber(oim->getLastCreatedInstanceNumber());
			   
			// perform an initial get() to initialize our data value
			this->setValue(this->get());
			
			// now record the data value         			
			if (this->getDataWrapper() != NULL) {
				// wrap the data...
				this->getDataWrapper()->wrap((uint8_t *)this->getValue().c_str(),(int)this->getValue().size());
				this->m_res->set_operation((M2MBase::Operation)this->m_res_mask);
				this->m_res->set_value( this->getDataWrapper()->get(),(uint8_t)this->getDataWrapper()->length());
				this->logger()->log("%s: [%s] value: [%s] bound (observable: %d)",this->m_res_type.c_str(),this->getFullName().c_str(),this->getDataWrapper()->get(),this->m_observable);
			}
			else {
				// do not wrap the data...
				this->m_res->set_operation((M2MBase::Operation)this->m_res_mask);
				this->m_res->set_value((uint8_t *)this->getValue().c_str(),(uint8_t)this->getValue().size());
 				this->logger()->log("%s: [%s] value: [%s] bound (observable: %d)",this->m_res_type.c_str(),this->getFullName().c_str(),this->getValue().c_str(),this->m_observable);
			}
			
			// set our endpoint instance
			this->m_ep = (void *)ep;
			
			// For POST-enabled  RESOURCES (only...), we must add a callback
			if ((this->m_res_mask & M2MBase::POST_ALLOWED)  != 0) { 
				// add a callback for the execute function...we will just direct through process()...
				//this->logger()->log("DynamicResource::bind(): Setting up POST execute callback function");
				this->m_res->set_execute_function(execute_callback(this, &DynamicResource::process_resource_post));
			}    		
		}		
	}
	else {
		// no instance pointer to our endpoint
      	this->logger()->log("%s: NULL endpoint instance pointer in bind() request...",this->m_res_type.c_str());
    }
}

// get our M2MBase representation
M2MResource *DynamicResource::getResource() {
	return this->m_res;
}

// process inbound mbed-client POST message for a Resource
void DynamicResource::process_resource_post(void *args) {
		// just call process() for POST and Resources...
		//this->logger()->log("DynamicResource::process_resource_post(): calling process(POST)");
		(void)this->process(M2MBase::POST_ALLOWED,this->m_res->base_type(),args);
}

// process inbound mbed-client message
uint8_t DynamicResource::process(M2MBase::Operation op,M2MBase::BaseType type,void *args) {
#if defined (HAS_EXECUTE_PARAMS)
     M2MResource::M2MExecuteParameter* param = NULL;
     
     // cast args if present...
	 if (args != NULL) {
     	param = (M2MResource::M2MExecuteParameter*)args;
     }
#endif	
	// DEBUG
	//this->logger()->log("in %s::process()  Operation=0x0%x Type=%x%x",this->m_res_type.c_str(),op,type);
	
	// PUT() check
	if ((op & M2MBase::PUT_ALLOWED) != 0) {
		string value = this->coapDataToString(this->m_res->value(),this->m_res->value_length());
	 	this->logger()->log("%s: put(%d) [%s]=[%s] called.",this->m_res_type.c_str(),type,this->getFullName().c_str(),value.c_str());
     	this->put(value.c_str());
     	return 0;
    }
 
#if defined (HAS_EXECUTE_PARAMS)   
    // POST() check
	if ((op & M2MBase::POST_ALLOWED) != 0) {
	    string value;
	    if (param != NULL) {
	    	// use parameters
	    	String object_name = param->get_argument_object_name();
	    	int instance_id = (int)param->get_argument_object_instance_id();
	    	String resource_name = param->get_argument_resource_name();
	    	value = this->coapDataToString((uint8_t *)param->get_argument_value(),param->get_argument_value_length());
	    	this->logger()->log("%s: post(%d) [%s/%d/%s]=[%s]) called.",this->m_res_type.c_str(),type,object_name.c_str(),instance_id,resource_name.c_str(),value.c_str());
	    }
	    else {
	    	// use the resource value itself
			value = this->coapDataToString((uint8_t *)this->m_res->value(),this->m_res->value_length());
	 		this->logger()->log("%s: post(%d) [%s]=[%s] called.",this->m_res_type.c_str(),type,this->getFullName().c_str(),value.c_str());
     	}
     	
     	// invoke
     	this->post(args);
     	return 0;
    }
#else  
    // POST() check
	if ((op & M2MBase::POST_ALLOWED) != 0) {
		if (args != NULL) {
			this->logger()->log("%s: post(%d) [%s]=[%s] called.",this->m_res_type.c_str(),type,this->getFullName().c_str(),(char *)args);
	     	this->post(args);
     	}
     	else {
     		string value = this->coapDataToString((uint8_t *)this->m_res->value(),this->m_res->value_length());
		 	this->logger()->log("%s: post(%d) [%s]=[%s] called.",this->m_res_type.c_str(),type,this->getFullName().c_str(),value.c_str());
	     	this->post((void *)value.c_str());
     	}
     	return 0;
    }
#endif

#if defined (HAS_EXECUTE_PARAMS)
	// DELETE() check
	if ((op & M2MBase::DELETE_ALLOWED) != 0) {
		if (param != NULL) {
	    	// use parameters
	    	String object_name = param->get_argument_object_name();
	    	int instance_id = (int)param->get_argument_object_instance_id();
	    	String resource_name = param->get_argument_resource_name();
	    	string value = this->coapDataToString((uint8_t *)param->get_argument_value(),param->get_argument_value_length());
	    	this->logger()->log("%s: delete(%d) [%s/%d/%s]=[%s]) called.",this->m_res_type.c_str(),type,object_name.c_str(),instance_id,resource_name.c_str(),value.c_str());
	    }
	    else {
	    	// use the resource value itself
			string value = this->coapDataToString((uint8_t *)this->m_res->value(),this->m_res->value_length());
	 		this->logger()->log("%s: delete(%d) [%s]=[%s] called.",this->m_res_type.c_str(),type,this->getFullName().c_str(),value.c_str());
     	}
     	
     	// invoke
     	this->del(args);
     	return 0;
    }
#else     
    // DELETE() check
	if ((op & M2MBase::DELETE_ALLOWED) != 0) {
		if (args != NULL) {
			this->logger()->log("%s: delete(%d) [%s]=[%s] called.",this->m_res_type.c_str(),type,this->getFullName().c_str(),(char *)args);
	     	this->del(args);
     	}
     	else {
     		string value = this->coapDataToString((uint8_t *)this->m_res->value(),this->m_res->value_length());
		 	this->logger()->log("%s: delete(%d) [%s]=[%s] called.",this->m_res_type.c_str(),type,this->getFullName().c_str(),value.c_str());
	     	this->del((void *)value.c_str());
     	}
     }
#endif
     
     // unknown type...
     this->logger()->log("%s: Unknown Operation (0x%x) for [%s]=[%s]... FAILED.",op,this->m_res_type.c_str(),this->getFullName().c_str(),this->m_res->value());
     return 1;
}

// send the notification
int DynamicResource::notify(const string data) {
    return this->notify((uint8_t *)data.c_str(),(int)data.length());
}

// send the notification
int DynamicResource::notify(uint8_t *data,int data_length) {
    uint8_t *notify_data = NULL;
    int notify_data_length = 0;
    int status = 0;

    // convert the string from the GET to something suitable for CoAP payloads
    if (this->getDataWrapper() != NULL) {
        // wrap the data...
        this->getDataWrapper()->wrap((uint8_t *)data,data_length);

        // announce (after wrap)
        //this->logger()->log("Notify payload [%s]...",this->getDataWrapper()->get());

        // fill notify
        notify_data_length = this->getDataWrapper()->length();
        notify_data = this->getDataWrapper()->get();
    }
    else {
        // announce (no wrap)
        //this->logger()->log("Notify payload [%s]...",data);

        // do not wrap the data...
        notify_data_length = data_length;
        notify_data = data;
    }
    
    // update the resource
    this->m_res->set_value((uint8_t *)notify_data,(uint8_t)notify_data_length);

    // return our status
    return status;
}

// default GET (does nothing)
string DynamicResource::get()
{
    // not used by default
    //this->logger()->log("DynamicResource::get() invoked (NOOP)");
    return string("");
}

// default PUT (does nothing)
void DynamicResource::put(const string /* value */)
{
    // not used by default
    //this->logger()->log("DynamicResource::put() invoked (NOOP)");
}

// default POST (does nothing)
void DynamicResource::post(void * /* args */)
{
    // not used by default
    //this->logger()->log("DynamicResource::post() invoked (NOOP)");
}

// default DELETE (does nothing)
void DynamicResource::del(void * /* args */)
{
    // not used by default
    //this->logger()->log("DynamicResource::del() invoked (NOOP)");
}

// default observe behavior
void DynamicResource::observe() {
    if (this->m_observable == true && this->isRegistered() == true) {
        this->notify(this->get());
    }
}

// set the observer pointer
void DynamicResource::setObserver(void *observer) {
    this->m_observer = observer;
}

// set the content-format in responses
void DynamicResource::setContentFormat(uint8_t content_format) {
    this->m_content_format = content_format;
}

// set the max-age of responses
void DynamicResource::setMaxAge(uint8_t maxage) {
    this->m_maxage = maxage;
}

// convert the CoAP data pointer to a string type
string DynamicResource::coapDataToString(uint8_t *coap_data_ptr,int coap_data_ptr_length)
{
    if (coap_data_ptr != NULL && coap_data_ptr_length > 0) {
        if (this->getDataWrapper() != NULL) {
            // unwrap the data...
            this->getDataWrapper()->unwrap(coap_data_ptr,coap_data_ptr_length);
            char *buf = (char *)this->getDataWrapper()->get();                  // assumes data is null terminated in DataWrapper...
            return string(buf);
        }
        else {
            // no unwrap of the data...
            char buf[MAX_VALUE_BUFFER_LENGTH+1];
            memset(buf,0,MAX_VALUE_BUFFER_LENGTH+1);
            int length = coap_data_ptr_length;
            if (length > MAX_VALUE_BUFFER_LENGTH) {
            	length = MAX_VALUE_BUFFER_LENGTH;
            	this->logger()->log("DynamicResource::coapDataToString: WARNING clipped data: %d bytes to %d bytes. Increase MAX_VALUE_BUFFER_LENGTH",
            					    coap_data_ptr_length,length);
            }
            memcpy(buf,(char *)coap_data_ptr,length);
            return string(buf);
        }
    }
    return string("");
}

// convert the CoAP data pointer to an integer type
int DynamicResource::coapDataToInteger(uint8_t *coap_data_ptr,int coap_data_ptr_length) {
	int value = 0;
	if (coap_data_ptr != NULL && coap_data_ptr_length > 0) {
        if (this->getDataWrapper() != NULL) {
            // unwrap the data...
            this->getDataWrapper()->unwrap(coap_data_ptr,coap_data_ptr_length);
            //value = (int)this->getDataWrapper()->get();                  // assumes data is null terminated in DataWrapper...
        }
        else {
            // no unwrap of the data...
            //value = (int)coap_data_ptr;
        }
    }
    return value;
}

// convert the CoAP data pointer to a float type
float DynamicResource::coapDataToFloat(uint8_t *coap_data_ptr,int coap_data_ptr_length) {
	float value = 0.0;
	if (coap_data_ptr != NULL && coap_data_ptr_length > 0) {
        if (this->getDataWrapper() != NULL) {
            // unwrap the data...
            this->getDataWrapper()->unwrap(coap_data_ptr,coap_data_ptr_length);
            //value = (float)this->getDataWrapper()->get();                  // assumes data is null terminated in DataWrapper...
        }
        else {
            // no unwrap of the data...
            //value = (float)coap_data_ptr;
        }
    }
    return value;
}

// convert the CoAP data pointer to an opaque type
void *DynamicResource::coapDataToOpaque(uint8_t *coap_data_ptr,int coap_data_ptr_length) {
	if (coap_data_ptr != NULL && coap_data_ptr_length > 0) {
        if (this->getDataWrapper() != NULL) {
            // unwrap the data...
            this->getDataWrapper()->unwrap(coap_data_ptr,coap_data_ptr_length);
            char *buf = (char *)this->getDataWrapper()->get();                  // assumes data is null terminated in DataWrapper...
            return (void *)buf;
        }
    }
    return (void *)coap_data_ptr;
}

// Determine if we are connected or not
bool DynamicResource::isConnected() {
	bool is_connected = false;
	
	// get our Endpoint
	Connector::Endpoint *ep = (Connector::Endpoint *)this->m_endpoint;
	if (ep != NULL)  {
		is_connected = ep->isConnected();
		if (is_connected) {
			//this->logger()->log("DynamicResource::isConnected = true");
		}
		else { 
			//this->logger()->log("DynamicResource::isConnected = false");
		}
	}
	else {
		this->logger()->log("DynamicResource::isConnected = false (no endpoint)");
	}
	
	// return our endpoint connection state
	return is_connected;
}

// Determine if we are registered or not
bool DynamicResource::isRegistered() {
	bool is_registered = false;
	
	if (this->isConnected() == true) {
		// get our Endpoint
		Connector::Endpoint *ep = (Connector::Endpoint *)this->m_endpoint;
		if (ep != NULL)  {
			is_registered = ep->isRegistered();
			if (is_registered) {
				//this->logger()->log("DynamicResource::isRegistered = true");
			}
			else { 
				//this->logger()->log("DynamicResource::isRegistered = false");
			}
		}
		else {
			this->logger()->log("DynamicResource::isRegistered = false (no endpoint)");
		}
	}
	
	// return our endpoint registration state
	return is_registered;
}

// get our observer
void *DynamicResource::getObserver() {
	return this->m_observer;
}