#include <gtest/gtest.h>

#include "Color.hpp"

namespace fort {
namespace myrmidon {

class ColorUTest : public ::testing::Test {};

TEST_F(ColorUTest,HaveADefaultPalette) {

	for(size_t i = 0 ;
	    i < DefaultPalette().size() * 2;
	    ++i ) {
		EXPECT_EQ(DefaultPaletteColor(i),DefaultPalette()[i%DefaultPalette().size()]);
	}

}

TEST_F(ColorUTest,FormatsColor) {
	struct TestData {
		Color       C;
		std::string E;
	};
	std::vector<TestData> testdata
		= {
		   {DefaultPaletteColor(0),"#e69f00"},
		   {DefaultPaletteColor(1),"#56b4e9"},
		   {DefaultPaletteColor(2),"#009e73"},
		   {DefaultPaletteColor(3),"#f0e442"},
		   {DefaultPaletteColor(4),"#0072b2"},
		   {DefaultPaletteColor(5),"#d55e00"},
		   {DefaultPaletteColor(6),"#cc79a7"},
		   {DefaultPaletteColor(7),"#e69f00"},
	};
	size_t i = 0;
	for ( const auto & d : testdata ) {
		std::ostringstream oss;
		oss << d.C;
		EXPECT_EQ(oss.str(),d.E) << "  With i :" << i;
		++i;
	}
}


} // namespace myrmidon
} // namespace fort
