/**
 * @file    ConnectorEndpoint.cpp
 * @brief   mbed CoAP Endpoint base class
 * @author  Doug Anson/Chris Paola
 * @version 1.0
 * @see
 *
 * Copyright (c) 2018
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

// Lower level Network interface
#include "mbed-connector-interface/mbedEndpointNetwork.h"

// ConnectorEndpoint
#include "mbed-connector-interface/ConnectorEndpoint.h"

// Utils support
#include "mbed-connector-interface/Utils.h"

// Device Manager support
#include "mbed-connector-interface/DeviceManager.h"

// trace configuration
#include "mbed-trace/mbed_trace.h"

// updater support
#include "update_ui_example.h"

// factory flow support
#include "factory_configurator_client.h"

// our endpoint instance
static Connector::Endpoint *__endpoint = NULL;

// LWIP Network interface instance
NetworkInterface *__network_interface = NULL;

// Connector namespace
namespace Connector {

// STATIC: Plumb the network
void Endpoint::plumbNetwork(void *device_manager, bool canActAsRouterNode) {
	// create our endpoint instance...
	if (__endpoint == NULL) {
		// initialize our endpoint instance
		printf("Connector::Endpoint::plumbNetwork: initializing endpoint instance...\r\n");
		__endpoint = (Connector::Endpoint *) utils_init_endpoint(canActAsRouterNode);
	}

	// make sure we have an endpoint...
	if (__endpoint != NULL) {
		// set the device manager
		if (device_manager != NULL) {
			// device manager has been supplied
			printf("Connector::Endpoint::plumbNetwork: setting a device manager...\r\n");
			__endpoint->setDeviceManager(device_manager);
		}

		// configure the endpoint...
		printf("Connector::Endpoint::plumbNetwork: configuring endpoint...\r\n");
		utils_configure_endpoint((void *) __endpoint);

		// plumb the endpoint's network...
		printf("Connector::Endpoint::plumbNetwork: plumbing network...\r\n");
		net_plumb_network((void *) __endpoint);
	}
}

// STATIC: Finalize the endpoint's configuration and begin the endpoint's main even loop (static, not tied into Logger)
void Endpoint::start() {
	if (__endpoint != NULL) {
		// build out the endpoint with its configuration...
		printf("Connector::Endpoint::start: building out endpoint...\r\n");
		utils_build_endpoint((void *) __endpoint);

		// make sure we have an endpoint interface before continuing...
		if (__endpoint != NULL && __endpoint->getEndpointInterface() != NULL) {
			// finalize the endpoint and start its main loop
			printf(
					"Endpoint::start: finalize and run the endpoint main loop..\r\n");
			net_finalize_and_run_endpoint_main_loop((void *) __endpoint);
		} else {
			// not starting the endpoint due to errors
			printf(
					"Connector::Endpoint::start: Not starting endpoint due to errors (no endpoint interface)... exiting...\r\n");

			// end in error...
			while (true) {
				ThisThread::sleep_for(1000);
			}
		}
	} else {
		// not starting the endpoint due to errors
		printf(
				"Connector::Endpoint::start: Not starting endpoint due to errors (no endpoint)... exiting...\r\n");

		// end in error...
		while (true) {
			ThisThread::sleep_for(1000);
		}
	}
}

// STATIC: Set the ConnectionStatusInterface Implementation instance
void Endpoint::setConnectionStatusInterface(ConnectionStatusInterface *csi) {
	if (__endpoint != NULL) {
		__endpoint->setConnectionStatusInterfaceImpl(csi);
	}
}

// Constructor
Endpoint::Endpoint(const Logger *logger, const Options *options) :
		MbedCloudClientCallback(), M2MInterfaceObserver()
{
	this->m_logger = (Logger *) logger;
	this->m_options = (Options *) options;
	this->m_device_manager = NULL;
	this->m_connected = false;
	this->m_registered = false;
	this->m_csi = NULL;
	this->m_oim = NULL;
	this->m_endpoint_interface = NULL;
}

// Copy Constructor
Endpoint::Endpoint(const Endpoint &ep) {
	this->m_logger = ep.m_logger;
	this->m_options = ep.m_options;
	this->m_endpoint_interface = ep.m_endpoint_interface;
	this->m_endpoint_object_list = ep.m_endpoint_object_list;
	this->m_device_manager = ep.m_device_manager;
	this->m_connected = ep.m_connected;
	this->m_registered = ep.m_registered;
	this->m_csi = ep.m_csi;
	this->m_oim = ep.m_oim;
}

// Destructor
Endpoint::~Endpoint() {
}

// set the device manager
void Endpoint::setDeviceManager(void *device_manager) {
	this->m_device_manager = device_manager;
}

// get the device manager
void *Endpoint::getDeviceManager(void) {
	return this->m_device_manager;
}

// router node behavior setting
void Endpoint::asRouterNode(bool canActAsRouterNode) {
	this->m_canActAsRouterNode = canActAsRouterNode;
}

// set our Options
void Endpoint::setOptions(Options *options) {
	this->m_options = options;
}

// get our Options
Options *Endpoint::getOptions() {
	return this->m_options;
}

// get our ObjectList
M2MObjectList Endpoint::getEndpointObjectList() {
	return this->m_endpoint_object_list;
}

// get our endpoint interface
MbedCloudClient *Endpoint::getEndpointInterface() {
	return this->m_endpoint_interface;
}

// Connector::Endpoint: create our interface
void Endpoint::createEndpointInterface() {
	this->createCloudEndpointInterface();
}

// mbedCloudClient: initialize the platform
bool Endpoint::initializePlatform() {
	// initialize the underlying platform
	this->logger()->log("initializePlatform: initializing underlying platform...");
	if (utils_init_platform() != true) {
		this->logger()->log("initializePlatform: ERROR: utils_init_platform() failed!");
		return false;
	}

	// DEBUG
	this->logger()->log("initializePlatform: platform initialized SUCCESS!");
	return true;
}

// mbedCloudClient: initialize provisioning flow
bool Endpoint::initializeProvisioningFlow() {
     this->logger()->log("iinitializeProvisioningFlow: initializing provisioning flow...");
     if (utils_init_provisioning_flow() != true) {
          this->logger()->log("initializeProvisioningFlow: ERROR utils_init_provisioning_flow() failed");
          return false;
     }

     // DEBUG
     this->logger()->log("initializeProvisioningFlow: provisioning flow initialized SUCCESS!");
     return true;
}

// mbedCloudClient: create our interface
void Endpoint::createCloudEndpointInterface() {
	if (this->m_endpoint_interface == NULL) {
		bool platform_init = this->initializePlatform();
		bool provisioning_flow_init = this->initializeProvisioningFlow();
		if (platform_init && provisioning_flow_init) {
			// create a new instance of mbedCloudClient
			this->logger()->log("createCloudEndpointInterface: creating mbed cloud client instance...");
			this->m_endpoint_interface = new MbedCloudClient();
			if (this->m_endpoint_interface == NULL) {
				// unable to allocate the MbedCloudClient instance
				this->logger()->log("createCloudEndpointInterface: ERROR: unable to allocate MbedCloudClient instance...");
			} else {
#ifdef MBED_CLOUD_CLIENT_SUPPORT_UPDATE		
				// Establish the updater hook in MbedCloudClient
				update_ui_set_cloud_client(this->m_endpoint_interface);

				// Update Authorize Handler
				this->m_endpoint_interface->set_update_authorize_handler(&update_authorize);

				// Update Progress Handler (optional, disabled by default)
				this->m_endpoint_interface->set_update_progress_handler(&update_progress);
#endif
			}
		} else {
			if (platform_init) {
				// unable to create mbed cloud client instance... (FAILED provisioning flow init)
				this->logger()->log("createCloudEndpointInterface: ERROR: unable to initialize provisioning flow...");
			} else {
				// unable to create mbed cloud client instance... (FAILED platform init)
				this->logger()->log("createCloudEndpointInterface: ERROR: unable to initialize platform...");
			}
			this->m_endpoint_interface = NULL;
		}
	}

	// bind LWIP network interface pointer...
	if (__network_interface != NULL && this->m_endpoint_interface != NULL) {
		this->logger()->log("Connector::Endpoint: binding LWIP network instance (Cloud)...");
		this->m_endpoint_interface->on_registered(
				&Connector::Endpoint::on_registered);
		this->m_endpoint_interface->on_unregistered(
				&Connector::Endpoint::on_unregistered);
		this->m_endpoint_interface->on_error(&Connector::Endpoint::on_error);
		this->m_endpoint_interface->set_update_callback(this);
	} else {
		// skipping LWIP bind...
		this->logger()->log("Connector::Endpoint: ERROR (Cloud) skipping LWIP network instance bind due to previous error...");
	}
}

// Callback from mbed client stack if any error is encountered
void Endpoint::on_error(int error_code) {
	char *error = (char *) "No Error";
	switch (error_code) {
	case MbedCloudClient::ConnectErrorNone:
		error = (char *) "MbedCloudClient::ConnectErrorNone";
		break;
	case MbedCloudClient::ConnectAlreadyExists:
		error = (char *) "MbedCloudClient::ConnectAlreadyExists";
		break;
	case MbedCloudClient::ConnectBootstrapFailed:
		error = (char *) "MbedCloudClient::ConnectBootstrapFailed";
		break;
	case MbedCloudClient::ConnectInvalidParameters:
		error = (char *) "MbedCloudClient::ConnectInvalidParameters";
		break;
	case MbedCloudClient::ConnectNotRegistered:
		error = (char *) "MbedCloudClient::ConnectNotRegistered";
		break;
	case MbedCloudClient::ConnectTimeout:
		error = (char *) "MbedCloudClient::ConnectTimeout";
		break;
	case MbedCloudClient::ConnectNetworkError:
		error = (char *) "MbedCloudClient::ConnectNetworkError";
		break;
	case MbedCloudClient::ConnectResponseParseFailed:
		error = (char *) "MbedCloudClient::ConnectResponseParseFailed";
		break;
	case MbedCloudClient::ConnectUnknownError:
		error = (char *) "MbedCloudClient::ConnectUnknownError";
		break;
	case MbedCloudClient::ConnectMemoryConnectFail:
		error = (char *) "MbedCloudClient::ConnectMemoryConnectFail";
		break;
	case MbedCloudClient::ConnectNotAllowed:
		error = (char *) "MbedCloudClient::ConnectNotAllowed";
		break;
	case MbedCloudClient::ConnectSecureConnectionFailed:
		error = (char *) "MbedCloudClient::ConnectSecureConnectionFailed";
		break;
	case MbedCloudClient::ConnectDnsResolvingFailed:
		error = (char *) "MbedCloudClient::ConnectDnsResolvingFailed";
		break;
#ifdef MBED_CLOUD_CLIENT_SUPPORT_UPDATE
	case UpdateClient::WarningCertificateNotFound:
		error = (char *) "MbedCloudClient(Update): WarningCertificateNotFound";
		break;
	case UpdateClient::WarningIdentityNotFound:
		error = (char *) "MbedCloudClient(Update): WarningIdentityNotFound";
		break;
	case UpdateClient::WarningCertificateInvalid:
		error = (char *) "MbedCloudClient(Update): WarningCertificateInvalid";
		break;
	case UpdateClient::WarningSignatureInvalid:
		error = (char *) "MbedCloudClient(Update): WarningSignatureInvalid";
		break;
	case UpdateClient::WarningVendorMismatch:
		error = (char *) "MbedCloudClient(Update): WarningVendorMismatch";
		break;
	case UpdateClient::WarningClassMismatch:
		error = (char *) "MbedCloudClient(Update): WarningClassMismatch";
		break;
	case UpdateClient::WarningDeviceMismatch:
		error = (char *) "MbedCloudClient(Update): WarningDeviceMismatch";
		break;
	case UpdateClient::WarningURINotFound:
		error = (char *) "MbedCloudClient(Update): WarningURINotFound";
		break;
	case UpdateClient::WarningRollbackProtection:
		error =
				(char *) "MbedCloudClient(Update): WarningRollbackProtection. Manifest is older than currently running image.";
		break;
	case UpdateClient::WarningUnknown:
		error = (char *) "MbedCloudClient(Update): WarningUnknown";
		break;
	case UpdateClient::ErrorWriteToStorage:
		error = (char *) "MbedCloudClient(Update): ErrorWriteToStorage";
		break;
#endif
	default:
		error = (char *) "UNKNOWN";
	}
	printf("Connector::Endpoint(Cloud) Error(%x): %s\r\n", error_code, error);
}

// mbed-client: Callback from mbed client stack if any error is encountered
void Endpoint::error(M2MInterface::Error error) {
    switch (error) {
        case M2MInterface::AlreadyExists:
                this->logger()->log("Connector::Endpoint(ERROR): M2MInterface::AlreadyExists");
                break;
        case M2MInterface::BootstrapFailed:
                this->logger()->log("Connector::Endpoint(ERROR): M2MInterface::BootstrapFailed");
                break;
        case M2MInterface::InvalidParameters:
                this->logger()->log("Connector::Endpoint(ERROR): M2MInterface::InvalidParameters");
                break;
        case M2MInterface::NotRegistered:
                this->logger()->log("Connector::Endpoint(ERROR): M2MInterface::NotRegistered");
                break;
        case M2MInterface::Timeout:
                this->logger()->log("Connector::Endpoint(ERROR): M2MInterface::Timeout");
                break;
        case M2MInterface::NetworkError:
                this->logger()->log("Connector::Endpoint(ERROR): M2MInterface::NetworkError");
                break;
        case M2MInterface::ResponseParseFailed:
                this->logger()->log("Connector::Endpoint(ERROR): M2MInterface::ResponseParseFailed");
                break;
        case M2MInterface::UnknownError:
                this->logger()->log("Connector::Endpoint(ERROR): M2MInterface::UnknownError");
                break;
        case M2MInterface::MemoryFail:
                this->logger()->log("Connector::Endpoint(ERROR): M2MInterface::MemoryFail");
                break;
        case M2MInterface::NotAllowed:
                this->logger()->log("Connector::Endpoint(ERROR): M2MInterface::NotAllowed");
                break;
        default:
                break;
        }
}

// re-register the endpoint
void Endpoint::re_register_endpoint() {
	if (this->m_endpoint_interface != NULL) {
		// DEBUG
		this->logger()->log("Connector::Endpoint(Cloud): re-register endpoint...");
	}
}

// de-register endpoint
void Endpoint::de_register_endpoint(void) {
	if (this->m_endpoint_interface != NULL) {
		// DEBUG
		this->logger()->log("Connector::Endpoint(Cloud): de-registering endpoint...");
		this->m_endpoint_interface->close();
	}
}

// register the endpoint
void Endpoint::register_endpoint(M2MSecurity *endpoint_security,
		M2MObjectList endpoint_objects) {
	if (this->m_endpoint_interface != NULL) {
		this->logger()->log("Connector::Endpoint(Cloud): adding objects to endpoint...");
		this->m_endpoint_interface->add_objects(endpoint_objects);

		this->logger()->log("Connector::Endpoint(Cloud): registering endpoint...");
		this->m_endpoint_interface->setup(__network_interface);
	}
}

// object registered
void Endpoint::on_registered() {
	if (__endpoint != NULL) {
		printf("Connector::Endpoint(Cloud): on_registered()\r\n");
		__endpoint->object_registered();
	}
}

// registration updated
void Endpoint::on_registration_updated() {
	if (__endpoint != NULL) {
		printf("Connector::Endpoint(Cloud): on_registration_updated()\r\n");
		__endpoint->registration_updated();
	}
}

// object unregistered
void Endpoint::on_unregistered() {
	if (__endpoint != NULL) {
		printf("Connector::Endpoint(Cloud): on_unregistered()\r\n");
		__endpoint->object_unregistered(NULL);
	}
}

// object registered
void Endpoint::object_registered(M2MSecurity *security,
		const M2MServer &server) {
	this->object_registered((void *) security, (void *) &server);
}

// registration updated
void Endpoint::registration_updated(M2MSecurity *security,
		const M2MServer &server) {
	this->registration_updated((void *) security, (void *) &server);
}

// object unregistered
void Endpoint::object_unregistered(M2MSecurity *security) {
	// DEBUG
	this->logger()->log("Connector::Endpoint: endpoint de-registered.");

	// no longer connected/registered
	this->m_registered = false;
	this->m_connected = false;

	// stop all observers...
	this->stopObservations();

	// invoke ConnectionHandler if we have one...
	if (this->m_csi != NULL) {
		this->m_csi->object_unregistered((void *) this, (void *) security);
	}

	// halt the main event loop... we are done.
	net_shutdown_endpoint();
}

// bootstrap done
void Endpoint::bootstrap_done(M2MSecurity *security) {
	this->logger()->log("Connector::Endpoint: endpoint bootstrapped.");
	if (this->m_csi != NULL) {
		this->m_csi->bootstrapped((void *) this, (void *) security);
	}
}

// object registered
void Endpoint::object_registered(void *security, void *server) {
	this->logger()->log("Connector::Endpoint: endpoint registered.");
	this->m_connected = true;
	this->m_registered = true;
	if (this->m_csi != NULL) {
		this->m_csi->object_registered((void *) this, security, server);
	}
}

// registration updated
void Endpoint::registration_updated(void *security, void *server) {
	this->logger()->log("Connector::Endpoint: endpoint re-registered.");
	this->m_connected = true;
	this->m_registered = true;
	if (this->m_csi != NULL) {
		this->m_csi->registration_updated((void *) this, security, server);
	}
}

// resource value updated
void Endpoint::value_updated(M2MBase *base, M2MBase::BaseType type) {
	// Lookup the resource and invoke process() on it...
	DynamicResource *target_res = this->lookupDynamicResource(base);
	if (target_res != NULL) {
		// DEBUG
		//this->logger()->log("Value Updated (Custom Resource)");

		// its a custom resource...
		target_res->process(base->operation(), type);
	}

	// CSI
	if (this->m_csi != NULL) {
		this->m_csi->value_updated((void *) this, (void *) base, (int) type);
	}
}

// lookup which DynamicResource cooresponds to a given M2MBase instance...
DynamicResource *Endpoint::lookupDynamicResource(M2MBase *base) {
	const DynamicResourcesList *dynamic_resources =
			this->m_options->getDynamicResourceList();
	for (int i = 0; i < (int) dynamic_resources->size(); ++i) {
		M2MBase *t = (M2MBase *) dynamic_resources->at(i)->getResource();
		if (t == base) {
			return dynamic_resources->at(i);
		}
	}
	return NULL;
}

// build out the endpoint
void Endpoint::buildEndpoint() {
	// initialize as an mbed-client
	this->createEndpointInterface();

	// make sure we have an endpoint interface...
	if (this->getEndpointInterface() != NULL) {
		// We now have to bind our device resources
		if (this->m_device_manager != NULL) {
			// DEBUG
			this->logger()->log("Connector::Endpoint::build(): plumbing the device management objects and resources...");

			// bind the device manager
			((DeviceManager *) this->m_device_manager)->bind();
		}
		else {
			// no device manager installed
			this->logger()->log("Connector::Endpoint::build(): No device manager installed.");
		}
		
		// Loop through Static Resources and bind each of them...
		this->logger()->log("Connector::Endpoint::build(): adding static resources...");
		const StaticResourcesList *static_resources =
				this->m_options->getStaticResourceList();
		for (int i = 0; i < (int) static_resources->size(); ++i) {
			this->logger()->log("Connector::Endpoint::build(): binding static resource: [%s]...",static_resources->at(i)->getFullName().c_str());
			static_resources->at(i)->bind(this);
		}

		// Loop through Dynamic Resources and bind each of them...
		this->logger()->log("Connector::Endpoint::build(): adding dynamic resources...");
		const DynamicResourcesList *dynamic_resources =
				this->m_options->getDynamicResourceList();
		for (int i = 0; i < (int) dynamic_resources->size(); ++i) {
			this->logger()->log("Connector::Endpoint::build(): binding dynamic resource: [%s]...",dynamic_resources->at(i)->getFullName().c_str());
			dynamic_resources->at(i)->bind(this);
		}

		// Get the ObjectList from the ObjectInstanceManager...
		NamedPointerList list =
				this->getObjectInstanceManager()->getObjectList();

		// DEBUG
		//this->logger()->log("Endpoint::build(): All Resources bound. Number of Objects in list: %d",list.size());

		// add all of the object instances we have created...
		for (int i = 0; i < (int) list.size(); ++i) {
			// DEBUG
			//this->logger()->log("Endpoint::build(): adding Object Instance with ObjID: %s...",list.at(i).name().c_str());

			// push back the object instance...
			this->m_endpoint_object_list.push_back(
					(M2MObject *) (list.at(i).ptr()));
		}
	} else {
		// no endpoint interface created
		this->logger()->log("Endpoint::build(): ERROR in creating the endpoint interface...");
	}
}

// stop underlying observation mechanisms
void Endpoint::stopObservations() {
	const DynamicResourcesList *dynamic_resources =
			this->m_options->getDynamicResourceList();
	for (int i = 0; i < (int) dynamic_resources->size(); ++i) {
		if (dynamic_resources->at(i)->isObservable() == true) {
			ResourceObserver *observer =
					(ResourceObserver *) dynamic_resources->at(i)->getObserver();
			if (observer != NULL) {
				this->logger()->log("Connector::Endpoint::stopObservations(): stopping resource observer for: [%s]...",dynamic_resources->at(i)->getFullName().c_str());
				observer->halt();
			}
		}
	}
}

// underlying network is connected (SET)
void Endpoint::isConnected(bool connected) {
	this->m_connected = connected;
}

// underlying network is connected (GET)
bool Endpoint::isConnected() {
	return this->m_connected;
}

// Registered with mDC/mDS
bool Endpoint::isRegistered() {
	return this->m_registered;
}

// Set the ConnectionStatusInterface
void Endpoint::setConnectionStatusInterfaceImpl(
		ConnectionStatusInterface *csi) {
	this->m_csi = csi;
}

// Set our ObjectInstanceManager
void Endpoint::setObjectInstanceManager(ObjectInstanceManager *oim) {
	this->m_oim = oim;
}

// Get our ObjectInstanceManager
ObjectInstanceManager *Endpoint::getObjectInstanceManager() {
	return this->m_oim;
}

// our logger
Logger *Endpoint::logger() {
	return this->m_logger;
}

} // namespace Connector
