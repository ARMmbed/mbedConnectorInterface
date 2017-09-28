/**
 * @file    DeviceManagementResponder.cpp
 * @brief   mbed CoAP Endpoint Device Management Responder class
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
 #include "mbed-connector-interface/DeviceManagementResponder.h"
 
 // Endpoint support
 #include "mbed-connector-interface/ConnectorEndpoint.h"
 
 // DeviceManager Support
 #include "mbed-connector-interface/DeviceManager.h"
 
 // constructor
 DeviceManagementResponder::DeviceManagementResponder(const Logger *logger,const Authenticator *authenticator) {
     this->m_logger = (Logger *)logger;
     this->m_authenticator = (Authenticator *)authenticator;
     this->m_endpoint = NULL;
     this->m_initialize_fn = NULL;
     this->m_reboot_responder_fn = NULL;
     this->m_reset_responder_fn = NULL;
     this->m_fota_manifest_fn = NULL;
     this->m_fota_image_set_fn = NULL;
     this->m_fota_invocation_fn = NULL;
     this->m_manifest = NULL;
     this->m_manifest_length = 0;
     this->m_fota_image = NULL;
     this->m_fota_image_length = 0;
}

// copy constructor
DeviceManagementResponder::DeviceManagementResponder(const DeviceManagementResponder &responder) {
    this->m_logger = responder.m_logger;
    this->m_authenticator = responder.m_authenticator;
    this->m_endpoint = responder.m_endpoint;
    this->m_initialize_fn = responder.m_initialize_fn;
    this->m_reboot_responder_fn = responder.m_reboot_responder_fn;
    this->m_reset_responder_fn = responder.m_reset_responder_fn;
    this->m_fota_manifest_fn = responder.m_fota_manifest_fn;
    this->m_fota_image_set_fn = responder.m_fota_image_set_fn;
    this->m_fota_invocation_fn = responder.m_fota_invocation_fn;
    this->m_manifest = responder.m_manifest;
    this->m_manifest_length = responder.m_manifest_length;
    this->m_fota_image = responder.m_fota_image;
    this->m_fota_image_length = responder.m_fota_image_length;
}

// destructor
DeviceManagementResponder::~DeviceManagementResponder() {
}

// set the Endpoint
void DeviceManagementResponder::setEndpoint(const void *ep) {
    this->m_endpoint = (void *)ep;
}

// authenticate
bool DeviceManagementResponder::authenticate(const void *challenge) {
    if (this->m_authenticator != NULL) {
        return this->m_authenticator->authenticate((void *)challenge);
    }
    return false;
}

// set the Initialize handler 
void DeviceManagementResponder::setInitializeHandler(initialize_fn initialize_fn) {
    this->m_initialize_fn = initialize_fn;
    if (this->m_initialize_fn != NULL) {
        (*this->m_initialize_fn)(this->m_logger);
    }
}

// set the Reboot Responder handler 
void DeviceManagementResponder::setRebootResponderHandler(responder_fn reboot_responder_fn) {
    this->m_reboot_responder_fn = reboot_responder_fn;
}

// set the Reset Responder handler 
void DeviceManagementResponder::setResetResponderHandler(responder_fn reset_responder_fn) {
    this->m_reset_responder_fn = reset_responder_fn;
}

// set the FOTA manifest handler 
void DeviceManagementResponder::setFOTAManifestHandler(manifest_fn fota_manifest_fn) {
    this->m_fota_manifest_fn = fota_manifest_fn;
}

// set the FOTA image set handler 
void DeviceManagementResponder::setFOTAImageHandler(image_set_fn fota_image_set_fn) {
    this->m_fota_image_set_fn = fota_image_set_fn;
}

// set the FOTA invocation handler 
void DeviceManagementResponder::setFOTAInvocationHandler(responder_fn fota_invocation_fn) {
    this->m_fota_invocation_fn = fota_invocation_fn;
}

// set the FOTA manifest
void DeviceManagementResponder::setFOTAManifest(char *manifest,uint32_t manifest_length) {
    this->m_manifest = manifest;
    this->m_manifest_length = manifest_length;
    if (this->m_fota_manifest_fn != NULL) {
        (*this->m_fota_manifest_fn)(this->m_endpoint,this->m_logger,this->m_manifest,this->m_manifest_length);
    }
}

// get the FOTA manifest
char *DeviceManagementResponder::getFOTAManifest() {
    return this->m_manifest;
}

// get the FOTA manifest
uint32_t DeviceManagementResponder::getFOTAManifestLength() {
    return this->m_manifest_length;
}

// set the FOTA Image
void DeviceManagementResponder::setFOTAImage(void *fota_image,uint32_t fota_image_length) {
    this->m_fota_image = fota_image;
    this->m_fota_image_length = fota_image_length;
    if (this->m_fota_image_set_fn != NULL) {
        (*this->m_fota_image_set_fn)(this->m_endpoint,this->m_logger,this->m_fota_image,this->m_fota_image_length);
    }
}

// get the FOTA Image
void *DeviceManagementResponder::getFOTAImage() {
    return this->m_fota_image;
}

// get the FOTA Image Length
uint32_t DeviceManagementResponder::getFOTAImageLength() {
    return this->m_fota_image_length;
}

// ACTION: deregister device
void DeviceManagementResponder::deregisterDevice(const void *challenge) {
    // check endpoint
    if (this->m_endpoint != NULL) {        
        // authenticate
        if (this->authenticate(challenge)) {
            // DEBUG
            this->m_logger->log("DeviceManagementResponder(deregister): de-registering device...");

            // act
            ((Connector::Endpoint *)this->m_endpoint)->de_register_endpoint();
        }
        else {
            // authentication failure
            this->m_logger->log("DeviceManagementResponder(deregister): authentication failed. No action taken.");
        }
    }
    else {
        // no endpoint
        this->m_logger->log("DeviceManagementResponder(deregister): No endpoint instance. No action taken.");
    }
}

// ACTION: reboot device
void DeviceManagementResponder::rebootDevice(const void *challenge) {
    // check our Reboot Responder handler pointer
    if (this->m_reboot_responder_fn != NULL) {
        // check endpoint
        if (this->m_endpoint != NULL) {            
            // authenticate
            if (this->authenticate(challenge)) {
                // act
                (*this->m_reboot_responder_fn)((const void *)this->m_endpoint,(const void *)this->m_logger,NULL);
            }
            else {
                // authentication failure
                this->m_logger->log("DeviceManagementResponder(reboot): authentication failed. No action taken.");
            }
        }
        else {
            // no endpoint
            this->m_logger->log("DeviceManagementResponder(reboot): No endpoint instance. No action taken.");
        }
    }
    else {
        // no reset responder handler
        this->m_logger->log("DeviceManagementResponder(reboot): No reboot responder handler pointer. No action taken.");
    }
}

// ACTION: reset device
void DeviceManagementResponder::resetDevice(const void *challenge) {
    // check our Reset Responder handler pointer
    if (this->m_reset_responder_fn != NULL) {
        // check endpoint
        if (this->m_endpoint != NULL) {
            // authenticate
            if (this->authenticate(challenge)) {
                // act
                (*this->m_reset_responder_fn)((const void *)this->m_endpoint,(const void *)this->m_logger,NULL);
            }
            else {
                // authentication failure
                this->m_logger->log("DeviceManagementResponder(reset): authentication failed. No action taken.");
            }
        }
        else {
            // no endpoint
            this->m_logger->log("DeviceManagementResponder(reset): No endpoint instance. No action taken.");
        }
    }
    else {
        // no reset responder handler
        this->m_logger->log("DeviceManagementResponder(reset): No reset responder handler pointer. No action taken.");
    }
}

// ACTION: invoke FOTA
void DeviceManagementResponder::invokeFOTA(const void *challenge) {
    // check our FOTA invocation handler pointer
    if (this->m_fota_invocation_fn != NULL) {
        // check endpoint
        if (this->m_endpoint != NULL) {            
            // authenticate
            if (this->authenticate(challenge)) {
                // act
                (*this->m_fota_invocation_fn)((const void *)this->m_endpoint,(const void *)this->m_logger,this->getFOTAManifest());
            }
            else {
                // authentication failure
                this->m_logger->log("DeviceManagementResponder(FOTA): authentication failed. No action taken.");
            }
        }
        else {
            // no endpoint
            this->m_logger->log("DeviceManagementResponder(FOTA): No endpoint instance. No action taken.");
        }
    }
    else {
        // no FOTA invocation handler
        this->m_logger->log("DeviceManagementResponder(FOTA): No FOTA invocation handler pointer. No action taken.");
    }
}