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

#include<cstdio>
#include<string>
#include<memory>
#include<stdexcept>
#include<gst/gst.h>
#include<gio/gio.h>
#include<glib.h>

#include "thumbnailextractor.h"

using namespace std;

namespace {

bool extract_video(const std::string &uri, const std::string &ofname) {
    ThumbnailExtractor extractor;

    extractor.set_uri(uri);
    if (!extractor.extract_video_frame()) {
        return false;
    }
    extractor.save_screenshot(ofname);
    return true;
}

bool extract_audio(const std::string &uri, const std::string &ofname) {
    ThumbnailExtractor extractor;

    extractor.set_uri(uri);
    if (!extractor.extract_audio_cover_art()) {
        return false;
    }
    extractor.save_screenshot(ofname);
    return true;
}

}

int main(int argc, char **argv) {
    gst_init(&argc, &argv);
    if(argc != 3) {
        printf("%s <source file> <output file>\n", argv[0]);
        return 1;
    }
    string infile(argv[1]);
    string outfile(argv[2]);
    bool success = false;

    // Determine file type.
    std::unique_ptr<GFile, void(*)(void *)> file(
            g_file_new_for_commandline_arg(infile.c_str()), g_object_unref);
    if(!file) {
        return 1;
    }

    std::unique_ptr<char, void(*)(void *)> c_uri(
        g_file_get_uri(file.get()), g_free);
    std::string uri(c_uri.get());

    std::unique_ptr<GFileInfo, void(*)(void *)> info(
            g_file_query_info(file.get(), G_FILE_ATTRIBUTE_STANDARD_CONTENT_TYPE,
                G_FILE_QUERY_INFO_NONE, /* cancellable */ NULL, /* error */NULL),
            g_object_unref);
    if(!info) {
        return 1;
    }

    std::string content_type(g_file_info_get_content_type(info.get()));

    if (content_type.find("audio/") == 0) {
        try {
            success = extract_audio(uri, outfile);
        } catch(runtime_error &e) {
            printf("Error creating thumbnail: %s\n", e.what());
            return 2;
        }
    }

    if (content_type.find("video/") == 0) {
        try {
            success = extract_video(uri, outfile);
        } catch(runtime_error &e) {
            printf("Error creating thumbnail: %s\n", e.what());
            return 2;
        }
    }

    if(success) {
        return 0;
    }
    return 1;
}