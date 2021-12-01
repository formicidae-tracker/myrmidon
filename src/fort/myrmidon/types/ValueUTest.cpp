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


} // namespace myrmidon
} // namespace fort
