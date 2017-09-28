/**
 * @file    EventQueueResourceObserver.cpp
 * @brief   mbed CoAP DynamicResource Thread-based observer (implementation)
 * @author  Doug Anson/Chris Paola
 * @version 1.0
 * @see
 *
 * Copyright (c) 2017
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
 #include "mbed-connector-interface/EventQueueResourceObserver.h"
 
 #ifdef CONNECTOR_USING_EVENT_QUEUES
 
 // our instance
 static void *_instance = NULL;
 
 // constructor
 EventQueueResourceObserver::EventQueueResourceObserver(DynamicResource *resource,int sleep_time) : ResourceObserver(resource,sleep_time), m_event_queue() {
        // default is not observing...
        this->setObserving(false);
        
        // DEBUG
        this->logger()->log("EventQueueResourceObserver being used for %s (sleep_time: %d ms)",resource->getFullName().c_str(),sleep_time);
        
        // our instance
        _instance = (void *)this;
        
        // start the thread by invoking the thread task...
        this->m_id = this->m_event_queue.call_every(sleep_time,EventQueueResourceObserver::observation_task);
 }
 
 // destructor
 EventQueueResourceObserver::~EventQueueResourceObserver() {
     this->stopObservation();
     this->m_event_queue.cancel(this->m_id);
 }
 
 // observation task method
 void EventQueueResourceObserver::observation_task() {
     EventQueueResourceObserver *me = (EventQueueResourceObserver *)_instance;
     if (me != NULL && me->isObserving() == true && me->getResource() != NULL) {
         DynamicResource *res = me->getResource();
         if (res != NULL && res->isRegistered() == true) {
             res->observe();
         }
     }
 }

 // begin observing...
 void EventQueueResourceObserver::beginObservation() {
     this->setObserving(true);
 }
 
 // stop observing...
 void EventQueueResourceObserver::stopObservation() {
     this->setObserving(false);
 }
 
 // halt the underlying observer mechanism
 void EventQueueResourceObserver::halt() {
     this->m_event_queue.cancel(this->m_id);
 }
 
 #endif // CONNECTOR_USING_EVENT_QUEUES