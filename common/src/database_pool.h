/*
    Realm of Aesir backend
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

#include <string>
#include <vector>
#include <tuple>
#include <memory>
#include <mutex>
#include <pqxx/pqxx>

namespace roa {
    class idatabase_connection;

    class idatabase_pool {
    public:
        virtual ~idatabase_pool() = default;

        /**
         * Get a connection from the pool, sleeps and retries infinitely until a connection is free
         * @return
         */
        virtual std::unique_ptr<idatabase_connection> get_connection() = 0;
    };

    class database_pool : public idatabase_pool {
    public:
        database_pool() noexcept;
        ~database_pool();

        void create_connections(std::string connection_string, uint32_t min_connections = 5);

        std::unique_ptr<idatabase_connection> get_connection() override;

        /**
         * Marks connection as available again
         * @param id
         */
        void release_connection(uint32_t id);
    private:
        std::string _connection_string;
        uint32_t _min_connections;
        std::vector<std::tuple<bool, uint32_t, std::shared_ptr<pqxx::connection>>> _connections;
        std::mutex _connections_mutex;
    };
}