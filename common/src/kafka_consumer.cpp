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

#include "kafka_consumer.h"
#include "exceptions.h"
#include "macros.h"

#include <easylogging++.h>
#include <chrono>
#include <thread>

using namespace std;
using namespace roa;

/*int partition_cnt = 0;
int eof_cnt = 0;

class ExampleRebalanceCb : public RdKafka::RebalanceCb {
private:
    static void part_list_print (const std::vector<RdKafka::TopicPartition*>&partitions){
        for (unsigned int i = 0 ; i < partitions.size() ; i++)
            std::cerr << partitions[i]->topic() <<
                      "[" << partitions[i]->partition() << "], ";
        std::cerr << "\n";
    }

public:
    void rebalance_cb (RdKafka::KafkaConsumer *consumer,
                       RdKafka::ErrorCode err,
                       std::vector<RdKafka::TopicPartition*> &partitions) {
        std::cerr << "RebalanceCb: " << RdKafka::err2str(err) << ": ";

        part_list_print(partitions);

        for(auto partition : partitions) {
            partition->set_offset(RdKafka::Topic::OFFSET_BEGINNING);
        }

        if (err == RdKafka::ERR__ASSIGN_PARTITIONS) {
            consumer->assign(partitions);
            partition_cnt = (int)partitions.size();
        } else {
            consumer->unassign();
            partition_cnt = 0;
        }
        eof_cnt = 0;
    }
};

ExampleRebalanceCb ex_rebalance_cb;*/

template <bool UseJson>
kafka_consumer<UseJson>::kafka_consumer()
    : _closing(), _consumer() {

}

template <bool UseJson>
kafka_consumer<UseJson>::~kafka_consumer() {
    close();
}

template <bool UseJson>
tuple<uint32_t, unique_ptr<message<UseJson> const>> kafka_consumer<UseJson>::try_get_message(uint16_t ms_to_wait) {
    if(unlikely(!_consumer)) {
        LOG(ERROR) << "[kafka_consumer] No consumer";
        throw kafka_exception("[kafka_consumer] No consumer");
    }

    if(unlikely(_closing)) {
        return {};
    }

    unique_ptr<RdKafka::Message> msg;
    {
        RdKafka::Message *raw_msg = _consumer->consume(ms_to_wait);

        if (!raw_msg) {
            LOG(WARNING) << "[kafka_consumer] No message";
            return {};
        }

        msg.reset(raw_msg);
    }

    if(msg->err() != RdKafka::ERR_NO_ERROR && msg->err() != RdKafka::ERR__TIMED_OUT) {
        LOG(WARNING) << "[kafka_consumer] Message error: " << msg->errstr();
        return {};
    }

    if(msg->len() == 0) {
        return {};
    }

    LOG(INFO) << "[kafka_consumer] Received message at offset " << msg->offset() << " with size " << msg->len();

    char* payload = static_cast<char*>(msg->payload());
    string payload_string = string(payload, msg->len());

    if(payload_string.size() == 0) {
        LOG(INFO) << "[kafka_consumer] Converted message is empty";
        return {};
    }

    return move(message<UseJson>::template deserialize<UseJson>(payload_string));
}

template <bool UseJson>
bool kafka_consumer<UseJson>::is_queue_empty() {
    return _consumer && _consumer->outq_len() == 0;
}

