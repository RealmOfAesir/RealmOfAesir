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

#include "sprite.h"

#include <array>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <easylogging++.h>

#include "shader_utils.h"
#include "texture_manager.h"

using namespace std;

#ifdef EXPERIMENTAL_OPTIONAL
using namespace experimental;
#endif

sprite::sprite(std::shared_ptr<texture_atlas> texture_atlas, glm::vec4 const position, optional<glm::vec4> const clip) noexcept
     : _texture_atlas(texture_atlas), _position(position) {

    float x = position.x;
    float y = position.y;
    float w = position.z;
    float h = position.w;

    array<GLfloat, 16> vertexData = {
        x, y, 0.0f, 0.0f,
        x+w, y, 1.0f, 0.0f,
        x, y+h, 0.0f, 1.0f,
        x+w, y+h, 1.0f, 1.0f
    };

    if(clip) {
        if(clip.value().x < 0 || clip.value().x > _texture_atlas->texture_width() || clip.value().y < 0 || clip.value().y > _texture_atlas->texture_height()) {
            LOG(FATAL) << "clip out of bounds";
        }

        vertexData[2] = clip.value().x / _texture_atlas->texture_width();
        vertexData[3] = clip.value().y / _texture_atlas->texture_height();

        vertexData[6] = (clip.value().x + clip.value().z) / _texture_atlas->texture_width();
        vertexData[7] = clip.value().y / _texture_atlas->texture_height();

        vertexData[10] = clip.value().x / _texture_atlas->texture_width();
        vertexData[11] = (clip.value().y + clip.value().w) / _texture_atlas->texture_height();

        vertexData[14] = (clip.value().x + clip.value().z) / _texture_atlas->texture_width();
        vertexData[15] = (clip.value().y + clip.value().w) / _texture_atlas->texture_height();
    }

    _vertex_data_position = _texture_atlas->add_data_object(vertexData);
}

sprite::~sprite() noexcept {
    _texture_atlas->remove_data_object(_vertex_data_position);
}
