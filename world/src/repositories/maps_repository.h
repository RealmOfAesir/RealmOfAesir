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
#include "models/map_model.h"

namespace roa {
    class imaps_repository : public irepository {
    public:
        virtual ~imaps_repository() = default;

        /**
         * Insert map
         * @param _map
         * @param transaction
         */
        virtual void insert_map(map& _map, std::unique_ptr<idatabase_transaction> const &transaction) = 0;

        /**
         * Update map
         * @param _map
         * @param transaction
         */
        virtual void update_map(map& _map, std::unique_ptr<idatabase_transaction> const &transaction) = 0;

        /**
         * Get map
         * @param id
         * @param transaction
         * @return map if found, {} if not
         */
        virtual STD_OPTIONAL<map> get_map(uint32_t id, std::unique_ptr<idatabase_transaction> const &transaction) = 0;

        /**
         * Get map
         * @param name
         * @param transaction
         * @return map if found, {} if not
         */
        virtual STD_OPTIONAL<map> get_map(std::string const &name, std::unique_ptr<idatabase_transaction> const &transaction) = 0;
    };

    class maps_repository : public repository, public imaps_repository {
    public:
        explicit maps_repository(std::shared_ptr<idatabase_pool> database_pool);
        maps_repository(maps_repository &repo);
        maps_repository(maps_repository &&repo);
        ~maps_repository();

        auto create_transaction() -> decltype(repository::create_transaction()) override;

        void insert_map(map& _map, std::unique_ptr<idatabase_transaction> const &transaction) override;
        void update_map(map& _map, std::unique_ptr<idatabase_transaction> const &transaction) override;
        STD_OPTIONAL<map> get_map(uint32_t id, std::unique_ptr<idatabase_transaction> const &transaction) override;
        STD_OPTIONAL<map> get_map(std::string const &name, std::unique_ptr<idatabase_transaction> const &transaction) override;
    };
}