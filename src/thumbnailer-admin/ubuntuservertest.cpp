/*
 * Copyright (C) 2014 Canonical Ltd.
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
 * Authored by: Pawel Stolowski <pawel.stolowski@canonical.com>
 */

#include<internal/ubuntuserverdownloader.h>

using namespace std;

int main(int /*argc*/, char **/*argv*/) {
    UbuntuServerDownloader dl;
    dl.download("Radiohead", "Amnesiac", "image.jpg");
    dl.download_artist("Radiohead", "Amnesiac", "image2.jpg");
    return 0;
}
