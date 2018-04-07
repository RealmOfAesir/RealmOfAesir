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
#include <memory>
#include <pqxx/pqxx>

namespace roa {
    class idatabase_transaction;
    class database_pool;

    class idatabase_connection {
    public:
        virtual ~idatabase_connection() = default;

        /**
         * Creates a transaction
         * @return transaction object
         */
        virtual std::unique_ptr<idatabase_transaction> create_transaction() = 0;
    };

    class database_connection : public idatabase_connection {
    public:
        explicit database_connection(database_pool* database_pool, std::tuple<uint32_t, std::shared_ptr<pqxx::connection>> connection) noexcept;

        /**
         * Releases the connection upon destruction
         */
        ~database_connection();

        std::unique_ptr<idatabase_transaction> create_transaction() override;

    private:
        // Just hold a pointer, do not delete
        database_pool* _database_pool;
        std::tuple<uint32_t, std::shared_ptr<pqxx::connection>> _connection;
    };
}