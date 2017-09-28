/**
 * @file    ObjectInstanceManager.h
 * @brief   mbed CoAP Endpoint Device Management Object Instance Manager (header)
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

#ifndef __OBJECT_INSTANCE_MANAGER_H__
#define __OBJECT_INSTANCE_MANAGER_H__

// Logger
#include "mbed-connector-interface/Logger.h"

// Named Pointer List 
#include "mbed-connector-interface/NamedPointer.h"

// Resources list
#include <vector>
typedef vector<NamedPointer> NamedPointerList;

class ObjectInstanceManager {
    public:
        /**
        Default constructor
        @param logger input logger instance
        @param ep input the Endpoint instance
        */
        ObjectInstanceManager(const Logger *logger,const void *ep);
        
        /**
        Copy constructor
        @param resource input the ObjectInstanceManager that is to be deep copied
        */
        ObjectInstanceManager(const ObjectInstanceManager &manager);
    
        /**
        Destructor
        */
        virtual ~ObjectInstanceManager();
        
        /**
        Create DynamicResourceInstance
        @param objID input the Object ID to parent this new resource instance under 
        @param resID input the Resource ID for this resource
        @param resName input the Resource Name 
        @param resType input the type of Resource (cast from Resource::ResourceType)
        @param observable input whether this Resource is observable or not
        */
        void *createDynamicResourceInstance(char *objID,char *resID,char *resName,int resType,bool observable);
        
        /**
        Create StaticResourceInstance
        @param objID input the Object ID to parent this new resource instance under 
        @param resID input the Resource ID for this resource
        @param resName input the Resource Name 
        @param resType input the type of Resource (cast from Resource::ResourceType)
        @param observable input whether this Resource is observable or not
        */
        void *createStaticResourceInstance(char *objID,char *resID,char *resName,int resType,void *data,int data_length);
        
        /**
        Get our Object List
        */
        NamedPointerList getObjectList();
        
        /** 
        Get the instance number of the just-created ResourceInstance
        */
        int getLastCreatedInstanceNumber();
    
    protected:
        Logger          *m_logger;
        void            *m_ep;
        NamedPointerList m_object_list;
        int              m_instance_number;
        
    private:
        // Generic Static and Dynamic Resource Instances/Objects
        void *getOrCreateInstance(char *objID,char *resID);
        NamedPointer *getOrCreateObject(char *objID);
                
        // Underlying lookup support
        NamedPointer *getNamedPointer(const char *id,NamedPointerList *list);
        
        // Logger
        Logger *logger();
};

#endif // __OBJECT_INSTANCE_MANAGER_H__