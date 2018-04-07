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
#include <custom_optional.h>
#include <database_pool.h>
#include <database_transaction.h>
#include <database_connection.h>

namespace roa {
    class irepository {
    public:
        virtual ~irepository() = default;

        /**
         * Non-thread safe function to create a transaction
         * @throws sql_transaction_already_created_exception if an transaction is already active
         */
        virtual std::tuple<std::unique_ptr<idatabase_connection>, std::unique_ptr<idatabase_transaction>> create_transaction() = 0;
    };

    class repository : public irepository {
    public:
        explicit repository(std::shared_ptr<idatabase_pool> database_pool);
        ~repository();

        std::tuple<std::unique_ptr<idatabase_connection>, std::unique_ptr<idatabase_transaction>> create_transaction() override;
    protected:
        std::shared_ptr<idatabase_pool> _database_pool;
    };
}