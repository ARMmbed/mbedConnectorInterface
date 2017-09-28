/**
 * @file    DeviceManager.h
 * @brief   mbed CoAP Endpoint Device Management class
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

#ifndef __DEVICE_MANAGER_H__
#define __DEVICE_MANAGER_H__

// mbed-client support
#include "mbed-client/m2mconstants.h"
#include "mbed-client/m2mdevice.h"
#include "mbed-client/m2mfirmware.h"

// Action Resource: Device DeRegistration
#include "mbed-connector-interface/DeviceDeRegisterResource.h"

// LWM2M Device Info # of Device Resources we support
#define NUM_DEVICE_RESOURCES                9

// LWM2M Firmware Resources supported
#define NUM_FIRMWARE_RESOURCES              7

// LWM2M DeRegistration Object ID
#define LWM2M_DREGISTER_OBJ_ID              "86"

// LWM2M Device DeRegistration Resource ID
#define LWM2M_DEV_DEREGISTER_ID             "1"

/** DeviceManager is the endpoint management class
 */
class DeviceManager
{    
    public:
        // Optional Firmware Resource (indices)
        typedef enum {
            PackageName = 0,
            PackageVersion = 1, 
            Update = 2,
            Package = 3,
            PackageURI = 4,
            State = 5,
            UpdateResult = 6
        } FirmwareResources;
        
        // Optional Device Resource (indices)
        typedef enum {
            Manufacturer = 0,
            DeviceType = 1,
            ModelNumber = 2, 
            SerialNumber = 3,
            FirmwareVersion = 4,
            HardwareVersion = 5,
            SoftwareVersion = 6,
            Reboot = 7,
            FactoryReset = 8,
            DeRegistration = 9
        } DeviceResources;
        
        /**
        Default constructor
        @param logger input logger instance
        @param dm_responder input data management responder/processor
        @param mfg input endpoint manufacturer
        @param dev_type input the endpoint type
        @param model input the model of the endpoint
        @param serial input the serial of the endpoint
        @param fw_vers input the current firmware version
        @param hw_vers input the current hardware version
        @param sw_vers input the current software version
        */
        DeviceManager(const Logger *logger,const void *dm_responder=NULL,const char *mfg="ARM",const char *dev_type="mbed",const char *model="ARM" ,const char *serial="000000",const char *fw_ver="0.0.0",const char *hw_ver="0.0.0",const char *sw_ver="0.0.0");
        
        /**
        Copy constructor
        @param resource input the DeviceManager that is to be deep copied
        */
        DeviceManager(const DeviceManager &manager);
    
        /**
        Destructor
        */
        virtual ~DeviceManager();
        
        /**
        install the device manager into the endpoint
        @param endpoint input the endpoint instance
        @param config input the endpoint configuration instance
        */
        void install(const void *endpoint,const void *config);
        
        /**
        get the DeviceManagementResponder
        @return the DeviceManagementResponder or NULL
        */
        void *getResponder();
        
        // LWM2M Device: Reboot Action Handler
        void process_reboot_action(void *args);
        
        // LWM2M Device: Reset Action Handler 
        void process_reset_action(void *args);
        
        // LWM2M Firmware: Update
        void process_firmware_update_action(void *args);
        
        // bind our device resources   
        void bind();
        
        // Get a specific Firmware Resource
        M2MResource *getFirmwareResource(FirmwareResources res); 
        
        // Get a specific Device Resource
        M2MResource *getDeviceResource(DeviceResources res);
        
        // Get the Device Object
        M2MDevice *getDeviceObject();
        
        // Get the Firmware Object
        M2MFirmware *getFirmwareObject();
        
        // Process updated values
        void process(M2MBase *base, M2MBase::BaseType type);
        
    private:
        Logger                              *m_logger;
        void                                *m_endpoint;
        void                                *m_config;
        char                                *m_dev_type;
        void                                *m_dm_responder;
        char                                *m_mfg;
        char                                *m_model;
        char                                *m_serial;
        char                                *m_fw_vers;
        char                                *m_hw_vers;
        char                                *m_sw_vers;
        
        // LWM2M Firmware Data
        char                                *m_fw_manifest;
        uint32_t                             m_fw_manifest_length;
        void                                *m_fw_image;
        uint32_t                             m_fw_image_length;
        
        // LWM2M Device Info Resources
        M2MDevice                           *m_device;
        M2MResource                         *m_dev_res[NUM_DEVICE_RESOURCES];
        
        // LWM2M Firmware Resources
        M2MFirmware                         *m_firmware;
        M2MResource                         *m_fw_res[NUM_FIRMWARE_RESOURCES];
        
        // DeRegistation Resource
        DeviceDeRegisterResource            *m_deregister_resource; 
        
        // Memory utilities
        char *saveManifest(uint8_t *value,uint32_t length);
        void *saveImage(void *image,uint32_t image_length);
        
        // Bind our Device Resources to our Device Object
        void bindDeviceResources();
        
        // Bind our Firmware Resources to our Firmware Object
        void bindFirmwareResources();   
        
        // Bind our mbed Cloud Resource
        void bindMBEDCloudResources();
        
        // Get the Device Reboot Resource from the Device Object
        M2MResource *getDeviceRebootResource();
        
        // Get the Firmware Update Resource from the Firmware Object
        M2MResource *getFirmwareUpdateResource();
        
        // Get the Firmware Package Resource from the Firmware Object
        M2MResource *getFirmwarePackageResource();
        
        // Get the Firmware Package URI Resource from the Firmware Object
        M2MResource *getFirmwarePackageURIResource();
        
        // Get the Firmware State Resource from the Firmware Object
        M2MResource *getFirmwareStateResource();
        
        // Get the Firmware UpdateResult Resource from the Firmware Object
        M2MResource *getFirmwareUpdateResultResource();
        
        // Get a specific resource from a given resource object
        M2MResource *getResourceFromObject(M2MObject *obj,int instanceID,int resID);
};

#endif // __DEVICE_MANAGER_H__