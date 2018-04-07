/*
    Realm of Aesir
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

#include "test_message.h"

using namespace roa;
using namespace std;

template <bool UseJson>
test_message<UseJson>::test_message(message_sender sender) noexcept : message<UseJson>(sender) {

}

template <bool UseJson>
test_message<UseJson>::~test_message() {

}

template <bool UseJson>
std::string const test_message<UseJson>::serialize() const {
    return "";
}

template class test_message<false>;
template class test_message<true>;