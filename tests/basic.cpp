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

#include<thumbnailer.h>
#include<testsetup.h>
#include<gtest/gtest.h>
#include<unistd.h>
#include<gdk-pixbuf/gdk-pixbuf.h>

#define TESTIMAGE TESTDATADIR "/testimage.jpg"
#define TESTIMAGE_NOEXIF TESTDATADIR "/testimage_noexif.png"
#define ROTTESTIMAGE TESTDATADIR "/testrotate.jpg"
#define TESTVIDEO TESTDATADIR "/testvideo.ogg"
#define CHINESETEXTDIR TESTDATADIR "/chinese_text"

using namespace std;

bool file_exists(const string &s) {
    FILE *f = fopen(s.c_str(), "r");
    if(f) {
        fclose(f);
        return true;
    }
    return false;
}

void copy_file(const string &src, const string &dst) {
    FILE* f = fopen(src.c_str(), "r");
    ASSERT_TRUE(f);
    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);

    char* buf = new char[size];

    fseek(f, 0, SEEK_SET);
    ASSERT_EQ(fread(buf, 1, size, f), size);
    fclose(f);

    f = fopen(dst.c_str(), "w");
    ASSERT_TRUE(f);
    ASSERT_EQ(fwrite(buf, 1, size, f), size);
    fclose(f);
    delete[] buf;
}

TEST(Thumbnailer, trivial) {
    Thumbnailer tn;
}

static void file_test(Thumbnailer &tn, string &ifile) {
    int w, h;
    ASSERT_TRUE(file_exists(ifile));
    string thumbfile = tn.get_thumbnail(ifile, TN_SIZE_SMALL, TN_LOCAL);
    unlink(thumbfile.c_str());
    ASSERT_FALSE(file_exists(thumbfile));
    string thumbfile2 = tn.get_thumbnail(ifile, TN_SIZE_SMALL, TN_LOCAL);
    ASSERT_EQ(thumbfile, thumbfile2);
    ASSERT_TRUE(file_exists(thumbfile));
    ASSERT_TRUE(gdk_pixbuf_get_file_info(thumbfile.c_str(), &w, &h));
    ASSERT_LE(w, 128);
    ASSERT_LE(h, 128);
}

TEST(Thumbnailer, image) {
    Thumbnailer tn;
    string imfile(TESTIMAGE);
    file_test(tn, imfile);
}

TEST(Thumbnailer, noexif_image) {
    Thumbnailer tn;
    string imfile(TESTIMAGE_NOEXIF);
    file_test(tn, imfile);
}

TEST(Thumbnailer, video) {
    Thumbnailer tn;
    string videofile(TESTVIDEO);
    file_test(tn, videofile);
}

TEST(Thumbnailer, rotate) {
    Thumbnailer tn;
    string imfile(ROTTESTIMAGE);
    int w, h;
    ASSERT_TRUE(file_exists(imfile));
    string thumbfile = tn.get_thumbnail(imfile, TN_SIZE_LARGE);
    unlink(thumbfile.c_str());
    ASSERT_FALSE(file_exists(thumbfile));
    string thumbfile2 = tn.get_thumbnail(imfile, TN_SIZE_LARGE);
    ASSERT_EQ(thumbfile, thumbfile2);
    ASSERT_TRUE(file_exists(thumbfile));
    ASSERT_TRUE(gdk_pixbuf_get_file_info(imfile.c_str(), &w, &h));
    ASSERT_GT(w, h); // gdk_pixbuf does not reorient images automatically.
    ASSERT_TRUE(gdk_pixbuf_get_file_info(thumbfile.c_str(), &w, &h));
    ASSERT_GT(h, w); // Has the orientation been straightened during scaling?
}


TEST(Thumbnailer, video_original) {
    Thumbnailer tn;
    int w, h;
    string videofile(TESTVIDEO);
    string origsize = tn.get_thumbnail(videofile, TN_SIZE_ORIGINAL);
    ASSERT_TRUE(file_exists(origsize));
    ASSERT_TRUE(gdk_pixbuf_get_file_info(origsize.c_str(), &w, &h));
    ASSERT_EQ(w, 1920);
    ASSERT_EQ(h, 1080);
}


