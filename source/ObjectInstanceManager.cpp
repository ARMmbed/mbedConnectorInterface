/**
 * @file    ObjectInstanceManager.cpp
 * @brief   mbed CoAP Endpoint Device Management Object Instance Manager
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
#include "mbed-connector-interface/ObjectInstanceManager.h"
 
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
 
 // constructor
 ObjectInstanceManager::ObjectInstanceManager(const Logger *logger,const void *ep) {
     this->m_logger = (Logger *)logger;
     this->m_ep = (void *)ep;
     this->m_object_list.clear();
     this->m_instance_number = 0;
}

// copy constructor
ObjectInstanceManager::ObjectInstanceManager(const ObjectInstanceManager &oim) {
    this->m_logger = oim.m_logger;
    this->m_ep = oim.m_ep;
    this->m_object_list = oim.m_object_list;
    this->m_instance_number = oim.m_instance_number;
}

// destructor
ObjectInstanceManager::~ObjectInstanceManager() {
    this->m_object_list.clear();
}

// create a Dynamic Resource Instance
void *ObjectInstanceManager::createDynamicResourceInstance(char *objID,char *resID,char *resName,int resType,bool observable) {
    void *res = NULL;
    M2MObjectInstance *instance = (M2MObjectInstance *)this->getOrCreateInstance(objID,resID);
    if (instance != NULL) {
        // DEBUG
        //this->logger()->log("ObjectInstanceManager: Creating Dynamic Resource: ObjID:%s ResID:%s ResName:%s Type:%d Observable: %d",objID,resID,resName,resType,observable);
    
        // create the resource
        res = (void *)instance->create_dynamic_resource(resID,resName,(M2MResourceInstance::ResourceType)resType,observable);
    }
    return res;   
}

// create a Static Resource Instance
void *ObjectInstanceManager::createStaticResourceInstance(char *objID,char *resID,char *resName,int resType,void *data,int data_length) {
    void *res = NULL;
    M2MObjectInstance *instance = (M2MObjectInstance *)this->getOrCreateInstance(objID,resID);
    if (instance != NULL) {
        // DEBUG
        //this->logger()->log("ObjectInstanceManager: Creating Static Resource: ObjID:%s ResID:%s ResName:%s Type:%d DataLength: %d",objID,resID,resName,resType,data_length);
    
        // create the resource
        res = (void *)instance->create_static_resource(resID,resName,(M2MResourceInstance::ResourceType)resType,(uint8_t *)data,(uint8_t)data_length);
    }
    return res;  
}

// Get the instance number of the resource just created
int ObjectInstanceManager::getLastCreatedInstanceNumber() {
    return this->m_instance_number;
}

// create and/or retrieve a given instance
void *ObjectInstanceManager::getOrCreateInstance(char *objID,char *resID) {
    void *instance = NULL;
    NamedPointer *obj_np = this->getOrCreateObject(objID);
    if (obj_np != NULL) {
        NamedPointerList *list = (NamedPointerList *)obj_np->list();
        NamedPointer *inst_np = this->getNamedPointer(resID,list);
        if (inst_np != NULL) {
            // instance already exists... so create another one (n)...
            M2MObject *obj = (M2MObject *)(obj_np->ptr());
            if (obj != NULL) {
                instance = (void *)obj->create_object_instance();
                this->m_instance_number = inst_np->index()+1;
                NamedPointer new_inst_np(string(resID),instance,this->m_instance_number);
                list->push_back(new_inst_np); 
            }
        }
        else if (list->size() > 0) {
            // 0th instance exists... parent the resource to it...
            this->m_instance_number = 0;
            instance = (void *)(list->at(0).ptr());
        }
        else {            
            // no instance does not exist so create one (0)..
            M2MObject *obj = (M2MObject *)(obj_np->ptr());
            if (obj != NULL) {
                instance = (void *)obj->create_object_instance();
                this->m_instance_number = 0;
                NamedPointer new_inst_np(string(resID),instance,this->m_instance_number);
                list->push_back(new_inst_np);
            }
        }
    }
    else {
        // DEBUG
        this->logger()->log("getOrCreateInstance: unable to create object instance for objID:%s",objID); 
    }
    return instance;
}

// create and/or retrieve a given objectID
NamedPointer *ObjectInstanceManager::getOrCreateObject(char *objID) {
    NamedPointer *result = NULL;
    if (objID != NULL) {
        result = this->getNamedPointer(objID,&(this->m_object_list));
    }
    if (result == NULL) {            
        void *obj = (void *)M2MInterfaceFactory::create_object(objID);
        NamedPointer new_np(string(objID),obj,0);
        this->m_object_list.push_back(new_np);
        result = this->getNamedPointer(objID,&(this->m_object_list));
    }
    return result;
}

// get the named pointer for a given id
NamedPointer *ObjectInstanceManager::getNamedPointer(const char *id,NamedPointerList *list) {
    NamedPointer *result = NULL;
    bool found = false;
    for(int i=0;list != NULL && i<(int)list->size() && !found;++i) {
        char *tmp_id = (char *)list->at(i).name().c_str();
        if (id != NULL && tmp_id != NULL && strcmp(id,tmp_id) == 0) {
            found = true;
            result = &(list->at(i));
        }
    }
    return result;
}

// Get our Object List
NamedPointerList ObjectInstanceManager::getObjectList() {
    return this->m_object_list;
}

// Logger
Logger *ObjectInstanceManager::logger() {
    return this->m_logger;
}

