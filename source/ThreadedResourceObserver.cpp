/**
 * @file    ThreadedResourceObserver.cpp
 * @brief   mbed CoAP DynamicResource Thread-based observer (implementation)
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
 
 // Class support
 #include "mbed-connector-interface/ThreadedResourceObserver.h"
 
 #ifdef CONNECTOR_USING_THREADS
 
 // constructor
 ThreadedResourceObserver::ThreadedResourceObserver(DynamicResource *resource,int sleep_time) : ResourceObserver(resource,sleep_time), m_observation_thread() {
        // default is not observing...
        this->setObserving(false);
        
        // DEBUG
        this->logger()->log("ThreadedResourceObserver being used for %s (sleep_time: %d ms)",resource->getFullName().c_str(),sleep_time);
        
        // start the thread by invoking the thread task...
        this->m_observation_thread.start(callback(this,&ThreadedResourceObserver::observation_task));
 }
 
 // destructor
 ThreadedResourceObserver::~ThreadedResourceObserver() {
     this->stopObservation();
     this->m_observation_thread.terminate();
 }
 
 // observation task method
 void ThreadedResourceObserver::observation_task() {
     while(true) {
         Thread::wait(this->getSleepTime());
         if (this->isObserving() == true && this->getResource() != NULL) {
             DynamicResource *res = this->getResource();
             if (res != NULL && res->isRegistered() == true) {
                 res->observe();
             }
         }
     }
 }

 // begin observing...
 void ThreadedResourceObserver::beginObservation() {
     this->setObserving(true);
 }
 
 // stop observing...
 void ThreadedResourceObserver::stopObservation() {
     this->setObserving(false);
 }
 
 // halt the underlying observer mechanism
 void ThreadedResourceObserver::halt() {
     this->m_observation_thread.terminate();
 }
 
 #endif // CONNECTOR_USING_THREADS