/**
 * @file    mbedEndpointNetworkImpl.h
 * @brief   mbed Endpoint Network header (impl) - forward references to public functions of mbedEndpointNetwork
 * @author  Doug Anson
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

#ifndef __MBED_ENDPOINT_NETWORK_IMPL_H__
#define __MBED_ENDPOINT_NETWORK_IMPL_H__

// Forward references to public functions of mbedEndpointNetwork...
extern "C" char *net_get_type(void);
extern "C" void  net_shutdown_endpoint(void);
extern "C" void  net_plumb_network(void *p);                                       
extern "C" void  net_finalize_and_run_endpoint_main_loop(void *p); 

#endif // __MBED_ENDPOINT_NETWORK_IMPL_H__