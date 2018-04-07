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
#include "models/script_zone_model.h"

namespace roa {
    class iscript_zones_repository : public irepository {
    public:
        virtual ~iscript_zones_repository() = default;

        /**
         * Insert script zone
         * @param zone
         * @param transaction
         */
        virtual void insert_script_zone(script_zone& zone, std::unique_ptr<idatabase_transaction> const &transaction) = 0;

        /**
         * Update a specific zone
         * @param zone
         * @param transaction
         */
        virtual void update_script_zone(script_zone& zone, std::unique_ptr<idatabase_transaction> const &transaction) = 0;

        /**
         * get specific zone
         * @param id
         * @param transaction
         * @return optional zone
         */
        virtual STD_OPTIONAL<script_zone> get_script_zone(uint32_t id, std::unique_ptr<idatabase_transaction> const &transaction) = 0;
    };

    class script_zones_repository : public repository, public iscript_zones_repository {
    public:
        explicit script_zones_repository(std::shared_ptr<idatabase_pool> database_pool);
        script_zones_repository(script_zones_repository &repo);
        script_zones_repository(script_zones_repository &&repo);
        ~script_zones_repository();

        auto create_transaction() -> decltype(repository::create_transaction()) override;

        void insert_script_zone(script_zone& zone, std::unique_ptr<idatabase_transaction> const &transaction) override;
        void update_script_zone(script_zone& zone, std::unique_ptr<idatabase_transaction> const &transaction) override;
        STD_OPTIONAL<script_zone> get_script_zone(uint32_t id, std::unique_ptr<idatabase_transaction> const &transaction) override;
    };
}