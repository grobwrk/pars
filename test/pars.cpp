#include <gtest/gtest.h>

#include "pars/pars.h"

TEST(pars, included)
{
  EXPECT_TRUE(pars_included_v);
}
