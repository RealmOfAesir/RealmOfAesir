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

#include "users_repository.h"
#include "banned_users_repository.h"
#include "repository_exceptions.h"

#include <pqxx/pqxx>
#include <easylogging++.h>

#include <sql_exceptions.h>
#include <macros.h>

using namespace std;
using namespace chrono;
using namespace experimental;
using namespace roa;
using namespace pqxx;

banned_users_repository::banned_users_repository(std::shared_ptr<idatabase_pool> database_pool) : repository(database_pool) {

}

banned_users_repository::banned_users_repository(banned_users_repository &repo) : repository(repo._database_pool) {

}

banned_users_repository::banned_users_repository(banned_users_repository &&repo) : repository(repo._database_pool) {

}

banned_users_repository::~banned_users_repository() {

}

auto banned_users_repository::create_transaction() -> decltype(repository::create_transaction()) {
    return repository::create_transaction();
}

void banned_users_repository::insert_banned_user(banned_user &usr, std::unique_ptr<idatabase_transaction> const &transaction) {
    string ip = usr.ip.size() > 0 ? "'" + transaction->escape(usr.ip) + "'" : "NULL";
    string user_id = usr._user ? to_string(usr._user->id) : "NULL";
    string until = usr.until ? "to_timestamp(" + to_string(duration_cast<seconds>(usr.until->time_since_epoch()).count()) + ")" : "NULL";

    auto result = transaction->execute(
            "INSERT INTO banned_users (ip, user_id, until) VALUES (" + ip + ", " + user_id + ", " + until + ") RETURNING id");

    LOG(DEBUG) << NAMEOF(banned_users_repository::insert_banned_user) << "insert_banned_user contains " << result.size() << " entries";

    if(result.size() == 0) {
        LOG(ERROR) << NAMEOF(banned_users_repository::insert_banned_user) << "insert_banned_user unknown result";
        return;
    }

    usr.id = result[0][0].as<uint64_t>();
}

void banned_users_repository::update_banned_user(banned_user &usr, std::unique_ptr<idatabase_transaction> const &transaction) {
    string ip = usr.ip.size() > 0 ? "'" + transaction->escape(usr.ip) + "'" : "NULL";
    string user_id = usr._user ? to_string(usr._user->id) : "NULL";
    string until = usr.until ? "to_timestamp(" + to_string(duration_cast<seconds>(usr.until->time_since_epoch()).count()) + ")" : "NULL";

    transaction->execute(
            "UPDATE banned_users SET ip = " + ip + ", user_id = " + user_id + ", until = " + until);
}

STD_OPTIONAL<banned_user>
banned_users_repository::is_username_or_ip_banned(STD_OPTIONAL<std::string> username, STD_OPTIONAL<std::string> ip,
                                                  std::unique_ptr<idatabase_transaction> const &transaction) {
    if(!username && !ip) {
        LOG(ERROR) << NAMEOF(banned_users_repository::is_username_or_ip_banned) << " no arguments supplied";
        throw wrong_parameters_exception();
    }

    uint64_t usr_id;
    string ip_ret{};
    optional<system_clock::time_point> until;
    auto now = "to_timestamp(" + to_string(duration_cast<seconds>(system_clock::now().time_since_epoch()).count()) + ")";
    int64_t until_int;

    if(username && ip) {
        auto result = transaction->execute("SELECT bu.id as id, bu.ip, extract(epoch from bu.until)::bigint as until FROM banned_users bu "s +
                                           "LEFT JOIN users u ON bu.user_id = u.id AND u.username = '"s + transaction->escape(username.value()) + "' "s +
                                           "WHERE bu.until >= " + now + " AND (u.id IS NOT NULL OR bu.ip = '"s + transaction->escape(ip.value()) + "')"s);

        if(result.size() == 0) {
            return {};
        }

        if(!result[0]["ip"].is_null()) {
            ip_ret = result[0]["ip"].as<string>();
        }

        if(!result[0]["until"].is_null()) {
            until_int = result[0]["until"].as<int64_t>();
            until = system_clock::time_point(seconds(until_int));
        }

        usr_id = result[0]["id"].as<uint64_t>();
    } else if(username) {
        auto result = transaction->execute("SELECT bu.id as id, bu.ip, extract(epoch from bu.until)::bigint as until FROM banned_users bu "s +
                                           "LEFT JOIN users u ON bu.user_id = u.id AND u.username = '"s + transaction->escape(username.value()) + "' "s +
                                           "WHERE bu.until >= " + now + " AND u.id IS NOT NULL"s);

        if(result.size() == 0) {
            return {};
        }

        if(!result[0]["ip"].is_null()) {
            ip_ret = result[0]["ip"].as<string>();
        }

        if(!result[0]["until"].is_null()) {
            until_int = result[0]["until"].as<int64_t>();
            until = system_clock::time_point(seconds(until_int));
        }

        usr_id = result[0]["id"].as<uint64_t>();
    } else {
        auto result = transaction->execute("SELECT bu.id as id, bu.ip, extract(epoch from bu.until)::bigint as until FROM banned_users bu "s +
                                           "WHERE bu.until >= " + now + " AND bu.ip = '"s + transaction->escape(ip.value()) + "'"s);

        if(result.size() == 0) {
            return {};
        }

        if(!result[0]["ip"].is_null()) {
            ip_ret = result[0]["ip"].as<string>();
        }

        if(!result[0]["until"].is_null()) {
            until_int = result[0]["until"].as<int64_t>();
            until = system_clock::time_point(seconds(until_int));
        }

        usr_id = result[0]["id"].as<uint64_t>();
    }

    LOG(DEBUG) << NAMEOF(banned_users_repository::is_username_or_ip_banned) << " returning {" << usr_id << ", " << ip_ret << ", {}, " << until_int << "}";

    return make_optional<banned_user>({usr_id, ip_ret, {}, until});
}

optional<banned_user> banned_users_repository::get_banned_user(uint64_t id, std::unique_ptr<idatabase_transaction> const &transaction) {
    auto result = transaction->execute("SELECT id, ip, user_id, extract(epoch from until)::bigint as until FROM banned_users "s +
                               "WHERE id = " + to_string(id));

    LOG(DEBUG) << NAMEOF(banned_users_repository::get_banned_user) << " username contains " << result.size() << " entries";

    if(result.size() == 0) {
        return {};
    }

    string ip{};
    optional<user> _user;
    optional<system_clock::time_point> until;

    if(!result[0]["ip"].is_null()) {
        ip = result[0]["ip"].as<string>();
    }

    if(!result[0]["user_id"].is_null()) {
        _user = make_optional<user>({result[0]["user_id"].as<uint64_t>()});
    }

    if(!result[0]["until"].is_null()) {
        until = system_clock::time_point(seconds(result[0]["until"].as<int64_t>()));
    }

    return make_optional<banned_user>({result[0]["id"].as<uint64_t>(), ip, _user, until});
}
