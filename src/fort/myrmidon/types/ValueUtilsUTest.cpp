#include <gtest/gtest.h>

#include "ValueUtils.hpp"
#include <fort/myrmidon/UtilsUTest.hpp>


namespace fort {
namespace myrmidon {
class ValueUtilsUTest : public ::testing::Test {};


TEST_F(ValueUtilsUTest,Type) {
	EXPECT_EQ(ValueUtils::Type(true),ValueType::BOOL);
	EXPECT_EQ(ValueUtils::Type(0),ValueType::INT);
	EXPECT_EQ(ValueUtils::Type(0.0),ValueType::DOUBLE);
	EXPECT_EQ(ValueUtils::Type(std::string()),ValueType::STRING);
	EXPECT_EQ(ValueUtils::Type(Time()),ValueType::TIME);
}


TEST_F(ValueUtilsUTest,Parsing) {

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


TEST_F(ValueUtilsUTest,HasDefault) {
	EXPECT_EQ(ValueUtils::Default(ValueType::BOOL),Value(false));
	EXPECT_EQ(ValueUtils::Default(ValueType::INT),Value(0));
	EXPECT_EQ(ValueUtils::Default(ValueType::DOUBLE),Value(0.0));
	EXPECT_EQ(ValueUtils::Default(ValueType::STRING),Value(std::string()));
	EXPECT_EQ(ValueUtils::Default(ValueType::TIME),Value(Time()));
	EXPECT_THROW(ValueUtils::Default(ValueType(int(ValueType::TIME)+1)),std::invalid_argument);
}

TEST_F(ValueUtilsUTest,HasTypeName) {
	EXPECT_EQ(ValueUtils::TypeName(ValueType::BOOL),"Bool");
	EXPECT_EQ(ValueUtils::TypeName(ValueType::INT),"Int");
	EXPECT_EQ(ValueUtils::TypeName(ValueType::DOUBLE),"Double");
	EXPECT_EQ(ValueUtils::TypeName(ValueType::STRING),"String");
	EXPECT_EQ(ValueUtils::TypeName(ValueType::TIME),"Time");
	EXPECT_THROW(ValueUtils::TypeName(ValueType(42)),std::invalid_argument);
	EXPECT_EQ(ValueUtils::TypeName(true),"Bool");
	EXPECT_EQ(ValueUtils::TypeName(0),"Int");
	EXPECT_EQ(ValueUtils::TypeName(0.0),"Double");
	EXPECT_EQ(ValueUtils::TypeName(std::string()),"String");
	EXPECT_EQ(ValueUtils::TypeName(Time()),"Time");
}

TEST_F(ValueUtilsUTest,BuildValuedTimeRange) {
	struct TestData {
		std::map<Time,Value>            Values;
		ValueUtils::ValuedTimeRangeList Expected;
		std::string FormatInput() const {
			std::ostringstream oss;
			oss << "{" << std::endl;
			for ( const auto & [t,v] : Values ) {
				oss << "  {" << t << ":" << v << "}" << std::endl;
			}
			oss << "}" << std::endl;
			return oss.str();
		}
		void Expect(const ValueUtils::ValuedTimeRangeList & res) const {
			EXPECT_EQ(res.size(),Expected.size());
			for ( size_t i = 0; i < std::min(res.size(),Expected.size()); ++i) {
				SCOPED_TRACE(i);
				EXPECT_VALUE_EQ(res[i].Value,Expected[i].Value);
				EXPECT_TIME_EQ(res[i].Start,Expected[i].Start);
				EXPECT_TIME_EQ(res[i].End,Expected[i].End);
			}
		}
	};

	std::vector<TestData> testdata
		= {
		   {
		    .Values = {},
		    .Expected = {},
		   },
		   {
		    .Values = {
		               {fort::Time::SinceEver(),0},
			   },
		    .Expected = {
		                 {
		                  .Value = 0,
		                  .Start = fort::Time::SinceEver(),
		                  .End = fort::Time::Forever(),
		                 },
			   },
		   },

		   {
		    .Values = {
		               {fort::Time::SinceEver(),0},
		               {fort::Time(),1},
		               {fort::Time().Add(1),1},
		               {fort::Time().Add(2),2},
		               {fort::Time().Add(3),2},
			   },
		    .Expected = {
		                 {
		                  .Value = 0,
		                  .Start = fort::Time::SinceEver(),
		                  .End = fort::Time(),
		                 },
		                 {
		                  .Value = 1,
		                  .Start = fort::Time(),
		                  .End = fort::Time().Add(2),
		                 },
		                 {
		                  .Value = 2,
		                  .Start = fort::Time().Add(2),
		                  .End = fort::Time::SinceEver(),
		                 },
			   },
		   },
	};

	for ( const auto & d : testdata ) {
		SCOPED_TRACE(d.FormatInput());
		d.Expect(ValueUtils::BuildRanges(d.Values));
	}


}

} // namespace myrmidon
} // namespace fort
