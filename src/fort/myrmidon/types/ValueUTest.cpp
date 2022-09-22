#include <gtest/gtest.h>

#include "Value.hpp"
#include <fort/myrmidon/UtilsUTest.hpp>

namespace fort {
namespace myrmidon {
class ValueUTest : public ::testing::Test {};

TEST_F(ValueUTest,HasEqualityOperator) {
	std::vector<std::pair<Value,Value>> testdata
		= {
		   {true,false},
		   {0,1},
		   {0.0,1.0},
		   {std::string("a"),std::string("b")},
		   {Time(),Time().Add(1)},
	};

	for ( size_t i = 0 ; i < testdata.size() ; ++i ) {
		std::ostringstream  oss;
		oss << "testing {" << testdata[i].first << "," << testdata[i].second << "}";
		SCOPED_TRACE(oss.str());
		EXPECT_TRUE(testdata[i].first == testdata[i].first);
		EXPECT_TRUE(testdata[i].second == testdata[i].second);
		EXPECT_FALSE(testdata[i].first == testdata[i].second);
		for ( size_t j = i+1; j < testdata.size(); ++j) {
			std::ostringstream  oss;
			oss << "against {" << testdata[j].first << "," << testdata[j].second << "}";
			SCOPED_TRACE(oss.str());
			EXPECT_FALSE(testdata[i].first == testdata[j].first);
			EXPECT_FALSE(testdata[i].first == testdata[j].second);
			EXPECT_FALSE(testdata[i].second == testdata[j].first);
			EXPECT_FALSE(testdata[i].second == testdata[j].second);
		}
	}


	EXPECT_TRUE(Value(true) == Value(true));
	EXPECT_FALSE(Value(false) == Value(true));
	EXPECT_FALSE(Value(false) == Value(0));
}

TEST_F(ValueUTest,CanFormat) {
	std::vector<std::pair<Value,std::string>> testdata
		= {
		{ true,"true"},
		{ false, "false"},
		{ 0,"0"},
		{ 0.2,"0.2"},
		{ std::string("a"),"a"},
		{Time(),"1970-01-01T00:00:00Z"},
	};

	for ( const auto & [ value,expected ] : testdata) {
		std::ostringstream oss;
		oss << value;
		EXPECT_EQ(oss.str(),expected);
	}
}



} // namespace myrmidon
} // namespace fort
