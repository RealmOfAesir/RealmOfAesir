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
#include <vector>
#include <memory>
#include <tuple>
#include <rdkafkacpp.h>

namespace roa {
    template <bool UseJson>
    class ikafka_consumer {
    public:
        virtual ~ikafka_consumer() = default;

        /**
         * Start the consumer with the given parameters and start listening
         * @param broker_list comma-separated list of FQDN
         * @param group_id kafka group this consumer belongs to
         * @param topics list of topics to listen on
         * @param debug give more stdout info
         */
        virtual void start(std::string broker_list, std::string group_id, std::vector<std::string> topics, uint32_t fetch_wait_max_ms = 100, bool debug = false) = 0;

        /**
         * close the consumer, immediately stop try_get_message from working and wait a couple of seconds to process the queue
         * before cleaning up.
         */
        virtual void close() = 0;

        /**
         * Wait for a message from one of the kafka topics and deserialize it
         * @param ms_to_wait -1 for indefinite, 0 for non-blocking, >0 for milliseconds to wait for a message
         * @return tuple with message id and pointer to deserialized string if message found, otherwise empty tuple.
         */
        virtual std::tuple<uint32_t, std::unique_ptr<message<UseJson> const>> try_get_message(uint16_t ms_to_wait = 0) = 0;

        /**
         * Check if kafka queue is empty
         * @return true if empty, false if not
         */
        virtual bool is_queue_empty() = 0;
    };

    template <bool UseJson>
    class kafka_consumer : public ikafka_consumer<UseJson> {
    public:
        kafka_consumer();

        ~kafka_consumer();

        void start(std::string broker_list, std::string group_id, std::vector<std::string> topics, uint32_t fetch_wait_max_ms = 100, bool debug = false) override;
        void close() override;

        std::tuple<uint32_t, std::unique_ptr<message<UseJson> const>> try_get_message(uint16_t ms_to_wait = 0) override;
        bool is_queue_empty() override;
    private:
        bool _closing;
        std::unique_ptr<RdKafka::KafkaConsumer> _consumer;
    };
}