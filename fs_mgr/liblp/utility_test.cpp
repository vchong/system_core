/*
 * Copyright (C) 2018 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "utility.h"
#include <gtest/gtest.h>

using namespace android;
using namespace android::fs_mgr;

TEST(liblp, SlotNumberForSlotSuffix) {
    EXPECT_EQ(SlotNumberForSlotSuffix(""), 0);
    EXPECT_EQ(SlotNumberForSlotSuffix("_a"), 0);
    EXPECT_EQ(SlotNumberForSlotSuffix("_b"), 1);
    EXPECT_EQ(SlotNumberForSlotSuffix("_c"), 2);
    EXPECT_EQ(SlotNumberForSlotSuffix("_d"), 3);
}

TEST(liblp, GetMetadataOffset) {
    LpMetadataGeometry geometry = {
            LP_METADATA_GEOMETRY_MAGIC, sizeof(geometry), {0}, 16384, 4, 10000, 80000};
    EXPECT_EQ(GetPrimaryMetadataOffset(geometry, 0), 4096);
    EXPECT_EQ(GetPrimaryMetadataOffset(geometry, 1), 4096 + 16384);
    EXPECT_EQ(GetPrimaryMetadataOffset(geometry, 2), 4096 + 16384 * 2);
    EXPECT_EQ(GetPrimaryMetadataOffset(geometry, 3), 4096 + 16384 * 3);

    EXPECT_EQ(GetBackupMetadataOffset(geometry, 3), -4096 - 16384 * 1);
    EXPECT_EQ(GetBackupMetadataOffset(geometry, 2), -4096 - 16384 * 2);
    EXPECT_EQ(GetBackupMetadataOffset(geometry, 1), -4096 - 16384 * 3);
    EXPECT_EQ(GetBackupMetadataOffset(geometry, 0), -4096 - 16384 * 4);
}