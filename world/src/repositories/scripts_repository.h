/*
    RealmOfAesirWorld
    Copyright (C) 2017  Michael de Lang

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
#include "models/script_model.h"

namespace roa {
    class iscripts_repository : public irepository {
    public:
        virtual ~iscripts_repository() = default;

        /**
         * Insert or update script
         * @param scr
         * @param transaction
         * @return true if inserted, false if updated
         */
        virtual bool insert_or_update_script(script& scr, std::unique_ptr<idatabase_transaction> const &transaction) = 0;

        /**
         * Get script by name
         * @param name name of script
         * @param transaction
         * @return setting if found, {} if not
         */
        virtual STD_OPTIONAL<script> get_script(std::string const & name, std::unique_ptr<idatabase_transaction> const &transaction) = 0;
    };

    class scripts_repository : public repository, public iscripts_repository {
    public:
        explicit scripts_repository(std::shared_ptr<idatabase_pool> database_pool);
        scripts_repository(scripts_repository &repo);
        scripts_repository(scripts_repository &&repo);
        ~scripts_repository();

        auto create_transaction() -> decltype(repository::create_transaction()) override;

        bool insert_or_update_script(script& scr, std::unique_ptr<idatabase_transaction> const &transaction) override;
        STD_OPTIONAL<script> get_script(std::string const & name, std::unique_ptr<idatabase_transaction> const &transaction) override;
    };
}