template <bool UseJson>
void kafka_consumer<UseJson>::start(std::string broker_list, std::string group_id, std::vector<std::string> topics, uint32_t fetch_wait_max_ms, bool debug) {
    string errstr;
    RdKafka::Conf *conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    RdKafka::Conf *topic_conf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);

    if(broker_list.empty()) {
        LOG(ERROR) << "[kafka_consumer] broker_list empty";
        throw kafka_exception("[kafka_consumer] broker_list empty");
    }

    if(group_id.empty()) {
        LOG(ERROR) << "[kafka_consumer] group_id empty";
        throw kafka_exception("[kafka_consumer] group_id empty");
    }

    if(topics.empty()) {
        LOG(ERROR) << "[kafka_consumer] topics empty";
        throw kafka_exception("[kafka_consumer] topics empty");
    }

    if(conf->set("api.version.request", "true", errstr) != RdKafka::Conf::CONF_OK) {
        LOG(ERROR) << "[kafka_consumer] api.version.request " << errstr;
        throw kafka_exception("[kafka_consumer] api.version.request");
    }

    if(conf->set("group.id", group_id, errstr) != RdKafka::Conf::CONF_OK) {
        LOG(ERROR) << "[kafka_consumer] group.id " << errstr;
        throw kafka_exception("[kafka_consumer] group.id");
    }

    if(conf->set("metadata.broker.list", broker_list, errstr) != RdKafka::Conf::CONF_OK) {
        LOG(ERROR) << "[kafka_consumer] metadata.broker.list " << errstr;
        throw kafka_exception("[kafka_consumer] metadata.broker.list");
    }

    if(conf->set("default_topic_conf", topic_conf, errstr) != RdKafka::Conf::CONF_OK) {
        LOG(ERROR) << "[kafka_consumer] default_topic_conf " << errstr;
        throw kafka_exception("[kafka_consumer] default_topic_conf");
    }

    if(conf->set("fetch.wait.max.ms", to_string(fetch_wait_max_ms), errstr) != RdKafka::Conf::CONF_OK) {
        LOG(ERROR) << "[kafka_consumer] fetch.wait.max.ms " << errstr;
        throw kafka_exception("[kafka_consumer] fetch.wait.max.ms");
    }

    if(conf->set("fetch.error.backoff.ms", "250", errstr) != RdKafka::Conf::CONF_OK) {
        LOG(ERROR) << "[kafka_consumer] fetch.error.backoff.ms " << errstr;
        throw kafka_exception("[kafka_consumer] fetch.error.backoff.ms");
    }

    /*if(conf->set("rebalance_cb", &ex_rebalance_cb, errstr) != RdKafka::Conf::CONF_OK) {
        LOG(ERROR) << "[kafka_consumer] rebalance_cb " << errstr;
        throw kafka_exception("[kafka_consumer] rebalance_cb");
    }*/

    if(debug) {
        if(conf->set("debug", "cgrp,topic,fetch", errstr) != RdKafka::Conf::CONF_OK) {
            LOG(ERROR) << "[kafka_consumer] debug " << errstr;
            throw kafka_exception("[kafka_consumer] debug");
        }
    }

    RdKafka::KafkaConsumer *consumer = RdKafka::KafkaConsumer::create(conf, errstr);
    if (consumer == nullptr) {
        LOG(ERROR) << "[kafka_consumer] Failed to create consumer: " << errstr;
        throw kafka_exception("[kafka_consumer] Failed to create consumer");
    }

    _consumer.reset(consumer);

    RdKafka::ErrorCode err = _consumer->subscribe(topics);
    if (err != RdKafka::ErrorCode::ERR_NO_ERROR) {
        LOG(ERROR) << "[kafka_consumer] Failed to subscribe to topics: " << RdKafka::err2str(err);
        this->close();
        throw kafka_exception("[kafka_consumer] Failed to subscribe to topics");
    }

    delete topic_conf;
    delete conf;

    LOG(INFO) << "[kafka_consumer] Created consumer " << _consumer->name();
}

template <bool UseJson>
void kafka_consumer<UseJson>::close() {
    if(unlikely(!_consumer)) {
        return;
    }

    if(unlikely(_closing)) {
        throw new kafka_exception("[kafka_consumer] already closing consumer");
    }

    _closing = true;
    _consumer->commitSync();
    auto now = chrono::system_clock::now().time_since_epoch().count();
    auto wait_until = (chrono::system_clock::now() += 2000ms).time_since_epoch().count();

    while(!is_queue_empty() && now < wait_until) {
        this_thread::sleep_for(100ms);
        now = chrono::system_clock::now().time_since_epoch().count();
        LOG(INFO) << "[kafka_consumer] waiting for outq_len";
    }
    _consumer.reset();
    _closing = false;
}

template class kafka_consumer<false>;
template class kafka_consumer<true>;