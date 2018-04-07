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

#include <memory>
#include <string>
#include <pqxx/pqxx>

namespace roa {
    class idatabase_transaction {
    public:
        virtual ~idatabase_transaction() = default;

        /**
         * Execute sql query and return result
         * @param query
         * @return result
         */
        virtual pqxx::result execute(std::string query) = 0;

        /**
         * prevent sql injection
         * @param element
         * @return escaped element
         */
        virtual std::string escape(std::string element) = 0;

        /**
         * In the case of update or insertions, commit the transaction to go through with the change.
         */
        virtual void commit() = 0;
    };

    class database_transaction : public idatabase_transaction {
    public:
        explicit database_transaction(std::shared_ptr<pqxx::connection> connection) noexcept;

        ~database_transaction();

        pqxx::result execute(std::string query) override;
        std::string escape(std::string element) override;
        void commit() override;

    private:
        std::shared_ptr <pqxx::connection> _connection;
        pqxx::work _transaction;
    };
}