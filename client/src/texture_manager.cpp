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

#include "texture_manager.h"

#include <unordered_map>
#include <SDL.h>
#include <SDL_image.h>
#include <easylogging++.h>

#include "shader_utils.h"

using namespace std;

unordered_map<string, texture> texture_cache;

texture create_texture_from_image(string const & image) noexcept {

    auto found_texture = texture_cache.find(image);

    if(found_texture != texture_cache.end()) {
        found_texture->second._reference_count++;
        return found_texture->second;
    }

    SDL_Surface *surface = IMG_Load(image.c_str());
    if(!surface) {
        LOG(FATAL) << "[texture_manager] surface load failed: " << IMG_GetError();
    }

    GLenum texture_format;
    GLenum internal_format;
    GLenum texture_type;

    if(surface->format->BytesPerPixel == 4) {
        LOG(INFO) << "[texture_manager] 4 byte image";
        if (surface->format->Rmask == 0x000000ff) {
            texture_format = GL_RGBA;
            texture_type = GL_UNSIGNED_INT_8_8_8_8_REV;
        } else {
            texture_format = GL_BGRA;
            texture_type = GL_UNSIGNED_INT_8_8_8_8;
            LOG(INFO) << "[texture_manager] GL_BGRA";
        }
        internal_format = GL_RGBA8;
    } else if(surface->format->BytesPerPixel == 3) {
        LOG(INFO) << "[texture_manager] 3 byte image ";
        if (surface->format->Rmask == 0x000000ff) {
            texture_format = GL_RGB;
            texture_type = GL_UNSIGNED_BYTE;
        } else {
            texture_format = GL_BGR;
            texture_type = GL_UNSIGNED_BYTE;
            LOG(INFO) << "[texture_manager] GL_BGR";
        }
        internal_format = GL_RGB8;
    } else {
        LOG(FATAL) << "[texture_manager] image " << image << " unknown BPP " << (int)surface->format->BytesPerPixel;
    }

    LOG(INFO) << "[texture_manager] size " << surface->w << "x" << surface->h;

    int alignment = 8;
    LOG(INFO) << "[texture_manager] surface->pitch " << surface->pitch;
    while (surface->pitch % alignment) { // x%1==0 for any x
        alignment >>= 1;
    }
    LOG(INFO) << "[texture_manager] alignment " << alignment;
    glPixelStorei(GL_UNPACK_ALIGNMENT, alignment);

    GLuint texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glTexImage2D(GL_TEXTURE_2D, 0, internal_format, surface->w, surface->h, 0, texture_format, texture_type, surface->pixels);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, 0);

    SDL_FreeSurface(surface);

    texture tex = texture(texture_id, 1, surface->w, surface->h);
    auto succeeded = texture_cache.insert(make_pair(image, tex)).second;

    if(!succeeded) {
        LOG(FATAL) << "[texture_manager] Couldn't insert texture into cache " << image;
    }

    LOG(INFO) << "[texture_manager] created texture " << texture_id;

    return tex;
}

GLuint create_shader_program(string const & vertex_shader, string const & fragment_shader) {
    GLuint program_id = glCreateProgram();

    auto vertexShaderMaybe = load_shader_from_file(vertex_shader, GL_VERTEX_SHADER);
    if(!vertexShaderMaybe) {
        LOG(FATAL) << "[texture_manager] Couldn't load vertex shader file " << vertex_shader;
    }

    glAttachShader(program_id, vertexShaderMaybe.value());

    auto fragmentShaderMaybe = load_shader_from_file(fragment_shader, GL_FRAGMENT_SHADER);
    if(!fragmentShaderMaybe) {
        LOG(FATAL) << "[texture_manager] Couldn't load fragment shader file " << fragment_shader;
    }

    glAttachShader(program_id, fragmentShaderMaybe.value());

    glLinkProgram(program_id);

    glDeleteShader(vertexShaderMaybe.value());
    glDeleteShader(fragmentShaderMaybe.value());

    GLint programSucces = GL_TRUE;
    glGetProgramiv(program_id, GL_LINK_STATUS, &programSucces);
    if(programSucces != GL_TRUE) {
        LOG(FATAL) << "[texture_manager] Couldn't link program " << program_id;
        print_program_log(program_id);
    }

    LOG(INFO) << "[texture_manager] Created shader program " << program_id;

    return program_id;
}

void delete_texture(std::string const & image) noexcept {
    auto found_texture = texture_cache.find(image);

    if(found_texture != texture_cache.end()) {
        found_texture->second._reference_count--;

        if(found_texture->second._reference_count == 0) {
            LOG(INFO) << "[texture_manager] deleting texture " << found_texture->second._texture_id;
            glDeleteTextures(1, &found_texture->second._texture_id);
            texture_cache.erase(image);
        }
    } else {
        LOG(FATAL) << "[texture_manager] couldn't delete texture " << image;
    }
}
