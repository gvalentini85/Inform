// Copyright 2016 ELIFE. All rights reserved.
// Use of this source code is governed by a MIT
// license that can be found in the LICENSE file.
#include <unit.h>

UNIT(OnePlusTwo)
{
    ASSERT_EQUAL(3, 1+2);
}

BEGIN_SUITE(Canary)
    ADD_UNIT(OnePlusTwo)
END_SUITE
