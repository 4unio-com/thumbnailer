/*
 * Copyright (C) 2013 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by: Jussi Pakkanen <jussi.pakkanen@canonical.com>
 */

#pragma once

#include <memory>
#include <string>

#include <QSize>

class ThumbnailerPrivate;

/**
 * This class provides a way to generate and access
 * thumbnails of video, audio and image files.
 *
 * All methods are blocking.
 *
 * All methods are thread safe.
 *
 * Errors are reported as exceptions.
 */

class Thumbnailer
{
public:
    Thumbnailer();
    ~Thumbnailer();

    Thumbnailer(Thumbnailer const&) = delete;
    Thumbnailer& operator=(Thumbnailer const&) = delete;

    /**
     * Gets a thumbnail of the given input file in the requested size.
     *
     * Return value is the thumbnail image as a string.
     * If the thumbnail could not be generated, an empty string is returned.
     */
    std::string get_thumbnail(std::string const& filename, QSize const& requested_size);

    /**
     * Gets album art for the given artist an album.
     */
    std::string get_album_art(std::string const& artist, std::string const& album, QSize const& requested_size);

    /**
     * Gets artist art for the given artist and album.
     */
    std::string get_artist_art(std::string const& artist, std::string const& album, QSize const& requested_size);

private:
    std::unique_ptr<ThumbnailerPrivate> p_;
};
