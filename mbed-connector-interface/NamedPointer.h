/**
 * @file    NamedPointer.h
 * @brief   mbed CoAP Endpoint Device Management Named Pointer (header)
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

#ifndef __NAMED_POINTER_H__
#define __NAMED_POINTER_H__

// String class
#include <string>

class NamedPointer {
public:
    // constructor
    NamedPointer(string name,void *ptr,int index);
    
    // copy constructor
    NamedPointer(const NamedPointer &np);
    
    // Destructor
    virtual ~NamedPointer();
    
    // Get the Name
    string name();
    
    // Get the Pointer
    void *ptr();
    
    // Get the associated list
    void *list();
    
    // Get our associated index
    int index();
    
private:
    string   m_name;
    int      m_index;
    void    *m_ptr; 
    void    *m_list;
    
    void    *copyList(void *list);
};

#endif // __NAMED_POINTER_H__