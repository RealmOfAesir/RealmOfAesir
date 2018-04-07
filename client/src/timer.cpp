/*
    Realm of Aesir client
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

#include "timer.h"

#include <SDL.h>

timer::timer() noexcept : _start_ticks(0), _paused_ticks(0), _paused(false), _started(false) {
}

void timer::start() noexcept {
    _started = true;
    _paused = false;

    _start_ticks = SDL_GetTicks();
    _paused_ticks = 0;
}

void timer::stop() noexcept {
    _started = false;
    _paused = false;

    _start_ticks = 0;
    _paused_ticks = 0;
}

void timer::pause() noexcept {
    if(_started && !_paused) {
        _paused = true;

        _paused_ticks = SDL_GetTicks() - _start_ticks;
        _start_ticks = 0;
    }
}

void timer::unpause() noexcept {
    if(_started && _paused) {
        _paused = false;

        _start_ticks = SDL_GetTicks() - _paused_ticks;
        _paused_ticks = 0;
    }
}

uint32_t timer::get_ticks() const noexcept {
    if(!_started) {
        return 0;
    }

    if(_paused) {
        return _paused_ticks;
    }

    return SDL_GetTicks() - _start_ticks;
}

bool timer::is_started() const noexcept {
    return _started;
}

bool timer::is_paused() const noexcept {
    return _paused;
}
