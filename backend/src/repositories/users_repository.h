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
#include "repository.h"

namespace roa {
    struct user {
        uint64_t id;
        std::string username;
        std::string password;
        std::string email;
        int16_t login_attempts;
        int16_t admin;
        int16_t no_of_players;
    };

    class iusers_repository : public irepository {
    public:
        virtual ~iusers_repository() = default;

        /**
         * Insert a user
         * @param usr Reference to user, id gets set on succesful insertion
         * @param transaction const reference to a transaction
         * @return true if insert, false if not
         */
        virtual bool insert_user_if_not_exists(user& usr, std::unique_ptr<idatabase_transaction> const &transaction) = 0;

        /**
         * Update a user
         * @param usr
         * @param transaction const reference to a transaction
         */
        virtual void update_user(user const & usr, std::unique_ptr<idatabase_transaction> const &transaction) = 0;

        /**
         * Get user by username
         * @param username
         * @param transaction const reference to a transaction
         * @return user
         * @throws not_found_exception
         */
        virtual STD_OPTIONAL<user> get_user(std::string const & username, std::unique_ptr<idatabase_transaction> const &transaction) = 0;

        /**
         * Get user by id
         * @param id
         * @param transaction const reference to a transaction
         * @return user
         * @throws not_found_exception
         */
        virtual STD_OPTIONAL<user> get_user(uint64_t id, std::unique_ptr<idatabase_transaction> const &transaction) = 0;
    };

    class users_repository : public repository, public iusers_repository {
    public:
        explicit users_repository(std::shared_ptr<idatabase_pool> database_pool);
        users_repository(users_repository &repo);
        users_repository(users_repository &&repo);
        ~users_repository();

        auto create_transaction() -> decltype(repository::create_transaction()) override;

        bool insert_user_if_not_exists(user& usr, std::unique_ptr<idatabase_transaction> const &transaction) override;
        void update_user(user const & usr, std::unique_ptr<idatabase_transaction> const &transaction) override;
        STD_OPTIONAL<user> get_user(std::string const & username, std::unique_ptr<idatabase_transaction> const &transaction) override;
        STD_OPTIONAL<user> get_user(uint64_t id, std::unique_ptr<idatabase_transaction> const &transaction) override;
    };
}