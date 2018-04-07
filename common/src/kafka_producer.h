/*
    Realm of Aesir
    Copyright (C) 2016  Michael de Lang

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "messages/message.h"

#include <string>
#include <unordered_map>
#include <memory>
#include <rdkafkacpp.h>
#include <mutex>

namespace roa {
    class producer_event_callback : public RdKafka::EventCb {
    public:
        void event_cb (RdKafka::Event &event);
    };

    /* Use of this partitioner is pretty pointless since no key is provided
     * in the produce() call. */
    class producer_hash_partitioner_callback : public RdKafka::PartitionerCb {
    public:
        int32_t partitioner_cb (const RdKafka::Topic *topic, const std::string *key, int32_t partition_cnt, void *msg_opaque);

    private:
        static inline unsigned int djb_hash (const char *str, size_t len);
    };

    class producer_delivery_callback : public RdKafka::DeliveryReportCb {
    public:
        void dr_cb (RdKafka::Message &message);
    };

    template <bool UseJson>
    class ikafka_producer {
    public:
        virtual ~ikafka_producer() = default;

        /**
         * Start the producer with the given parameters
         * @param broker_list comma-separated list of FQDN hosts that run kafka
         * @param debug give more stdout info
         */
        virtual void start(std::string broker_list, uint32_t queue_buffering_max_ms = 100, bool disable_nagle = true, bool debug = false) = 0;

        /**
         * close the producer, immediately stop try_get_message from working and wait a couple of seconds to process the queue
         * before cleaning up.
         */
        virtual void close() = 0;

        /**
         * Enqueue a message on a given topic. If topic has not been used before, creates said topic.
         * @param topic_str
         * @param msg
         * @threadsafe
         */
        virtual void enqueue_message(std::string topic_str, message<UseJson> const &msg) = 0;

        /**
         * Enqueue a message on a given topic. If topic has not been used before, creates said topic.
         * @param topic_str
         * @param msg
         * @threadsafe
         */
        virtual void enqueue_message(std::string topic_str, message<UseJson> const * const msg) = 0;

        /**
         * Check if kafka queue is empty
         * @return true if empty, false if not
         */
        virtual bool is_queue_empty() = 0;

        /**
         * Poll the underlying kafka producer for messages or events. Has to be called at least a couple times per second
         * @param ms_to_wait 0 for non-blocking, -1 for indefinite, >0 for blocking a maximum amount of ms
         * @return number of events served
         */
        virtual int poll(uint32_t ms_to_wait) = 0;
    };

    template <bool UseJson>
    class kafka_producer : public ikafka_producer<UseJson> {
    public:
        kafka_producer();

        ~kafka_producer();

        void start(std::string broker_list, uint32_t queue_buffering_max_ms = 100, bool disable_nagle = true, bool debug = false) override;
        void close() override;

        void enqueue_message(std::string topic_str, message<UseJson> const &msg) override;
        void enqueue_message(std::string topic_str, message<UseJson> const * const msg) override;
        bool is_queue_empty() override;
        int poll(uint32_t ms_to_wait) override;
    private:
        bool _closing;
        std::unique_ptr<RdKafka::Producer> _producer;
        std::unordered_map<std::string, std::unique_ptr<RdKafka::Topic>> _topics;
        std::mutex _topics_mutex;
        producer_hash_partitioner_callback _hash_partitioner_callback;
        producer_delivery_callback _delivery_callback;
        producer_event_callback _event_callback;
    };
}