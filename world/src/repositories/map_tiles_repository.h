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
#include "models/map_tile_model.h"

namespace roa {
    class imap_tiles_repository : public irepository {
    public:
        virtual ~imap_tiles_repository() = default;

        /**
         * Insert tile
         * @param tile
         * @param transaction
         */
        virtual void insert_tile(map_tile& tile, std::unique_ptr<idatabase_transaction> const &transaction) = 0;

        /**
         * Update tile
         * @param tile
         * @param transaction
         */
        virtual void update_tile(map_tile& tile, std::unique_ptr<idatabase_transaction> const &transaction) = 0;

        /**
         * Get tile
         * @param id
         * @param transaction
         * @return tile if found, {} if not
         */
        virtual STD_OPTIONAL<map_tile> get_tile(uint64_t id, std::unique_ptr<idatabase_transaction> const &transaction) = 0;

        /**
         * Get all tiles belonging to a map
         * @param map_id
         * @param transaction
         * @return empty vector if not found, otherwise vector of found tiles
         */
        virtual std::vector<map_tile> get_tiles_for_map(uint32_t map_id, std::unique_ptr<idatabase_transaction> const &transaction) = 0;
    };

    class map_tiles_repository : public repository, public imap_tiles_repository {
    public:
        explicit map_tiles_repository(std::shared_ptr<idatabase_pool> database_pool);
        map_tiles_repository(map_tiles_repository &repo);
        map_tiles_repository(map_tiles_repository &&repo);
        ~map_tiles_repository();

        auto create_transaction() -> decltype(repository::create_transaction()) override;

        void insert_tile(map_tile& tile, std::unique_ptr<idatabase_transaction> const &transaction) override;
        void update_tile(map_tile& tile, std::unique_ptr<idatabase_transaction> const &transaction) override;
        STD_OPTIONAL<map_tile> get_tile(uint64_t id, std::unique_ptr<idatabase_transaction> const &transaction) override;
        std::vector<map_tile> get_tiles_for_map(uint32_t map_id, std::unique_ptr<idatabase_transaction> const &transaction) override;
    };
}