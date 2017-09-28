/**
 * @file    Authenticator.cpp
 * @brief   mbed CoAP Endpoint Device Management Authenticator (base) class
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
 #include "mbed-connector-interface/Authenticator.h"
 
 // constructor
 Authenticator::Authenticator(const Logger *logger,const void *secret) {
     this->m_logger = (Logger *)logger;
     this->setSecret((void *)secret);
}

// copy constructor
Authenticator::Authenticator(const Authenticator &authenticator) {
    this->m_logger = authenticator.m_logger;
    this->m_secret = authenticator.m_secret;
}

// set secret
void Authenticator::setSecret(void *secret) {
    this->m_secret = secret;
}

// destructor
Authenticator::~Authenticator() {
}

// authenticator 
bool Authenticator::authenticate(void * /*challenge */) { return false; }