TEST(Thumbnailer, size) {
    Thumbnailer tn;
    int w, h;
    string imfile(TESTIMAGE);
    string thumbfile = tn.get_thumbnail(imfile, TN_SIZE_SMALL);
    string thumbfile2 = tn.get_thumbnail(imfile, TN_SIZE_LARGE);
    string thumbfile3 = tn.get_thumbnail(imfile, TN_SIZE_XLARGE);
    ASSERT_FALSE(thumbfile.empty());
    ASSERT_FALSE(thumbfile2.empty());
    ASSERT_FALSE(thumbfile3.empty());
    ASSERT_NE(thumbfile, thumbfile2);
    ASSERT_NE(thumbfile, thumbfile3);
    ASSERT_NE(thumbfile2, thumbfile3);
    ASSERT_TRUE(gdk_pixbuf_get_file_info(thumbfile.c_str(), &w, &h));
    ASSERT_EQ(w, 128);
    ASSERT_LE(h, 128);
    ASSERT_TRUE(gdk_pixbuf_get_file_info(thumbfile2.c_str(), &w, &h));
    ASSERT_EQ(w, 256);
    ASSERT_LE(h, 256);
    ASSERT_TRUE(gdk_pixbuf_get_file_info(thumbfile3.c_str(), &w, &h));
    ASSERT_EQ(w, 512);
    ASSERT_LE(h, 512);
}

TEST(Thumbnailer, deletetest) {
    Thumbnailer tn;
    string srcimg(TESTIMAGE);
    string workimage("working_image.jpg");
    copy_file(srcimg, workimage);
    ASSERT_TRUE(file_exists(workimage));
    string thumbfile = tn.get_thumbnail(workimage, TN_SIZE_SMALL);
    string thumbfile2 = tn.get_thumbnail(workimage, TN_SIZE_LARGE);
    string thumbfile3 = tn.get_thumbnail(workimage, TN_SIZE_XLARGE);
    ASSERT_TRUE(file_exists(thumbfile));
    ASSERT_TRUE(file_exists(thumbfile2));
    ASSERT_TRUE(file_exists(thumbfile3));
    unlink(workimage.c_str());
    string tmp = tn.get_thumbnail(workimage, TN_SIZE_SMALL);
    ASSERT_TRUE(tmp.empty());
    ASSERT_FALSE(file_exists(thumbfile));
    ASSERT_FALSE(file_exists(thumbfile2));
    ASSERT_FALSE(file_exists(thumbfile3));
}

static void needs_generation_before_after_test(Thumbnailer &tn, string &imfile, ThumbnailSize size, bool always_needed = false) {
    bool needs_generation;
    string thumbfile = tn.get_thumbnail(imfile, size);
    needs_generation = tn.thumbnail_needs_generation(imfile, size);
    ASSERT_EQ(needs_generation, always_needed);
    unlink(thumbfile.c_str());
    needs_generation = tn.thumbnail_needs_generation(imfile, size);
    ASSERT_TRUE(needs_generation);
}

TEST(Thumbnailer, needsgeneration_size) {
    Thumbnailer tn;
    string srcimg(TESTIMAGE);
    string imfile("working_image.jpg");
    copy_file(srcimg, imfile);

    needs_generation_before_after_test(tn, imfile, TN_SIZE_SMALL);
    needs_generation_before_after_test(tn, imfile, TN_SIZE_LARGE);
    needs_generation_before_after_test(tn, imfile, TN_SIZE_XLARGE);
    /* when size is TN_SIZE_ORIGINAL generation will always happen */
    needs_generation_before_after_test(tn, imfile, TN_SIZE_ORIGINAL, true);
}

TEST(Thumbnailer, no_image_cache) {
    Thumbnailer tn;
    string srcimg(TESTIMAGE);
    string dstimg = tn.get_thumbnail(srcimg, TN_SIZE_ORIGINAL);
    ASSERT_EQ(srcimg, dstimg);
}

TEST(Thumbnailer, album_and_artist_art) {
    // During test suite run we may not have access to the net.
    // So just test calling to ensure symbol visibility etc work.
    Thumbnailer tn;
    string artist("The Prodigy");
    string album("Music for the Jilted Generation");
    tn.get_album_art(artist, album, TN_SIZE_ORIGINAL, TN_LOCAL);
    tn.get_artist_art(artist, album, TN_SIZE_ORIGINAL, TN_LOCAL);
}

TEST(Thumbnailer, chinese_text) {
    Thumbnailer tn;
    DIR *dir;
    struct dirent *ent;
    char path[PATH_MAX];
    struct stat buffer;
    int status;

    dir = opendir(CHINESETEXTDIR);
    while ((ent = readdir(dir)) != NULL) {
        sprintf(path, "%s/%s", CHINESETEXTDIR, ent->d_name);
        status = lstat(path, &buffer);
        ASSERT_EQ(status, 0);
        if(!S_ISDIR(buffer.st_mode)) {
            string srcimg(path);
            string thumbfile = tn.get_thumbnail(srcimg, TN_SIZE_SMALL);
            ASSERT_TRUE(file_exists(thumbfile));
        }
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
