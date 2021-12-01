#include <gtest/gtest.h>

#include "Value.hpp"
#include <fort/myrmidon/UtilsUTest.hpp>

namespace fort {
namespace myrmidon {
class ValueUTest : public ::testing::Test {};



TEST_F(ValueUTest,Type) {
	EXPECT_EQ(ValueUtils::Type(true),ValueType::BOOL);
	EXPECT_EQ(ValueUtils::Type(0),ValueType::INT);
	EXPECT_EQ(ValueUtils::Type(0.0),ValueType::DOUBLE);
	EXPECT_EQ(ValueUtils::Type(std::string()),ValueType::STRING);
	EXPECT_EQ(ValueUtils::Type(Time()),ValueType::TIME);
}


TEST_F(ValueUTest,Parsing) {

	EXPECT_NO_THROW({
			EXPECT_TRUE(std::get<bool>(ValueUtils::Parse(ValueType::BOOL,"true")));
		});
	EXPECT_NO_THROW({
			EXPECT_FALSE(std::get<bool>(ValueUtils::Parse(ValueType::BOOL,"false")));
		});
	EXPECT_THROW({ValueUtils::Parse(ValueType::BOOL,"");},std::invalid_argument);

	EXPECT_NO_THROW({
			EXPECT_EQ(std::get<int>(ValueUtils::Parse(ValueType::INT,"-12345")),-12345);
		});
	EXPECT_THROW({ValueUtils::Parse(ValueType::INT,"foo");},std::invalid_argument);


	EXPECT_NO_THROW({
			EXPECT_DOUBLE_EQ(std::get<double>(ValueUtils::Parse(ValueType::DOUBLE,"0.69e-6")),0.69e-6);
		});
	EXPECT_THROW({ValueUtils::Parse(ValueType::DOUBLE,"foo");},std::invalid_argument);

	EXPECT_NO_THROW({
			EXPECT_EQ(std::get<std::string>(ValueUtils::Parse(ValueType::STRING,"foobar")),"foobar");
		});

	EXPECT_NO_THROW({
			auto dateStr = "2019-11-02T23:46:23.000Z";
			EXPECT_TIME_EQ(std::get<Time>(ValueUtils::Parse(ValueType::TIME,dateStr)),Time::Parse(dateStr));
		});
	EXPECT_THROW({ValueUtils::Parse(ValueType::DOUBLE,"foo");},std::invalid_argument);

	EXPECT_THROW(ValueUtils::Parse(ValueType(42),"foo"),std::invalid_argument);

}


TEST_F(ValueUTest,HasDefault) {
	EXPECT_EQ(ValueUtils::Default(ValueType::BOOL),Value(false));
	EXPECT_EQ(ValueUtils::Default(ValueType::INT),Value(0));
	EXPECT_EQ(ValueUtils::Default(ValueType::DOUBLE),Value(0.0));
	EXPECT_EQ(ValueUtils::Default(ValueType::STRING),Value(std::string()));
	EXPECT_EQ(ValueUtils::Default(ValueType::TIME),Value(Time()));
	EXPECT_THROW(ValueUtils::Default(ValueType(int(ValueType::TIME)+1)),std::invalid_argument);
}


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


} // namespace myrmidon
} // namespace fort
