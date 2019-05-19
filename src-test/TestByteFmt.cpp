/*
**  Copyright(C) 2019, StepToSky and FlightFactor
**  All rights reserved
**
**  Redistribution and use in source and binary forms, with or without
**  modification, are permitted provided that the following conditions are met:
**
**  1.Redistributions of source code must retain the above copyright notice, this
**    list of conditions and the following disclaimer.
**  2.Redistributions in binary form must reproduce the above copyright notice,
**    this list of conditions and the following disclaimer in the documentation
**    and / or other materials provided with the distribution.
**  3.The name of StepToSky or the name of FlightFactor or the names of its
**    contributors may NOT be used to endorse or promote products derived from
**    this software without specific prior written permission.
**
**  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
**  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
**  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
**  DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
**  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
**  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
**  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
**  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
**  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
**  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
**  Contacts: www.steptosky.com or www.flightfactor.aero
*/

#include "ph/stdafx.h"

#include <stsff/logging/utils/ByteFormat.h>
#include <gtest/gtest.h>

using namespace stsff::logging;

/**************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/

TEST(ByteFmt, formatBin) {
    ASSERT_STREQ("0 B", ByteFormat::bin(ByteFormat::Size(0)).c_str());
    ASSERT_STREQ("27 B", ByteFormat::bin(ByteFormat::Size(27)).c_str());
    ASSERT_STREQ("999 B", ByteFormat::bin(ByteFormat::Size(999)).c_str());
    ASSERT_STREQ("1000 B", ByteFormat::bin(ByteFormat::Size(1000)).c_str());
    ASSERT_STREQ("1023 B", ByteFormat::bin(ByteFormat::Size(1023)).c_str());
    ASSERT_STREQ("1.0 KiB", ByteFormat::bin(ByteFormat::Size(1024)).c_str());
    ASSERT_STREQ("1.7 KiB", ByteFormat::bin(ByteFormat::Size(1728)).c_str());
    ASSERT_STREQ("108.0 KiB", ByteFormat::bin(ByteFormat::Size(110592)).c_str());
    ASSERT_STREQ("6.8 MiB", ByteFormat::bin(ByteFormat::Size(7077888)).c_str());
    ASSERT_STREQ("432.0 MiB", ByteFormat::bin(ByteFormat::Size(452984832)).c_str());
    ASSERT_STREQ("27.0 GiB", ByteFormat::bin(ByteFormat::Size(28991029248)).c_str());
    ASSERT_STREQ("1.7 TiB", ByteFormat::bin(ByteFormat::Size(1855425871872)).c_str());
    ASSERT_STREQ("1.7 PiB", ByteFormat::bin(ByteFormat::Size(1899956092796928)).c_str());
    ASSERT_STREQ("8.0 EiB", ByteFormat::bin(ByteFormat::Size(9223372036854775807)).c_str());
}

TEST(ByteFmt, formatSi) {
    ASSERT_STREQ("0 B", ByteFormat::si(ByteFormat::Size(0)).c_str());
    ASSERT_STREQ("27 B", ByteFormat::si(ByteFormat::Size(27)).c_str());
    ASSERT_STREQ("999 B", ByteFormat::si(ByteFormat::Size(999)).c_str());
    ASSERT_STREQ("1.0 kB", ByteFormat::si(ByteFormat::Size(1000)).c_str());
    ASSERT_STREQ("1.0 kB", ByteFormat::si(ByteFormat::Size(1023)).c_str());
    ASSERT_STREQ("1.0 kB", ByteFormat::si(ByteFormat::Size(1024)).c_str());
    ASSERT_STREQ("1.7 kB", ByteFormat::si(ByteFormat::Size(1728)).c_str());
    ASSERT_STREQ("110.6 kB", ByteFormat::si(ByteFormat::Size(110592)).c_str());
    ASSERT_STREQ("7.1 MB", ByteFormat::si(ByteFormat::Size(7077888)).c_str());
    ASSERT_STREQ("453.0 MB", ByteFormat::si(ByteFormat::Size(452984832)).c_str());
    ASSERT_STREQ("29.0 GB", ByteFormat::si(ByteFormat::Size(28991029248)).c_str());
    ASSERT_STREQ("1.9 TB", ByteFormat::si(ByteFormat::Size(1855425871872)).c_str());
    ASSERT_STREQ("1.9 PB", ByteFormat::si(ByteFormat::Size(1855425871872000)).c_str());
    ASSERT_STREQ("9.2 EB", ByteFormat::si(ByteFormat::Size(9223372036854775807)).c_str());
}

/**************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/
