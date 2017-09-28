/**
 * @file    NamedPointer.cpp
 * @brief   mbed CoAP Endpoint Device Management Named Pointer
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
 
// Base Class
#include "mbed-connector-interface/ObjectInstanceManager.h"

// constructor
NamedPointer::NamedPointer(string name,void *ptr,int index) {
    this->m_name = name;
    this->m_ptr = ptr;
    this->m_index = index;
    this->m_list = (void *)new NamedPointerList();
}

// copy constructor
NamedPointer::NamedPointer(const NamedPointer &np) {
    this->m_name = np.m_name;
    this->m_ptr = np.m_ptr;
    this->m_index = np.m_index;
    this->m_list = this->copyList(np.m_list);
}

// Destructor
NamedPointer::~NamedPointer() {
    NamedPointerList *list = (NamedPointerList *)this->m_list;
    if (list != NULL) {
        delete list;
    }
}

// Get the Name
string NamedPointer::name() { 
    return this->m_name; 
}

// Get the Pointer
void *NamedPointer::ptr() { 
    return this->m_ptr; 
}

// Get the Index
int NamedPointer::index() {
    return this->m_index;
}

// Get any associated list
void *NamedPointer::list() { 
    return this->m_list; 
}

// Copy the list
void *NamedPointer::copyList(void *list) {
    NamedPointerList *npl = new NamedPointerList();
    NamedPointerList *tmp_list = (NamedPointerList *)list;
    for(int i=0;tmp_list != NULL && i<(int)tmp_list->size();++i) {
        npl->push_back(tmp_list->at(i));
    }
    return (void *)npl;
}
 