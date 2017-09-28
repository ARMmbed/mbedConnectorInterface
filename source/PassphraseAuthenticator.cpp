/**
 * @file    PassphraseAuthenticator.cpp
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
 
// Class support
#include "mbed-connector-interface/PassphraseAuthenticator.h"

// demarshalling support
#include "mbed-client/m2mresource.h"
 
// constructor
PassphraseAuthenticator::PassphraseAuthenticator(const Logger *logger,const void *passphrase) : Authenticator(logger,passphrase) {
}

// copy constructor
PassphraseAuthenticator::PassphraseAuthenticator(const PassphraseAuthenticator &authenticator) : Authenticator(authenticator) {
}

// destructor
PassphraseAuthenticator::~PassphraseAuthenticator() {
}

// basic (trivial passphrase authentication)
bool PassphraseAuthenticator::authenticate(void *challenge) {
    // use simple, trivial passphrase based comparison as the check... 
    char *passphrase = (char *)this->m_secret;
    char *input_passphrase = NULL;
    
#if defined (HAS_EXECUTE_PARAMS)
    // ExecParam mbed-client: un-marshall the ExecuteParameter to get the simple string...
    M2MResource::M2MExecuteParameter* param = (M2MResource::M2MExecuteParameter*)challenge;
    if (param != NULL) {
        // use parameters to extract the passphrase
        String object_name = param->get_argument_object_name();
        // int instance_id = (int)param->get_argument_object_instance_id();
        String resource_name = param->get_argument_resource_name();
        string value = this->coapDataToString((uint8_t *)param->get_argument_value(),param->get_argument_value_length());
        input_passphrase = (char *)value.c_str();
    }
#else 
    // Non-ExecParam mbed-client: use the parameter directly... 
    input_passphrase = (char *)challenge;
#endif
    
    // DEBUG
    //this->m_logger->log("Authenticator(passphrase): passphrase: [%s] challenge: [%s]",passphrase,input_passphrase);
    
    // parameter checks...the compare passphrases and return the result
    if (passphrase != NULL && input_passphrase != NULL && strcmp(passphrase,input_passphrase) == 0) {
        // DEBUG
        this->m_logger->log("Authenticator(passphrase): Passphrases MATCH. Authenticated.");
    
        // passphrases match
        return true;
    }
    
    // DEBUG
    this->m_logger->log("Authenticator(passphrase): Passphrases do not match");
    
    // authentication failure
    return false;
}

// convenience method to get the URI from its buffer field...
string PassphraseAuthenticator::coapDataToString(uint8_t *coap_data_ptr,int coap_data_ptr_length) {
    if (coap_data_ptr != NULL && coap_data_ptr_length > 0) {
        char buf[MAX_VALUE_BUFFER_LENGTH+1];
        memset(buf,0,MAX_VALUE_BUFFER_LENGTH+1);
        int length = coap_data_ptr_length;
        if (length > MAX_VALUE_BUFFER_LENGTH) {
            length = MAX_VALUE_BUFFER_LENGTH;
            this->m_logger->log("Authenticator(passphrase): WARNING clipped data: %d bytes to %d bytes. Increase MAX_VALUE_BUFFER_LENGTH",
                                coap_data_ptr_length,length);
        }
        memcpy(buf,(char *)coap_data_ptr,length);
        return string(buf);
    }
    return string("");
}