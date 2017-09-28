/**
 * @file    ConnectorEndpoint.cpp
 * @brief   mbed CoAP Endpoint base class
 * @author  Doug Anson/Chris Paola
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

// Lower level Network
#include "mbed-connector-interface/mbedEndpointNetwork.h"

// ConnectorEndpoint
#include "mbed-connector-interface/ConnectorEndpoint.h"

// Utils support
#include "mbed-connector-interface/Utils.h"

// Device Manager support
#include "mbed-connector-interface/DeviceManager.h"

// factory storage and configurator support (mbed Cloud R1.2+)
#ifdef ENABLE_MBED_CLOUD_SUPPORT
// trace configuration
#include "mbed-trace/mbed_trace.h"

// updater support
#include "update_ui_example.h"

// factory flow support
#include "factory_configurator_client.h"
#endif

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
		printf(
				"Connector::Endpoint::plumbNetwork: initializing endpoint instance...\r\n");
		__endpoint = (Connector::Endpoint *) utils_init_endpoint(
				canActAsRouterNode);
	}

	// make sure we have an endpoint...
	if (__endpoint != NULL) {
		// set the device manager
		if (device_manager != NULL) {
			// device manager has been supplied
			printf(
					"Connector::Endpoint::plumbNetwork: setting a device manager...\r\n");
			__endpoint->setDeviceManager(device_manager);
		}

		// configure the endpoint...
		printf(
				"Connector::Endpoint::plumbNetwork: configuring endpoint...\r\n");
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
				Thread::wait(1000);
			}
		}
	} else {
		// not starting the endpoint due to errors
		printf(
				"Connector::Endpoint::start: Not starting endpoint due to errors (no endpoint)... exiting...\r\n");

		// end in error... 
		while (true) {
			Thread::wait(1000);
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
#ifdef ENABLE_MBED_CLOUD_SUPPORT
Endpoint::Endpoint(const Logger *logger, const Options *options) : MbedCloudClientCallback(), M2MInterfaceObserver()
#else
Endpoint::Endpoint(const Logger *logger, const Options *options) :
		M2MInterfaceObserver()
#endif
{
	this->m_logger = (Logger *) logger;
	this->m_options = (Options *) options;
	this->m_device_manager = NULL;
	this->m_connected = false;
	this->m_registered = false;
	this->m_csi = NULL;
	this->m_oim = NULL;
	this->m_endpoint_security = NULL;
	this->m_endpoint_interface = NULL;
}

// Copy Constructor
Endpoint::Endpoint(const Endpoint &ep) {
	this->m_logger = ep.m_logger;
	this->m_options = ep.m_options;
	this->m_endpoint_interface = ep.m_endpoint_interface;
	this->m_endpoint_security = ep.m_endpoint_security;
	this->m_endpoint_object_list = ep.m_endpoint_object_list;
	this->m_device_manager = ep.m_device_manager;
	this->m_connected = ep.m_connected;
	this->m_registered = ep.m_registered;
	this->m_csi = ep.m_csi;
	this->m_oim = ep.m_oim;
}

// Destructor
Endpoint::~Endpoint() {
#ifndef ENABLE_MBED_CLOUD_SUPPORT
	if (this->m_endpoint_interface != NULL)
		delete this->m_endpoint_interface;

	if (this->m_endpoint_security != NULL)
		delete this->m_endpoint_security;
#endif
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

// get our endpoint security instance
M2MSecurity *Endpoint::getSecurityInstance() {
	return this->m_endpoint_security;
}

// set our endpoint security instance
void Endpoint::setSecurityInstance(M2MSecurity *security) {
	if (security != NULL) {
		this->m_endpoint_security = security;
	}
}

// get our ObjectList
M2MObjectList Endpoint::getEndpointObjectList() {
	return this->m_endpoint_object_list;
}

#ifdef ENABLE_MBED_CLOUD_SUPPORT
// get our endpoint interface
MbedCloudClient *Endpoint::getEndpointInterface() {
	return this->m_endpoint_interface;
}
#else
// get our endpoint interface
M2MInterface *Endpoint::getEndpointInterface() {
	return this->m_endpoint_interface;
}
#endif

// Connector::Endpoint: create our interface
void Endpoint::createEndpointInterface() {
#ifdef ENABLE_MBED_CLOUD_SUPPORT
	this->createCloudEndpointInterface();
#else
	this->createConnectorEndpointInterface();
#endif
}

#ifdef ENABLE_MBED_CLOUD_SUPPORT
// mbedCloudClient: initialize Storage
bool Endpoint::initializeStorage() {
#ifdef MBED_CLOUD_STORAGE_INIT
	// initialize mbed-trace
	mbed_trace_init();

	// initialize FCC
	fcc_status_e status = fcc_init();
	if(status != FCC_STATUS_SUCCESS) {
		this->logger()->log("initializeStorage: ERROR: mfcc_init failed with status=%d...", status);
		return false;
	}

#ifdef MBED_RESET_STORAGE
	// Resets storage to an empty state.
	// Use this function when you want to clear SD card from all the factory-tool generated data and user data.
	// After this operation device must be injected again by using factory tool or developer certificate.
	this->logger()->log("initializeStorage: Resetting storage to an empty state...");
	fcc_status_e delete_status = fcc_storage_delete();
	if (delete_status != FCC_STATUS_SUCCESS) {
		this->logger()->log("initializeStorage: Failed to reset storage to an empty state. status=%d (OK)...", delete_status);
	}
#endif
#else
	// not enabled
	this->logger()->log("initializeStorage: storage initialize disabled (OK)...");
#endif
	return true;
}

// mbedCloudClient:: initialize factory flow
bool Endpoint::initializeFactoryFlow() {
#ifdef MBED_CLOUD_DEV_FLOW_INIT
#ifdef MBED_CONF_APP_DEVELOPER_MODE
	this->logger()->log("initializeFactoryFlow: Start developer flow...");
	fcc_status_e status = fcc_developer_flow();
	if (status == FCC_STATUS_KCM_FILE_EXIST_ERROR) {
		this->logger()->log("initializeFactoryFlow: Developer credentials already exists (OK)...");
	} else if (status != FCC_STATUS_SUCCESS) {
		this->logger()->log("initializeFactoryFlow: ERROR: Failed to load developer credentials");
		return false;
	}
	status = fcc_verify_device_configured_4mbed_cloud();
	if (status != FCC_STATUS_SUCCESS) {
		this->logger()->log("initializeFactoryFlow: ERROR: Device not configured for mbed Cloud");
		return false;
	}
	return true;
#else
	this->logger()->log("initializeFactoryFlow: non-developer factory flow chosen... continuing...");
	return true;
#endif
#else
	this->logger()->log("initializeFactoryFlow: developer flow init disabled (OK)...");
	return true;
#endif
}

// mbedCloudClient: create our interface
void Endpoint::createCloudEndpointInterface() {
	if (this->m_endpoint_interface == NULL) {
		bool storage_init = this->initializeStorage();
		bool factory_flow_init = this->initializeFactoryFlow();
		if (storage_init && factory_flow_init) {
			// create a new instance of mbedCloudClient
			this->logger()->log("createCloudEndpointInterface: creating mbed cloud client instance...");
			this->m_endpoint_interface = new MbedCloudClient();
			if (this->m_endpoint_interface == NULL) {
				// unable to allocate the MbedCloudClient instance
				this->logger()->log("createCloudEndpointInterface: ERROR: unable to allocate MbedCloudClient instance...");
			}
			else {
				// enable hooks for Updater support (R1.2+) (if enabled)
#ifdef MBED_CLOUD_CLIENT_SUPPORT_UPDATE		
				// Establish the updater hook	
				update_ui_set_cloud_client(this->m_endpoint_interface);

				// Update Authorize Handler (optional, disabled by default)
#ifdef ENABLE_UPDATE_AUTHORIZE_HANDLER
				this->m_endpoint_interface->set_update_authorize_handler(&Connector::Endpoint::update_authorize);
#endif

				// Update Progress Handler (optional, disabled by default)
#ifdef ENABLE_UPDATE_PROGRESS_HANDLER
				this->m_endpoint_interface->set_update_progress_handler(&Connector::Endpoint::update_progress);
#endif

#endif
			}
		}
		else {
			if (storage_init) {
				// unable to create mbed cloud client instance... (FAILED factory flow init)
				this->logger()->log("createCloudEndpointInterface: ERROR: unable to initialize factory flow...");
			}
			else {
				// unable to create mbed cloud client instance... (FAILED storage init)
				this->logger()->log("createCloudEndpointInterface: ERROR: unable to initialize storage...");
			}
			this->m_endpoint_interface = NULL;
		}
	}

	// bind LWIP network interface pointer...                                          
	if (__network_interface != NULL && this->m_endpoint_interface != NULL) {
		this->logger()->log("Connector::Endpoint: binding LWIP network instance (Cloud)...");
		this->m_endpoint_interface->on_registered(&Connector::Endpoint::on_registered);
		this->m_endpoint_interface->on_unregistered(&Connector::Endpoint::on_unregistered);
		this->m_endpoint_interface->on_error(&Connector::Endpoint::on_error);
		this->m_endpoint_interface->set_update_callback(this);
	}
	else {
		// skipping LWIP bind...
		this->logger()->log("Connector::Endpoint: ERROR (Cloud) skipping LWIP network instance bind due to previous error...");
	}
}
#else
// mbed-client: create our interface
void Endpoint::createConnectorEndpointInterface() {
	// get the CoAP listening port
	uint16_t listening_port = (uint16_t) this->m_options->getConnectorPort();

	// randomize the port if we are using certificates...
	if (this->m_options->getServerCertificateSize() > 0) {
		// Randomizing listening port for Certificate mode connectivity
		srand (time(NULL));listening_port = rand() % 65535 + 12345;
	}

	// DEBUG
	//this->logger()->log("Connector::Endpoint: listening port: %d",listening_port);

	// Socket protocol type: TCP or UDP
	M2MInterface::BindingMode socket_protocol_type = M2MInterface::UDP;
	if (this->m_options->getCoAPConnectionType() == COAP_TCP)
		socket_protocol_type = M2MInterface::TCP;

	// Socket address type: IPv4 or IPv6
	M2MInterface::NetworkStack socket_address_type = M2MInterface::LwIP_IPv4;
	if (this->m_options->getIPAddressType() == IP_ADDRESS_TYPE_IPV6) {
		// IPv6 mode for the socket addressing type... 
		socket_address_type = M2MInterface::LwIP_IPv6;

#if defined (IPV4_OVERRIDE)
		// OVERRIDE (until patched...)
		this->logger()->log("Connector::Endpoint: Socket Address Type: IPv4 (IPv6 OVERRIDE)");
		socket_address_type = M2MInterface::LwIP_IPv4;
#endif
	}

	// DEBUG
	if (socket_protocol_type == M2MInterface::TCP)
		this->logger()->log("Connector::Endpoint: Socket Protocol: TCP");
	if (socket_protocol_type == M2MInterface::UDP)
		this->logger()->log("Connector::Endpoint: Socket Protocol: UDP");
	if (socket_address_type == M2MInterface::LwIP_IPv4)
		this->logger()->log("Connector::Endpoint: Socket Address Type: IPv4");
	if (socket_address_type == M2MInterface::LwIP_IPv6)
		this->logger()->log("Connector::Endpoint: Socket Address Type: IPv6");

	// Create the endpoint M2MInterface instance
	this->m_endpoint_interface = M2MInterfaceFactory::create_interface(*this,
			(char *) this->m_options->getEndpointNodename().c_str(),// endpoint name
			(char *) this->m_options->getEndpointType().c_str(),// endpoint type
			(int32_t) this->m_options->getLifetime(),// registration lifetime (in seconds)
			listening_port,						// listening port (ephemeral...)
			(char *) this->m_options->getDomain().c_str(),	// endpoint domain
			socket_protocol_type,		// Socket protocol type: UDP or TCP...
			socket_address_type,		// Socket addressing type: IPv4 or IPv6
			CONTEXT_ADDRESS_STRING// context address string (mbedConnectorInterface.h)
			);

	// bind LWIP network interface pointer...
	if (__network_interface != NULL && this->m_endpoint_interface != NULL) {
		this->logger()->log("Connector::Endpoint: binding LWIP network instance (Connector)...");
		this->m_endpoint_interface->set_platform_network_handler(
				(void *) __network_interface);
	}
}
#endif

// mbed-client: createEndpointSecurityInstance()
M2MSecurity *Endpoint::createEndpointSecurityInstance() {
#ifdef ENABLE_MBED_CLOUD_SUPPORT
	// internalized... not used.
	return NULL;
#else
	// Creates register server object with mbed device server address and other parameters
	M2MSecurity *server = M2MInterfaceFactory::create_security(
			M2MSecurity::M2MServer);
	if (server != NULL) {
		const String url = this->m_options->getConnectorURL();
		server->set_resource_value(M2MSecurity::M2MServerUri, url);
		server->set_resource_value(M2MSecurity::BootstrapServer, false);
		server->set_resource_value(M2MSecurity::SecurityMode,
				M2MSecurity::Certificate);
		server->set_resource_value(M2MSecurity::ServerPublicKey,
				this->m_options->getServerCertificate(),
				this->m_options->getServerCertificateSize());
		server->set_resource_value(M2MSecurity::PublicKey,
				this->m_options->getClientCertificate(),
				this->m_options->getClientCertificateSize());
		server->set_resource_value(M2MSecurity::Secretkey,
				this->m_options->getClientKey(),
				this->m_options->getClientKeySize());
	}
	return server;
#endif
}

#ifdef ENABLE_MBED_CLOUD_SUPPORT
// mbed-cloud-client: Callback from mbed client stack if any error is encountered
void Endpoint::on_error(int error_code) {
	char *error = (char *)"No Error";
	switch(error_code) {
		case MbedCloudClient::ConnectErrorNone:
		error = (char *)"MbedCloudClient::ConnectErrorNone";
		break;
		case MbedCloudClient::ConnectAlreadyExists:
		error = (char *)"MbedCloudClient::ConnectAlreadyExists";
		break;
		case MbedCloudClient::ConnectBootstrapFailed:
		error = (char *)"MbedCloudClient::ConnectBootstrapFailed";
		break;
		case MbedCloudClient::ConnectInvalidParameters:
		error = (char *)"MbedCloudClient::ConnectInvalidParameters";
		break;
		case MbedCloudClient::ConnectNotRegistered:
		error = (char *)"MbedCloudClient::ConnectNotRegistered";
		break;
		case MbedCloudClient::ConnectTimeout:
		error = (char *)"MbedCloudClient::ConnectTimeout";
		break;
		case MbedCloudClient::ConnectNetworkError:
		error = (char *)"MbedCloudClient::ConnectNetworkError";
		break;
		case MbedCloudClient::ConnectResponseParseFailed:
		error = (char *)"MbedCloudClient::ConnectResponseParseFailed";
		break;
		case MbedCloudClient::ConnectUnknownError:
		error = (char *)"MbedCloudClient::ConnectUnknownError";
		break;
		case MbedCloudClient::ConnectMemoryConnectFail:
		error = (char *)"MbedCloudClient::ConnectMemoryConnectFail";
		break;
		case MbedCloudClient::ConnectNotAllowed:
		error = (char *)"MbedCloudClient::ConnectNotAllowed";
		break;
		case MbedCloudClient::ConnectSecureConnectionFailed:
		error = (char *)"MbedCloudClient::ConnectSecureConnectionFailed";
		break;
		case MbedCloudClient::ConnectDnsResolvingFailed:
		error = (char *)"MbedCloudClient::ConnectDnsResolvingFailed";
		break;
#ifdef MBED_CLOUD_CLIENT_SUPPORT_UPDATE
		case UpdateClient::WarningCertificateNotFound:
		error = (char *)"MbedCloudClient(Update): WarningCertificateNotFound";
		break;
		case UpdateClient::WarningIdentityNotFound:
		error = (char *)"MbedCloudClient(Update): WarningIdentityNotFound";
		break;
		case UpdateClient::WarningCertificateInvalid:
		error = (char *)"MbedCloudClient(Update): WarningCertificateInvalid";
		break;
		case UpdateClient::WarningSignatureInvalid:
		error = (char *)"MbedCloudClient(Update): WarningSignatureInvalid";
		break;
		case UpdateClient::WarningVendorMismatch:
		error = (char *)"MbedCloudClient(Update): WarningVendorMismatch";
		break;
		case UpdateClient::WarningClassMismatch:
		error = (char *)"MbedCloudClient(Update): WarningClassMismatch";
		break;
		case UpdateClient::WarningDeviceMismatch:
		error = (char *)"MbedCloudClient(Update): WarningDeviceMismatch";
		break;
		case UpdateClient::WarningURINotFound:
		error = (char *)"MbedCloudClient(Update): WarningURINotFound";
		break;
		case UpdateClient::WarningRollbackProtection:
		error = (char *)"MbedCloudClient(Update): WarningRollbackProtection. Manifest is older than currently running image.";
		break;
		case UpdateClient::WarningUnknown:
		error = (char *)"MbedCloudClient(Update): WarningUnknown";
		break;
		case UpdateClient::ErrorWriteToStorage:
		error = (char *)"MbedCloudClient(Update): ErrorWriteToStorage";
		break;
#endif		
		default:
		error = (char *)"UNKNOWN";
	}
	printf("Connector::Endpoint(Cloud) Error(%x): %s\r\n",error_code,error);
}

// mbed-cloud-client: update_authorized
void Endpoint::update_authorize(int32_t request) {
	// simple debug for now... this will NOT authorize the update request... 
	printf("Connector::Endpoint(Cloud) Update Authorize: request: %d\n",(int)request);
}

// mbed-cloud-client: update_progress
void Endpoint::update_progress(uint32_t progress, uint32_t total) {
	// simple debug for now...
	printf("Connector::Endpoint(Cloud) Update Progress: (%d/%d)\n",(int)progress,(int)total);
}
#endif

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
#ifdef ENABLE_MBED_CLOUD_SUPPORT
		// DEBUG
		this->logger()->log("Connector::Endpoint(Cloud): re-register endpoint...");
#else
		this->m_endpoint_interface->update_registration(
				this->m_endpoint_security, this->m_options->getLifetime());
#endif
	}
}

// de-register endpoint 
void Endpoint::de_register_endpoint(void) {
	if (this->m_endpoint_interface != NULL) {
#ifdef ENABLE_MBED_CLOUD_SUPPORT
		// DEBUG
		this->logger()->log("Connector::Endpoint(Cloud): de-registering endpoint...");
		this->m_endpoint_interface->close();
#else
		// de-register endpoint
		this->logger()->log("Connector::Endpoint: de-registering endpoint...");
		if (this->m_csi != NULL) {
			this->m_csi->begin_object_unregistering((void *) this);
		} else {
			this->m_endpoint_interface->unregister_object(NULL);
		}
#endif		
	}
}

// register the endpoint
void Endpoint::register_endpoint(M2MSecurity *endpoint_security,
		M2MObjectList endpoint_objects) {
#ifdef ENABLE_MBED_CLOUD_SUPPORT
	if (this->m_endpoint_interface != NULL) {
		this->logger()->log("Connector::Endpoint(Cloud): adding objects to endpoint...");
		this->m_endpoint_interface->add_objects(endpoint_objects);

		this->logger()->log("Connector::Endpoint(Cloud): registering endpoint...");
		this->m_endpoint_interface->setup(__network_interface);
	}
#else
	if (this->m_endpoint_interface != NULL && endpoint_security != NULL
			&& endpoint_objects.size() > 0) {
		// register  endpoint
		this->logger()->log("Connector::Endpoint: registering endpoint...");
		this->m_endpoint_interface->register_object(endpoint_security,
				endpoint_objects);
	}
#endif
}

#ifdef ENABLE_MBED_CLOUD_SUPPORT
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
		__endpoint->object_unregistered(__endpoint->getSecurityInstance());
	}
}
#endif

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

#ifndef ENABLE_MBED_CLOUD_SUPPORT
	else {
		// DEBUG
		//this->logger()->log("Value Updated (Device Manager)");

		// let DeviceManager handle it
		((DeviceManager *) this->m_device_manager)->process(base, type);
	}
#endif

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
		// Create our server instance
		this->setSecurityInstance(this->createEndpointSecurityInstance());

		// We now have to bind our device resources
		if (this->m_device_manager != NULL) {
			// DEBUG
			this->logger()->log("Connector::Endpoint::build(): plumbing the device management objects and resources...");

			// bind the device manager
			((DeviceManager *) this->m_device_manager)->bind();

			// push back the Device Resources Object
			if (this->m_options->getDeviceResourcesObject() != NULL) {
				// DEBUG
				this->logger()->log("Connector::Endpoint::build(): plumbing device resources object...");

				// push back the device resources object
				this->m_endpoint_object_list.push_back(
						(M2MObject *) this->m_options->getDeviceResourcesObject());
			} else {
				// unable to plumb device manager
				this->logger()->log("Connector::Endpoint::build(): Unable to plumb device resources. Not installing device resource object...");
			}

			// push back the Firmware Resources Object
			if (this->m_options->getFirmwareResourcesObject() != NULL) {
				// DEBUG
				this->logger()->log("Connector::Endpoint::build(): plumbing firmware resources object...");

				// push back the firmware resources object
				this->m_endpoint_object_list.push_back(
						(M2MObject *) this->m_options->getFirmwareResourcesObject());
			} else {
				// unable to plumb firmware manager
				this->logger()->log("Connector::Endpoint::build(): Unable to plumb firmware resources. Not installing firmware resource object...");
			}
		} else {
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
