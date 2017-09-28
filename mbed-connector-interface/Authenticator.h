/** 
 * @file    Authenticator.h
 * @brief   mbed CoAP Endpoint Device Management Authenticator class
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

#ifndef __DM_AUTHENTICATOR_H__
#define __DM_AUTHENTICATOR_H__

// Logger
#include "mbed-connector-interface/Logger.h"

class Authenticator {
    public:
        /**
        Default constructor
        @param logger input logger instance
        @param secret input secret
        */
        Authenticator(const Logger *logger,const void *secret = NULL);
        
        /**
        Copy constructor
        @param resource input the Authenticator that is to be deep copied
        */
        Authenticator(const Authenticator &manager);
    
        /**
        Destructor
        */
        virtual ~Authenticator();
        
        /**
        Set the secret
        @param secret input the secret
        */
        virtual void setSecret(void *secret);
        
        /**
        Authenticate
        @param challenge input the challenge to authenticate with
        @return true - authenciated successfully, false - otherwise
        */
        virtual bool authenticate(void *challenge); 
    
    protected:
        Logger  *m_logger;
        void    *m_secret;
};

#endif // __DM_AUTHENTICATOR_H__