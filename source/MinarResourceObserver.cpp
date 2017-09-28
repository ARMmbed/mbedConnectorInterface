/**
 * @file    MinarResourceObserver.cpp
 * @brief   mbed CoAP DynamicResource Minar-based observer (implementation)
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

// class support
#include "mbed-connector-interface/MinarResourceObserver.h"

#if defined (MCI_MINAR_SCHEDULER)

 // constructor
 MinarResourceObserver::MinarResourceObserver(DynamicResource *resource,int sleep_time) : ResourceObserver(resource,sleep_time) {
     this->setObserving(false);
        
     // DEBUG
     this->logger()->log("MinarResourceObserver being used for %s (sleep_time: %d ms)",resource->getFullName().c_str(),sleep_time);
 }

 // destructor
 MinarResourceObserver::~MinarResourceObserver() {
     this->stopObservation();
 }

 // observation task method
 void MinarResourceObserver::observation_task() {
     if (this->isObserving() == true && this->getResource() != NULL) {
         DynamicResource *res = this->getResource();
         if (res != NULL && res->isRegistered() == true) {
             res->observe();
         }
     }
 }

 // begin observing...
 void MinarResourceObserver::beginObservation() {
     this->setObserving(true);
     minar::Scheduler::postCallback(this,&MinarResourceObserver::observation_task).period(minar::milliseconds(this->getSleepTime()));
 }

 // stop observing...
 void MinarResourceObserver::stopObservation() {
     this->setObserving(false);
 }
 
 // halt the underlying observer mechanism
 void MinarResourceObserver::halt() {
     minar::Scheduler::stop();
 }
 
 #endif // MCI_MINAR_SCHEDULER
