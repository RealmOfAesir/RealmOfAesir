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

#include "tiled_converter.h"
#include <cereal/external/rapidjson/rapidjson.h>
#include <cereal/external/rapidjson/stringbuffer.h>
#include <cereal/external/rapidjson/writer.h>
#include <base64.h>
#include <zlib.h>
#include <easylogging++.h>
#include <macros.h>
#include <lz4.h>

using namespace std;
using namespace roa;
using namespace rapidjson;

#if __GNUC__ >= 6
constexpr auto& constexpr_strlen = std::char_traits<char>::length;
#else
int constexpr constexpr_strlen(const char* str)
{
    return *str ? 1 + constexpr_strlen(str + 1) : 0;
}
#endif

string tiled_converter::convert_map_to_json(EntityManager const &es, map_component const &mc) {
    StringBuffer sb;
    Writer<StringBuffer> writer(sb);

    writer.StartObject();

    writer.String("height", static_cast<SizeType>(constexpr_strlen("height")));
    writer.Uint(mc.height);
    writer.String("width", static_cast<SizeType>(constexpr_strlen("width")));
    writer.Uint(mc.width);
    writer.String("tileheight", static_cast<SizeType>(constexpr_strlen("tileheight")));
    writer.Uint(mc.tile_height);
    writer.String("tilewidth", static_cast<SizeType>(constexpr_strlen("tilewidth")));
    writer.Uint(mc.tile_width);
    writer.String("nextobjectid", static_cast<SizeType>(constexpr_strlen("nextobjectid")));
    writer.Uint(1);
    writer.String("orientation", static_cast<SizeType>(constexpr_strlen("orientation")));
    writer.String("orthogonal", static_cast<SizeType>(constexpr_strlen("orthogonal")));
    writer.String("renderorder", static_cast<SizeType>(constexpr_strlen("renderorder")));
    writer.String("right-down", static_cast<SizeType>(constexpr_strlen("right-down")));
    writer.String("version", static_cast<SizeType>(constexpr_strlen("version")));
    writer.Uint(1);

    writer.String("tilesets", static_cast<SizeType>(constexpr_strlen("tilesets")));
    writer.StartArray();

    for(auto& tileset : mc.tilesets) {
        writer.StartObject();
        writer.String("columns", static_cast<SizeType>(constexpr_strlen("columns")));
        writer.Uint(0);
        writer.String("firstgid", static_cast<SizeType>(constexpr_strlen("firstgid")));
        writer.Uint(tileset.first_gid);
        writer.String("image", static_cast<SizeType>(constexpr_strlen("image")));
        writer.String(tileset.image_path.c_str(), static_cast<SizeType>(tileset.image_path.length()));
        writer.String("imagewidth", static_cast<SizeType>(constexpr_strlen("imagewidth")));
        writer.Uint(tileset.width);
        writer.String("imageheight", static_cast<SizeType>(constexpr_strlen("imageheight")));
        writer.Uint(tileset.height);
        writer.String("margin", static_cast<SizeType>(constexpr_strlen("margin")));
        writer.Uint(0);
        writer.String("name", static_cast<SizeType>(constexpr_strlen("name")));
        writer.String(tileset.image_path.c_str(), static_cast<SizeType>(tileset.image_path.length()));
        writer.String("spacing", static_cast<SizeType>(constexpr_strlen("spacing")));
        writer.Uint(0);
        writer.String("tilecount", static_cast<SizeType>(constexpr_strlen("tilecount")));
        writer.Uint((tileset.height / tileset.tile_height) * (tileset.width / tileset.tile_width));
        writer.String("tilewidth", static_cast<SizeType>(constexpr_strlen("tilewidth")));
        writer.Uint(tileset.tile_width);
        writer.String("tileheight", static_cast<SizeType>(constexpr_strlen("tileheight")));
        writer.Uint(tileset.tile_height);
        writer.EndObject();
    }

    writer.EndArray();

    writer.String("layers", static_cast<SizeType>(constexpr_strlen("layers")));
    writer.StartArray();

    for(auto& layer : mc.layers) {
        int tiles_size = layer.tiles.size() * 4;
        int destSize = LZ4_compressBound(tiles_size);
        char* tiles_compressed = new char[destSize];
        char* tiles = new char[tiles_size];
        uint32_t *tiles32 = reinterpret_cast<uint32_t*>(tiles);


        for(uint32_t i = 0; i < layer.tiles.size(); i++) {
            tiles32[i] = es.get<tile_component>(layer.tiles[i]).tile_id;
        }

        auto ret = LZ4_compress_default(tiles, tiles_compressed, tiles_size, destSize);
        if(ret == 0) {
            LOG(ERROR) << NAMEOF(convert_map_to_json) << " LZ4_compress_default error";
            delete[] tiles;
            delete[] tiles_compressed;
            throw std::runtime_error("Couldn't compress");
        }

        string base64_data = base64_encode(reinterpret_cast<unsigned char*>(tiles_compressed), ret);


        delete[] tiles;
        delete[] tiles_compressed;

        writer.StartObject();
        writer.String("compression", static_cast<SizeType>(constexpr_strlen("compression")));
        writer.String("lz4", static_cast<SizeType>(constexpr_strlen("lz4")));
        writer.String("data", static_cast<SizeType>(constexpr_strlen("data")));
        writer.String(base64_data.c_str(), static_cast<SizeType>(base64_data.length()));

        writer.String("x", static_cast<SizeType>(constexpr_strlen("x")));
        writer.Uint(layer.x);
        writer.String("y", static_cast<SizeType>(constexpr_strlen("y")));
        writer.Uint(layer.y);
        writer.String("width", static_cast<SizeType>(constexpr_strlen("width")));
        writer.Uint(layer.width);
        writer.String("height", static_cast<SizeType>(constexpr_strlen("height")));
        writer.Uint(layer.height);
        writer.String("name", static_cast<SizeType>(constexpr_strlen("name")));
        writer.String("name", static_cast<SizeType>(constexpr_strlen("name")));
        writer.String("type", static_cast<SizeType>(constexpr_strlen("type")));
        writer.String("tilelayer", static_cast<SizeType>(constexpr_strlen("tilelayer")));
        writer.String("visible", static_cast<SizeType>(constexpr_strlen("visible")));
        writer.String("true", static_cast<SizeType>(constexpr_strlen("true")));
        writer.String("opacity", static_cast<SizeType>(constexpr_strlen("opacity")));
        writer.Uint(1);
        writer.String("encoding", static_cast<SizeType>(constexpr_strlen("encoding")));
        writer.String("base64", static_cast<SizeType>(constexpr_strlen("base64")));
        writer.EndObject();
    }

    writer.EndArray();

    writer.EndObject();

    return sb.GetString();
}
