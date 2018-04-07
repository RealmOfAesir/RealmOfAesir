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
#include "models/location_model.h"

namespace roa {
    class ilocations_repository : public irepository {
    public:
        virtual ~ilocations_repository() = default;

        /**
         * Insert location
         * @param loc
         * @param transaction
         */
        virtual void insert_location(location& loc, std::unique_ptr<idatabase_transaction> const &transaction) = 0;

        /**
         * Update location
         * @param loc
         * @param transaction
         */
        virtual void update_location(location& loc, std::unique_ptr<idatabase_transaction> const &transaction) = 0;

        /**
         * Get location
         * @param id
         * @param transaction
         * @return location if found, {} if not
         */
        virtual STD_OPTIONAL<location> get_location(uint64_t id, std::unique_ptr<idatabase_transaction> const &transaction) = 0;

        /**
         * Get all locations by map_id
         * @param map_id
         * @param transaction
         * @return vector that may or may not be empty
         */
        virtual std::vector<location> get_locations_by_map_id(uint32_t map_id,
                                                              std::unique_ptr<idatabase_transaction> const &transaction) = 0;
    };

    class locations_repository : public repository, public ilocations_repository {
    public:
        explicit locations_repository(std::shared_ptr<idatabase_pool> database_pool);
        locations_repository(locations_repository &repo);
        locations_repository(locations_repository &&repo);
        ~locations_repository();

        auto create_transaction() -> decltype(repository::create_transaction()) override;

        void insert_location(location& loc, std::unique_ptr<idatabase_transaction> const &transaction) override;
        void update_location(location& loc, std::unique_ptr<idatabase_transaction> const &transaction) override;
        STD_OPTIONAL<location> get_location(uint64_t id, std::unique_ptr<idatabase_transaction> const &transaction) override;
        std::vector<location> get_locations_by_map_id(uint32_t map_id,
                                                      std::unique_ptr<idatabase_transaction> const &transaction) override;
    };
}