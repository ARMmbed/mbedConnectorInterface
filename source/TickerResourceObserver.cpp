/**
 * @file    TickerResourceObserver.cpp
 * @brief   mbed CoAP DynamicResource Ticker-based observer (implementation)
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
 #include "mbed-connector-interface/TickerResourceObserver.h"
 
 #ifdef CONNECTOR_USING_TICKER
 
 // our instance
 static void *_instance = NULL;
  
 // constructor
 TickerResourceObserver::TickerResourceObserver(DynamicResource *resource,int sleep_time) : ResourceObserver(resource,(int)(sleep_time/1000)) {
     this->setObserving(false);
     
     // DEBUG
     this->logger()->log("TickerResourceObserver being used for %s (sleep_time: %d ms)",resource->getFullName().c_str(),sleep_time);
     
     // our instance
     _instance = (void *)this;
 }
  
 // destructor
 TickerResourceObserver::~TickerResourceObserver() {
     this->stopObservation();
 }

 // observation task method
 void TickerResourceObserver::observation_task() {
     TickerResourceObserver *me = (TickerResourceObserver *)_instance;
     if (me != NULL && me->isObserving() == true && me->getResource() != NULL) {
         DynamicResource *res = me->getResource();
         if (res != NULL && res->isRegistered() == true) {
             res->observe();
         }
     }
 }
 
 // begin observing...
 void TickerResourceObserver::beginObservation() {
     if (this->isObserving() == false) {
        this->m_ticker.attach(this,&TickerResourceObserver::observation_task,(float)this->getSleepTime());
        this->setObserving(true);
     }
 }
 
 // begin observing...
 void TickerResourceObserver::stopObservation() {
     this->setObserving(false);
 }

 // halt the underlying observer mechanism
 void TickerResourceObserver::halt() {
     this->m_ticker.detach();
 }
 
 #endif // CONNECTOR_USING_TICKER