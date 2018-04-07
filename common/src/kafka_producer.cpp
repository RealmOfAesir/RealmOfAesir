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

#include "kafka_producer.h"
#include "exceptions.h"
#include "macros.h"

#include <easylogging++.h>
#include <chrono>

using namespace std;
using namespace roa;

void producer_event_callback::event_cb (RdKafka::Event &event) {
    switch (event.type())
    {
        case RdKafka::Event::EVENT_ERROR:
            LOG(ERROR) << "[kafka_producer] ERROR (" << RdKafka::err2str(event.err()) << "): " << event.str();
            break;

        case RdKafka::Event::EVENT_STATS:
            LOG(ERROR) << "[kafka_producer] \"STATS\": " << event.str();
            break;

        case RdKafka::Event::EVENT_LOG:
            LOG(ERROR) << "[kafka_producer] LOG-"<< event.severity() << "-" << event.fac() << ": " << event.str();
            break;

        default:
            LOG(ERROR) << "[kafka_producer] EVENT " << event.type() << " (" << RdKafka::err2str(event.err()) << "): " << event.str();
            break;
    }
}

int32_t producer_hash_partitioner_callback::partitioner_cb (const RdKafka::Topic *topic, const std::string *key, int32_t partition_cnt, void *msg_opaque) {
    return djb_hash(key->c_str(), key->size()) % partition_cnt;
    //return 5381;
}

inline unsigned int producer_hash_partitioner_callback::djb_hash (const char *str, size_t len) {
    unsigned int hash = 5381;
    for (size_t i = 0 ; i < len ; i++) {
        hash = ((hash << 5) + hash) + str[i];
    }
    return hash;
}

void producer_delivery_callback::dr_cb (RdKafka::Message &message) {
    if(message.err() != RdKafka::ERR_NO_ERROR) {
        LOG(ERROR) << "[kafka_producer] Message delivery for (" << message.len() << " bytes): " << message.errstr();
    }
    LOG(INFO) << "[kafka_producer] Message delivery for (" << message.len() << " bytes " << message.offset() << " offset): " << message.errstr();
}


template <bool UseJson>
kafka_producer<UseJson>::kafka_producer()
    : _closing(), _producer(), _topics(), _topics_mutex(),
      _hash_partitioner_callback(), _delivery_callback(), _event_callback() {

}

template <bool UseJson>
kafka_producer<UseJson>::~kafka_producer() {
    close();
}

template <bool UseJson>
void kafka_producer<UseJson>::enqueue_message(std::string topic_str, message<UseJson> const &msg) {
    if(unlikely(!_producer) || unlikely(topic_str.empty())) {
        LOG(ERROR) << "[kafka_producer] No producer or topic";
        throw kafka_exception("[kafka_producer] No producer or topic");
    }

    if(unlikely(_closing)) {
        LOG(WARNING) << "[kafka_producer] is closing";
        return;
    }

    RdKafka::Topic *topic;
    {
        lock_guard<mutex> l(_topics_mutex);
        auto topic_iter = _topics.find(topic_str);
        if (unlikely(topic_iter == end(_topics))) {
            std::string errstr;
            auto topic_conf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);

            if (topic_conf->set("partitioner_cb", &_hash_partitioner_callback, errstr) != RdKafka::Conf::CONF_OK) {
                LOG(ERROR) << "[kafka_producer] partitioner_cb " << errstr;
                throw kafka_exception("[kafka_producer] partitioner_cb");
            }

            topic = RdKafka::Topic::create(_producer.get(), topic_str, topic_conf, errstr);
            if (!topic) {
                LOG(ERROR) << "[kafka_producer] Failed to create topic: " << errstr;
                throw kafka_exception("[kafka_producer] Failed to create topic");
            }

            _topics[topic_str] = unique_ptr<RdKafka::Topic>(topic);

            delete topic_conf;
        } else {
            topic = get<1>(*topic_iter).get();
        }
    }

    auto msg_str = msg.serialize();

    RdKafka::ErrorCode resp;
    do {
        resp = _producer->produce(topic, RdKafka::Topic::PARTITION_UA,
                                                       RdKafka::Producer::RK_MSG_COPY,
                                                       const_cast<char *>(msg_str.c_str()), msg_str.size(), NULL, NULL);

        if (resp != RdKafka::ERR_NO_ERROR && resp != RdKafka::ERR__QUEUE_FULL) {
            LOG(ERROR) << "[kafka_producer] Produce failed: " << RdKafka::err2str(resp);
        }

        if(resp == RdKafka::ERR__QUEUE_FULL) {
            _producer->poll(10);
        }

    } while(resp == RdKafka::ERR__QUEUE_FULL);
}

