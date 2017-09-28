/**
 * @file    PassphraseAuthenticator.h
 * @brief   mbed CoAP Endpoint Device Management Passphrase-based Authenticator class
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

#ifndef __PASSPHRASE_AUTHENTICATOR_H__
#define __PASSPHRASE_AUTHENTICATOR_H__

// Base class
#include "mbed-connector-interface/Authenticator.h"

// string support
#include <string>

class PassphraseAuthenticator : public Authenticator {
    public:
        /**
        Default constructor
        @param logger input logger instance
        @param secret input secret (passphrase)
        */
        PassphraseAuthenticator(const Logger *logger,const void *passphrase = NULL);
        
        /**
        Copy constructor
        @param resource input the BasicAuthenticator that is to be deep copied
        */
        PassphraseAuthenticator(const PassphraseAuthenticator &manager);
    
        /**
        Destructor
        */
        virtual ~PassphraseAuthenticator();
        
        /**
        Authenticate
        @param challenge input the challenge to authenticate with
        @return true - authenciated successfully, false - otherwise
        */
        virtual bool authenticate(void *challenge); 
    
    private:
        string coapDataToString(uint8_t *coap_data_ptr,int coap_data_ptr_length);
};

#endif // __PASSPHRASE_AUTHENTICATOR_H__