#pragma once

#include <gtest/gtest.h>

#include "TagStatistics.hpp"

::testing::AssertionResult AssertTimedStatsEqual(const char * aExpr,
                                                 const char * bExpr,
                                                 const fort::myrmidon::priv::TagStatisticsHelper::Timed & a,
                                                 const fort::myrmidon::priv::TagStatisticsHelper::Timed & b);