template <bool UseJson>
void kafka_producer<UseJson>::enqueue_message(std::string topic_str, message<UseJson> const * const msg) {
    this->enqueue_message(topic_str, *msg);
}

template <bool UseJson>
bool kafka_producer<UseJson>::is_queue_empty() {
    return _producer && _producer->outq_len() == 0;
}

template <bool UseJson>
int kafka_producer<UseJson>::poll(uint32_t ms_to_wait) {
    if(unlikely(!_producer)) {
        LOG(ERROR) << "[kafka_producer] No producer";
        throw kafka_exception("[kafka_producer] No producer");
    }

    return _producer->poll(ms_to_wait);
}

template <bool UseJson>
void kafka_producer<UseJson>::start(std::string broker_list, uint32_t queue_buffering_max_ms, bool disable_nagle, bool debug) {
    auto conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);

    std::string errstr;

    if(broker_list.empty()) {
        LOG(ERROR) << "[kafka_producer] broker_list empty";
        throw kafka_exception("[kafka_producer] broker_list empty");
    }

    if(conf->set("api.version.request", "true", errstr) != RdKafka::Conf::CONF_OK) {
        LOG(ERROR) << "[kafka_producer] api.version.request " << errstr;
        throw kafka_exception("[kafka_producer] api.version.request");
    }

    if (conf->set("metadata.broker.list", broker_list, errstr) != RdKafka::Conf::CONF_OK) {
        LOG(ERROR) << "[kafka_producer] metadata.broker.list " << errstr;
        throw kafka_exception("[kafka_producer] metadata.broker.list");
    }

    if (conf->set("event_cb", &_event_callback, errstr) != RdKafka::Conf::CONF_OK) {
        LOG(ERROR) << "[kafka_producer] event_cb " << errstr;
        throw kafka_exception("[kafka_producer] event_cb");
    }

    if (conf->set("dr_cb", &_delivery_callback, errstr) != RdKafka::Conf::CONF_OK) {
        LOG(ERROR) << "[kafka_producer] dr_cb " << errstr;
        throw kafka_exception("[kafka_producer] dr_cb");
    }

    if (conf->set("queue.buffering.max.ms", to_string(queue_buffering_max_ms), errstr) != RdKafka::Conf::CONF_OK) {
        LOG(ERROR) << "[kafka_producer] queue.buffering.max.ms " << errstr;
        throw kafka_exception("[kafka_producer] queue.buffering.max.ms");
    }

    if(disable_nagle) {
        if (conf->set("socket.nagle.disable", "true", errstr) != RdKafka::Conf::CONF_OK) {
            LOG(ERROR) << "[kafka_producer] socket.nagle.disable " << errstr;
            throw kafka_exception("[kafka_producer] socket.nagle.disable");
        }
    }

    if(debug) {
        if(conf->set("debug", "broker,topic,msg", errstr) != RdKafka::Conf::CONF_OK) {
            LOG(ERROR) << "[kafka_producer] debug " << errstr;
            throw kafka_exception("[kafka_producer] debug");
        }
    }

    RdKafka::Producer *producer = RdKafka::Producer::create(conf, errstr);
    if(!producer) {
        LOG(ERROR) << "[kafka_producer] Failed to create producer: " << errstr;
        throw kafka_exception("[kafka_producer] Failed to create producer");
    }

    LOG(INFO) << "[kafka_producer] created producer " << producer->name();

    delete conf;

    _producer.reset(producer);
}

template <bool UseJson>
void kafka_producer<UseJson>::close() {
    if(unlikely(!_producer)) {
        return;
    }

    if(unlikely(_closing)) {
        throw new kafka_exception("[kafka_producer] already closing producer");
    }

    lock_guard<mutex> l(_topics_mutex);

    _closing = true;
    auto now = chrono::system_clock::now().time_since_epoch().count();
    auto wait_until = (chrono::system_clock::now() += 2000ms).time_since_epoch().count();

    while(!is_queue_empty() && now < wait_until) {
        _producer->poll(100);
        now = chrono::system_clock::now().time_since_epoch().count();
    }

    _topics.clear();
    _producer.reset();
    _closing = false;
}

template class kafka_producer<false>;
template class kafka_producer<true>;