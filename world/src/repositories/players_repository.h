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
#include "models/player_model.h"

namespace roa {
    enum class included_tables : int
    {
        none,
        stats,
        location,
        items,
        all
    };

    class iplayers_repository : public irepository {
    public:
        virtual ~iplayers_repository() = default;

        /**
         * Insert or update player
         * @param plyr
         * @param transaction
         * @return true if inserted, false if updated
         */
        virtual bool insert_or_update_player(player& plyr, std::unique_ptr<idatabase_transaction> const &transaction) = 0;

        /**
         * Update a player
         * @param plyr
         * @param transaction
         */
        virtual void update_player(player& plyr, std::unique_ptr<idatabase_transaction> const &transaction) = 0;

        /**
         * Get player by name
         * @param name name of player
         * @param transaction
         * @return player if found, {} if not
         */
        virtual STD_OPTIONAL<player> get_player(std::string const & name, included_tables includes, std::unique_ptr<idatabase_transaction> const &transaction) = 0;

        /**
         * Get player by player id
         * @param id id of player
         * @param transaction
         * @return player if found, {} if not
         */
        virtual STD_OPTIONAL<player> get_player(uint64_t id, included_tables includes, std::unique_ptr<idatabase_transaction> const &transaction) = 0;

        /**
         * Get players by user id
         * @param user_id id of player
         * @param transaction
         * @return players if found, {} if not
         */
        virtual std::vector<player> get_players_by_user_id(uint64_t user_id, included_tables includes,
                                                           std::unique_ptr<idatabase_transaction> const &transaction) = 0;
    };

    class players_repository : public repository, public iplayers_repository {
    public:
        explicit players_repository(std::shared_ptr<idatabase_pool> database_pool);
        players_repository(players_repository &repo);
        players_repository(players_repository &&repo);
        ~players_repository();

        auto create_transaction() -> decltype(repository::create_transaction()) override;

        bool insert_or_update_player(player& plyr, std::unique_ptr<idatabase_transaction> const &transaction) override;
        void update_player(player& plyr, std::unique_ptr<idatabase_transaction> const &transaction) override;
        STD_OPTIONAL<player> get_player(std::string const & name, included_tables includes, std::unique_ptr<idatabase_transaction> const &transaction) override;
        STD_OPTIONAL<player> get_player(uint64_t id, included_tables includes, std::unique_ptr<idatabase_transaction> const &transaction) override;
        std::vector<player> get_players_by_user_id(uint64_t user_id, included_tables includes,
                                                   std::unique_ptr<idatabase_transaction> const &transaction) override;
    };
}