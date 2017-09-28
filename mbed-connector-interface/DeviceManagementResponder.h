/**
 * @file    DeviceManagementResponder.h
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

#ifndef __DEVICE_MANAGEMENT_RESPONDER_H__
#define __DEVICE_MANAGEMENT_RESPONDER_H__

// Logger
#include "mbed-connector-interface/Logger.h"

// Authenticator Support
#include "mbed-connector-interface/Authenticator.h"

// invocation handler signature
typedef void (*initialize_fn)(const void *logger);
typedef bool (*manifest_fn)(const void *ep,const void *logger,const void *manifest,uint32_t manifest_length);
typedef bool (*image_set_fn)(const void *ep,const void *logger,const void *image,uint32_t image_length);   
typedef bool (*responder_fn)(const void *ep,const void *logger,const void *data); 

class DeviceManagementResponder {
    public:
        /**
        Default constructor
        @param logger input logger instance
        @param authenticator input authentication instance
        */
        DeviceManagementResponder(const Logger *logger,const Authenticator *authenticator);
        
        /**
        Copy constructor
        @param resource input the DeviceManagementResponder that is to be deep copied
        */
        DeviceManagementResponder(const DeviceManagementResponder &manager);
    
        /**
        Destructor
        */
        virtual ~DeviceManagementResponder();
        
        /**
        Set the Endpoint instance
        @param ep input the endpoint instance pointer
        */
        void setEndpoint(const void *ep);
        
        /** 
        Set our Initialize handler function
        @param initialize_fn input the device initialize function pointer
        */
        void setInitializeHandler(initialize_fn initialize_fn);
        
        /** 
        Set our Reboot Responder handler function
        @param reboot_responder_fn input the device reboot responder function pointer
        */
        void setRebootResponderHandler(responder_fn reboot_responder_fn);
        
        /** 
        Set our Reset Responder handler function
        @param reset_responder_fn input the device reset responder function pointer
        */
        void setResetResponderHandler(responder_fn reset_responder_fn);
        
        /** 
        Set our FOTA manifest handler function
        @param fota_manifest_fn input the FOTA manifest handler function pointer
        */
        void setFOTAManifestHandler(manifest_fn fota_manifest_fn);
        
        /** 
        Set our FOTA image set handler function
        @param fota_image_set_fn input the FOTA image set function pointer
        */
        void setFOTAImageHandler(image_set_fn fota_image_set_fn);
        
        /** 
        Set our FOTA invocation handler function
        @param fota_invocation_fn input the FOTA invocation handler function pointer
        */
        void setFOTAInvocationHandler(responder_fn fota_invocation_fn);
        
        /**
        ACTION: Deregister device
        @param challenge input the input authentication challenge
        */
        virtual void deregisterDevice(const void *challenge);
        
        /**
        ACTION: Reboot device
        @param challenge input the input authentication challenge
        */
        virtual void rebootDevice(const void *challenge);
        
        /**
        ACTION: Reset device 
        @param challenge input the input authentication challenge
        */
        virtual void resetDevice(const void *challenge);
        
        /**
        Set our FOTA manifest
        @param manifest input the FOTA manifest
        @param manifest_length input the length of the FOTA manifest
        */
        virtual void setFOTAManifest(char *manifest,uint32_t manifest_length);
        
        /**
        Get our FOTA manifest
        @return the FOTA manifest
        */
        virtual char *getFOTAManifest();
        
        /**
        Get our FOTA manifest length
        @return the FOTA manifest length
        */
        virtual uint32_t getFOTAManifestLength();
        
        /**
        Set our FOTA image
        @param fota_image input the FOTA image
        @param fota_image_length input the length of the fota image
        */
        virtual void setFOTAImage(void *fota_image,uint32_t fota_image_length);
        
        /**
        Get our FOTA image
        @return the FOTA image
        */
        virtual void *getFOTAImage();
        
        /**
        Get our FOTA image length
        @return the FOTA image length
        */
        virtual uint32_t getFOTAImageLength();
        
        /**
        ACTION: Invoke FOTA (default: empty action)
        @param challenge input the input authentication challenge
        */
        virtual void invokeFOTA(const void *challenge);
    
    private:
        Logger                      *m_logger;
        Authenticator               *m_authenticator;
        void                        *m_endpoint;
        char                        *m_manifest;
        int                          m_manifest_length;
        void                        *m_fota_image;
        uint32_t                     m_fota_image_length;
        
        initialize_fn                m_initialize_fn;
        responder_fn                 m_reboot_responder_fn;
        responder_fn                 m_reset_responder_fn;
        manifest_fn                  m_fota_manifest_fn;
        image_set_fn                 m_fota_image_set_fn;
        responder_fn                 m_fota_invocation_fn;
        
        bool                         authenticate(const void *challenge);
};

#endif // __DEVICE_MANAGEMENT_RESPONDER_H__