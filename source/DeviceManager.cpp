/**
 * @file    DeviceManager.cpp
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
 
 // configuration
 #include "mbed-connector-interface/mbedConnectorInterface.h"

 // BaseClass 
 #include "mbed-connector-interface/DeviceManager.h"
 
 // Endpoint Class
 #include "mbed-connector-interface/ConnectorEndpoint.h"
 
 // Options Builder
 #include "mbed-connector-interface/OptionsBuilder.h"
 
 // Device Management Responder
 #include "mbed-connector-interface/DeviceManagementResponder.h"
 
// Constructor
DeviceManager::DeviceManager(const Logger *logger,const void *dm_responder,const char *mfg,const char *dev_type,const char *model,const char *serial,const char *fw_vers,const char *hw_vers,const char *sw_vers) {
    // record the data management responder if we have one
    this->m_dm_responder = (void *)dm_responder;
    
    // save off for later
    this->m_dev_type = (char *)dev_type;
    this->m_logger = (Logger *)logger;
    this->m_mfg = (char *)mfg;
    this->m_model = (char *)model;
    this->m_serial = (char *)serial;
    this->m_fw_vers = (char *)fw_vers;
    this->m_sw_vers = (char *)sw_vers;
    this->m_hw_vers = (char *)hw_vers;
    
    // initialize
    this->m_device = NULL;
    for(int i=0;i<NUM_DEVICE_RESOURCES;++i) {
        this->m_dev_res[i] = NULL;
    }
    for(int i=0;i<NUM_FIRMWARE_RESOURCES;++i) {
        this->m_fw_res[i] = NULL;
    }
    this->m_deregister_resource = NULL;
    this->m_endpoint = NULL;
    this->m_config = NULL;
    this->m_fw_manifest = NULL;
    this->m_fw_manifest_length = 0;
    this->m_fw_image = NULL;
    this->m_fw_image_length = 0;
}

// Copy constructor
DeviceManager::DeviceManager(const DeviceManager &manager) {
    for(int i=0;i<NUM_DEVICE_RESOURCES;++i) {
        this->m_dev_res[i] = manager.m_dev_res[i];
    }
    for(int i=0;i<NUM_FIRMWARE_RESOURCES;++i) {
        this->m_fw_res[i] = manager.m_fw_res[i];
    }
    this->m_dm_responder = manager.m_dm_responder;
    this->m_device = manager.m_device;
    this->m_dev_type = manager.m_dev_type;
    this->m_logger = manager.m_logger;
    this->m_endpoint = manager.m_endpoint;
    this->m_config = manager.m_config;
    this->m_mfg = manager.m_mfg;
    this->m_model = manager.m_model;
    this->m_serial = manager.m_serial;
    this->m_fw_vers = manager.m_fw_vers;
    this->m_sw_vers = manager.m_sw_vers;
    this->m_hw_vers = manager.m_hw_vers;
    this->m_deregister_resource = manager.m_deregister_resource;
    this->m_fw_manifest = this->saveManifest((uint8_t *)manager.m_fw_manifest,manager.m_fw_manifest_length);
    this->m_fw_manifest_length = manager.m_fw_manifest_length;
    this->m_fw_image = this->saveImage(manager.m_fw_image,manager.m_fw_image_length);
    this->m_fw_image_length = manager.m_fw_image_length;
}

// Destructor
DeviceManager::~DeviceManager() {
    if (this->m_fw_manifest != NULL) {
        free(this->m_fw_manifest);
    }
    if (this->m_fw_image != NULL) {
        free(this->m_fw_image);
    }
    this->m_fw_manifest = NULL;
    this->m_fw_manifest_length = 0;
    this->m_fw_image = NULL;
    this->m_fw_image_length = 0;
}

// save the fota manifest
char *DeviceManager::saveManifest(uint8_t *manifest,uint32_t manifest_length) {
    if (manifest != NULL && manifest_length > 0) {
        if (this->m_fw_manifest != NULL) {
            free(this->m_fw_manifest);
        }
        this->m_fw_manifest = (char *)malloc(manifest_length+1);
        memset(this->m_fw_manifest,0,manifest_length+1);
        memcpy(this->m_fw_manifest,manifest,manifest_length);
        this->m_fw_manifest_length = manifest_length;
    }
    return this->m_fw_manifest;
}

// save the fota image
void *DeviceManager::saveImage(void *image,uint32_t image_length) {
    if (image != NULL && image_length > 0) {
        if (this->m_fw_image != NULL) {
            free(this->m_fw_image);
        }
        this->m_fw_image = (char *)malloc(image_length);
        memset(this->m_fw_image,0,image_length);
        memcpy(this->m_fw_image,image,image_length);
    }
    return this->m_fw_image;
}

// bind the device resources
void DeviceManager::bindDeviceResources() {
    // our Endpoint configuration
    Connector::OptionsBuilder *cfg = (Connector::OptionsBuilder *)this->m_config;
      
    // establish the default base LWM2M device info resource values
    this->m_device = M2MInterfaceFactory::create_device();
    if (this->m_device != NULL) {
        this->m_dev_res[0] = this->m_device->create_resource(M2MDevice::Manufacturer,this->m_mfg);            // Manufacturer
        this->m_dev_res[1] = this->m_device->create_resource(M2MDevice::DeviceType,this->m_dev_type);         // Device Type
        this->m_dev_res[2] = this->m_device->create_resource(M2MDevice::ModelNumber,this->m_model);           // Device Model
        this->m_dev_res[3] = this->m_device->create_resource(M2MDevice::SerialNumber,this->m_serial);         // Device Serial
        this->m_dev_res[4] = this->m_device->create_resource(M2MDevice::FirmwareVersion,this->m_fw_vers);     // Firmware Version
        this->m_dev_res[5] = this->m_device->create_resource(M2MDevice::HardwareVersion,this->m_hw_vers);     // Hardware Version
        this->m_dev_res[6] = this->m_device->create_resource(M2MDevice::SoftwareVersion,this->m_sw_vers);     // Software Version
        this->m_dev_res[7] = this->getDeviceRebootResource();                                                 // Reboot
        this->m_dev_res[8] = this->m_device->create_resource(M2MDevice::FactoryReset);                        // Reset
                
        // set the callback functions for Reboot and Reset
        if (this->m_dev_res[7] != NULL) {
            this->m_dev_res[7]->set_operation(M2MBase::POST_ALLOWED);
            this->m_dev_res[7]->set_execute_function(execute_callback(this,&DeviceManager::process_reboot_action));
        }
        if (this->m_dev_res[8] != NULL) {
            this->m_dev_res[8]->set_operation(M2MBase::POST_ALLOWED);
            this->m_dev_res[8]->set_execute_function(execute_callback(this,&DeviceManager::process_reset_action));
        }
        
        // set the Device Resources Object
        if (cfg != NULL) {
            cfg->setDeviceResourcesObject(this->m_device); // device resources created under single device object... so just set it
        }
    }
    else {
        // unable to create the device object
        this->m_logger->log("DeviceManager::bindDeviceResources(): Unable to create Device Object!");
    }  
}

// bind the firmware resources
void DeviceManager::bindFirmwareResources() {
    // our Endpoint configuration
    Connector::OptionsBuilder *cfg = (Connector::OptionsBuilder *)this->m_config;
    
    // establish the default base LWM2M firmware info resource values
    this->m_firmware = M2MInterfaceFactory::create_firmware();
    if (this->m_firmware != NULL) {
        // Create our optional resources
        this->m_fw_res[0] = this->m_firmware->create_resource(M2MFirmware::PackageName,"");                     // Package Name
        this->m_fw_res[1] = this->m_firmware->create_resource(M2MFirmware::PackageVersion,"");                  // Package Version
        
        // Get the speciality resources
        this->m_fw_res[2] = this->getFirmwareUpdateResource();
        this->m_fw_res[3] = this->getFirmwarePackageResource();
        this->m_fw_res[4] = this->getFirmwarePackageURIResource();
        this->m_fw_res[5] = this->getFirmwareStateResource();
        this->m_fw_res[6] = this->getFirmwareUpdateResultResource();
                
        // set the callback functions for Update
        if (this->m_fw_res[2] != NULL) {
            this->m_fw_res[2]->set_operation(M2MBase::POST_ALLOWED);
            this->m_fw_res[2]->set_execute_function(execute_callback(this,&DeviceManager::process_firmware_update_action));
        }
    
        // set the Firmware Resources Object
        if (cfg != NULL) {
            cfg->setFirmwareResourcesObject(this->m_firmware); // firmware resources created under single firmware object... so just set it
        }
    }
    else {
        // unable to create the firmware object
        this->m_logger->log("DeviceManager::bindFirmwareResources(): Unable to create Firmware Object!");
    }
}

// bind mbed Cloud resources
void DeviceManager::bindMBEDCloudResources() {
    // our Endpoint configuration
    Connector::OptionsBuilder *cfg = (Connector::OptionsBuilder *)this->m_config;
    
    // Add mbed Cloud resources here...
          
    // mbed Cloud DeRegistration Resource
    this->m_deregister_resource = new DeviceDeRegisterResource(this->m_logger,LWM2M_DREGISTER_OBJ_ID,LWM2M_DEV_DEREGISTER_ID,this->m_dm_responder);
    if (this->m_deregister_resource != NULL) {
        cfg->addResource(this->m_deregister_resource);              // de-registration action resource added
    }
}

// setup the Device Manager
void DeviceManager::bind() {
    // Bind our Device Resources
    this->bindDeviceResources();
    
    // Bind our Firmware Resources
    this->bindFirmwareResources();
    
    // Bind our mbed Cloud Resources
    this->bindMBEDCloudResources();
}

// process updated values (PUT): The only updatable device management resources are: Firmware::Package, Firmware::PackageURI
void DeviceManager::process(M2MBase *base, M2MBase::BaseType type) {
    // DeviceManagementResponder
    DeviceManagementResponder *dmr = (DeviceManagementResponder *)this->m_dm_responder;

    // PackageURI handler
    if (base == this->getFirmwareResource(PackageURI)) {
        // PackageURI resource
        M2MResource *res = this->getFirmwareResource(PackageURI);
        
        // Save off the manifest
        this->saveManifest(res->value(),res->value_length());
        
        // DEBUG
        this->m_logger->log("DeviceManager::process(PUT): Setting FOTA Manifest: [%s] type: %d length: %d",this->m_fw_manifest,type,this->m_fw_manifest_length);
        
        // Manifest Updated
        dmr->setFOTAManifest(this->m_fw_manifest,this->m_fw_manifest_length);
    }
    
    // Package handler
    if (base == this->getFirmwareResource(Package)) {
        // FOTA Image (direct) Updated
        M2MResource *res = this->getFirmwareResource(PackageURI);
        
        // DEBUG
        this->m_logger->log("DeviceManager::process(PUT): Setting FOTA Image. Length=%d type: %d",res->value_length(),type);
        
        // FOTA Image updated
        dmr->setFOTAImage(res->value(),res->value_length());
    }
}

// Get the Device Reboot Resource from the Device Object
M2MResource *DeviceManager::getDeviceRebootResource() {
    if (this->m_device != NULL) {
        // Get /3/0/3
        return this->getResourceFromObject(this->m_device,0,M2MDevice::Reboot);
    }
    return NULL;
}

// Get the Firmware Update Resource from the Firmware Object
M2MResource *DeviceManager::getFirmwareUpdateResource() {
    if (this->m_firmware != NULL) {
        // Get /5/0/2
        return this->getResourceFromObject(this->m_firmware,0,M2MFirmware::Update);
    }
    return NULL;
}

// Get the Firmware Package Resource from the Firmware Object
M2MResource *DeviceManager::getFirmwarePackageResource() {
    if (this->m_firmware != NULL) {
        // Get /5/0/0
        return this->getResourceFromObject(this->m_firmware,0,M2MFirmware::Package);
    }
    return NULL;
}

// Get the Firmware Package URI Resource from the Firmware Object
M2MResource *DeviceManager::getFirmwarePackageURIResource() {
    if (this->m_firmware != NULL) {
        // Get /5/0/1
        return this->getResourceFromObject(this->m_firmware,0,M2MFirmware::PackageUri);
    }
    return NULL;
}

// Get the Firmware State Resource from the Firmware Object
M2MResource *DeviceManager::getFirmwareStateResource() {
    if (this->m_firmware != NULL) {
        // Get /5/0/3
        return this->getResourceFromObject(this->m_firmware,0,M2MFirmware::State);
    }
    return NULL;
}

// Get the Firmware UpdateResult Resource from the Firmware Object
M2MResource *DeviceManager::getFirmwareUpdateResultResource() {
    if (this->m_firmware != NULL) {
        // Get /5/0/5
        return this->getResourceFromObject(this->m_firmware,0,M2MFirmware::UpdateResult);
    }
    return NULL;
}
// Get a specific resource from a resource object
M2MResource *DeviceManager::getResourceFromObject(M2MObject *obj,int instanceID,int resID) {
    if (obj != NULL) {
        M2MObjectInstanceList instances = obj->instances();
        if (instances.size() > 0 && instances.size() > instanceID) {
            M2MObjectInstance *instance = instances[instanceID];
            if (instance != NULL) {
                M2MResourceList resources = instance->resources();
                if (resources.size() > 0 && resources.size() > resID) {
                    M2MResource *resource = resources[resID];
                    return resource;
                }
            }
        }
    }
    return NULL;
}

// Get the Device Object
 M2MDevice *DeviceManager::getDeviceObject() {
    return this->m_device;
}

// Get the Firmware Object
M2MFirmware *DeviceManager::getFirmwareObject() {
    return this->m_firmware;
}

// extract a specific firmware resource
M2MResource *DeviceManager::getFirmwareResource(FirmwareResources res) {
    // indexed optional resources
    int index = (int)res;
    if (index >= 0 && index <NUM_FIRMWARE_RESOURCES) {
        return this->m_fw_res[index];
    }
    return NULL;
}

// extract a specific device resource
M2MResource *DeviceManager::getDeviceResource(DeviceResources res) {
    // special case: DeRegistration
    if (res == DeRegistration){
        return this->m_deregister_resource->getResource();
    }
    
    // indexed optional resources
    int index = (int)res;
    if (index >= 0 && index <NUM_DEVICE_RESOURCES) {
        return this->m_dev_res[index];
    }
    return NULL;
}

// Install the device manager into the Connector Endpoint
void DeviceManager::install(const void *endpoint,const void *config) {
    // record the configuration
    this->m_config = (void *)config;
    
    // record the endpoint
    this->m_endpoint = (void *)endpoint;
    
    // set the endpoint type
    Connector::OptionsBuilder *cfg = (Connector::OptionsBuilder *)this->m_config;
    if (cfg != NULL) {
        // set our device type
        cfg->setEndpointType(this->m_dev_type);
    }
            
    // establish connection to our responder
    if (this->m_dm_responder != NULL) {
        ((DeviceManagementResponder *)this->m_dm_responder)->setEndpoint(this->m_endpoint);
    }        
}

// get our device management responder
void *DeviceManager::getResponder() { 
    return this->m_dm_responder; 
}

// process our reboot action
void DeviceManager::process_reboot_action(void *args) {
    if (this->m_dm_responder != NULL) {
        ((DeviceManagementResponder *)this->m_dm_responder)->rebootDevice(args);
    }
    else {
        // no device management responder instance
        this->m_logger->log("DeviceManager::process_reboot_action: DeviceManagementResponder is NULL. No reboot processed");
    }
}

// process our reset action
void DeviceManager::process_reset_action(void *args) {
    if (this->m_dm_responder != NULL) {
        ((DeviceManagementResponder *)this->m_dm_responder)->resetDevice(args);
    }
    else {
        // no device management responder instance
        this->m_logger->log("DeviceManager:: process_reset_action: DeviceManagementResponder is NULL. No reset processed");
    }
}

// process our firmware update action
void DeviceManager::process_firmware_update_action(void *args) {
    if (this->m_dm_responder != NULL) {
        ((DeviceManagementResponder *)this->m_dm_responder)->invokeFOTA(args);
    }
    else {
        // no device management responder instance
        this->m_logger->log("DeviceManager:: process_firmware_update_action: DeviceManagementResponder is NULL. No firmware update action processed");
    }
}