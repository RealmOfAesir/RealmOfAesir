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
    struct setting {
        std::string name;
        std::string value;
    };

    class isettings_repository : public irepository {
    public:
        virtual ~isettings_repository() = default;

        /**
         * Insert or update setting
         * @param sett
         * @param transaction
         * @return true if inserted, false if updated
         */
        virtual bool insert_or_update_setting(setting& sett, std::unique_ptr<idatabase_transaction> const &transaction) = 0;

        /**
         * Get setting by name
         * @param name name of setting
         * @param transaction
         * @return setting if found, {} if not
         */
        virtual STD_OPTIONAL<setting> get_setting(std::string const & name, std::unique_ptr<idatabase_transaction> const &transaction) = 0;
    };

    class settings_repository : public repository, public isettings_repository {
    public:
        explicit settings_repository(std::shared_ptr<idatabase_pool> database_pool);
        settings_repository(settings_repository &repo);
        settings_repository(settings_repository &&repo);
        ~settings_repository();

        auto create_transaction() -> decltype(repository::create_transaction()) override;

        bool insert_or_update_setting(setting& sett, std::unique_ptr<idatabase_transaction> const &transaction) override;
        STD_OPTIONAL<setting> get_setting(std::string const & name, std::unique_ptr<idatabase_transaction> const &transaction) override;
    };
}