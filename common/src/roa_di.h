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

#include <boost/di.hpp>

namespace roa {
    template <bool UseJson>
    class ikafka_consumer;
    template <bool UseJson>
    class kafka_consumer;

    template <bool UseJson>
    class ikafka_producer;
    template <bool UseJson>
    class kafka_producer;

    auto create_common_di_injector = []() {
        return boost::di::make_injector(
                boost::di::bind<ikafka_consumer<true>>().to<kafka_consumer<true>>(),
                boost::di::bind<ikafka_producer<true>>().to<kafka_producer<true>>(),
                boost::di::bind<ikafka_consumer<false>>().to<kafka_consumer<false>>(),
                boost::di::bind<ikafka_producer<false>>().to<kafka_producer<false>>()
        );
    };
}