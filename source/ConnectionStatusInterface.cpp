/**
 * @file    ConnectionStatusInterface.cpp
 * @brief   ConnectionStatusInterface
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

// Class Support 
#include "mbed-connector-interface/ConnectionStatusInterface.h"

// Connector namespace
namespace Connector {
     
// Default constructor
ConnectionStatusInterface::ConnectionStatusInterface() {
}

// Copy constructor
ConnectionStatusInterface::ConnectionStatusInterface(ConnectionStatusInterface & /* csi */) {
}

// Destructor
ConnectionStatusInterface::~ConnectionStatusInterface() {
}

// Registered
void ConnectionStatusInterface::object_registered(void * /* ep */,void * /* security */,void * /*data */) {
}

// Re-registered
void ConnectionStatusInterface::registration_updated(void * /* ep */,void * /* security */,void * /*data */) {
}

// Beginning de-registration
void ConnectionStatusInterface::begin_object_unregistering(void * /* ep */) {
}

// De-registered
void ConnectionStatusInterface::object_unregistered(void * /* ep */,void * /* data */) {
}

// Bootstrapped
void ConnectionStatusInterface::bootstrapped(void * /* ep */,void * /* data */) {
}

// Value Updated
void ConnectionStatusInterface::value_updated(void * /* ep */,void * /* data */,int /* type */) {
}

} // Connector namespace