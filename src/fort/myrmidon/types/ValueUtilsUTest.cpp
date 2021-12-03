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
		                  .End = fort::Time::Forever(),
		                 },
			   },
		   },
	};

	for ( const auto & d : testdata ) {
		SCOPED_TRACE(d.FormatInput());
		d.Expect(ValueUtils::BuildRanges(d.Values));
	}


}


TEST_F(ValueUtilsUTest,FindConflicts) {
	std::map<Time,Value> values
		= {
		   {fort::Time::SinceEver(),true},
		   {fort::Time(),false},
		   {fort::Time().Add(10),true},
		   {fort::Time().Add(20),true},
		   {fort::Time().Add(30),false},
	};

	struct TestData {
		ValueUtils::ValuedTimeRange     Range;
		ValueUtils::ValuedTimeRangeList Expected;

		std::string FormatInput() const {
			std::ostringstream oss;
			oss << std::endl
			    << "{ Value: " << Range.Value
			    << ", Start:" << Range.Start
			    << ", End: " <<  Range.End
			    << "}";
			return oss.str();
		}

		void Expect(const ValueUtils::ValuedTimeRangeList & result) const {
			SCOPED_TRACE(FormatInput());
			EXPECT_EQ(result.size(),Expected.size());
			for ( size_t i = 0; i < std::min(result.size(),Expected.size()); ++i) {
				SCOPED_TRACE(i);
				EXPECT_VALUE_EQ(result[i].Value,Expected[i].Value);
				EXPECT_TIME_EQ(result[i].Start,Expected[i].Start);
				EXPECT_TIME_EQ(result[i].End,Expected[i].End);
			}
		}
	};

	std::vector<TestData> testdata
		= {
		   {
		    .Range = { .Value = true, .Start = Time(), .End = Time() },
		    .Expected = {},
		   },
		   {
		    .Range = { .Value = false, .Start = Time(), .End = Time().Add(1) },
		    .Expected = {},
		   },
		   {
		    .Range = { .Value = true, .Start = Time(), .End = Time().Add(1) },
		    .Expected = {
		                 { .Value = false, .Start = Time(), .End = Time().Add(10) },
			   },
		   },
		   {
		    .Range = { .Value = false,  .Start = Time().Add(5), .End = Time().Add(25) },
		    .Expected = {
			   },
		   },
		   {
		    .Range = { .Value = true,  .Start = Time().Add(5), .End = Time().Add(30) },
		    .Expected = {
		                 { .Value = false, .Start = Time(), .End = Time().Add(10) },
			   },
		   },
		   {
		    .Range = { .Value = false, .Start = Time::SinceEver(), .End = Time::Forever() },
		    .Expected = {
			   }
		   },
		   {
		    .Range = { .Value = true, .Start = Time::SinceEver(), .End = Time::Forever() },
		    .Expected = {
		                 { .Value = false, .Start = Time(), .End = Time().Add(10) },
		                 { .Value = false, .Start = Time().Add(30), .End = Time::Forever() },
			   }
		   },

	};

	for ( const auto & d : testdata ) {
		d.Expect(ValueUtils::FindConflicts(values,true,d.Range));
	}
}

struct MergeData {
	std::map<Time,Value>        Values;
	ValueUtils::ValuedTimeRange Range;
	ValueUtils::Operations      Expected;

	std::string FormatData() const {
		std::ostringstream oss;
		oss << std::endl
		    << "Values:{" << std::endl;
		for ( const auto & [time,value] : Values ) {
			oss << "  {" << time << ", " << value << "}," << std::endl;
		}
		oss << "}" << std::endl
		    << "Range:{ Value: " << Range.Value
		    << ", Start:" << Range.Start
		    << ", End: " <<  Range.End
		    << "}" << std::endl
		    << "Expected:{" << std::endl
		    << "  ToSet:{" << std::endl;
		for ( const auto & [time,value] : Expected.ToSet ) {
			oss << "    {" << time << ", " << value << "}," << std::endl;
		}
		oss << "  }" << std::endl
		    << "  ToDelete:{" << std::endl;
		for ( const auto & time : Expected.ToDelete ) {
			oss << "    " << time << "," << std::endl;
		}
		oss << "  }" << std::endl
		    << "}" << std::endl;
		return oss.str();
	}

	void Expect(const ValueUtils::Operations & operations) const {
		SCOPED_TRACE(FormatData());
		EXPECT_EQ(operations.ToSet.size(),Expected.ToSet.size());
		EXPECT_EQ(operations.ToDelete.size(),Expected.ToDelete.size());
		for ( size_t i = 0; i < std::min(operations.ToSet.size(),Expected.ToSet.size()); ++i) {
			EXPECT_TIME_EQ(std::get<0>(operations.ToSet[i]),
			               std::get<0>(Expected.ToSet[i]));
			EXPECT_VALUE_EQ(std::get<1>(operations.ToSet[i]),
			                std::get<1>(Expected.ToSet[i]));
		}
		for ( size_t i = 0; i < std::min(operations.ToDelete.size(),Expected.ToDelete.size()); ++i) {
			EXPECT_TIME_EQ(operations.ToDelete[i],
			               Expected.ToDelete[i]);
		}
	}
};


TEST_F(ValueUtilsUTest,OverwriteRanges) {
	std::vector<MergeData> testdata
		= {
		   {
		    .Values = {{Time::SinceEver(), true}},
		    .Range = { .Value = false, .Start = Time(), .End = Time() },
		    .Expected = { }
		   },
		   {
		    .Values = {{Time::SinceEver(), true}},
		    .Range = { .Value = true, .Start = Time(), .End = Time().Add(10) },
		    .Expected = {
			   }
		   },
		   {
		    .Values = {{Time::SinceEver(), true}},
		    .Range = { .Value = false, .Start = Time(), .End = Time().Add(10) },
		    .Expected = {
		                 .ToSet = { {Time(),false}, {Time().Add(10),true} },
		                 .ToDelete = { },
			   }
		   },
		   {
		    .Values = {{Time::SinceEver(), true}},
		    .Range = { .Value = false, .Start = Time(), .End = Time::Forever() },
		    .Expected = {
		                 .ToSet = { {Time(),false} },
		                 .ToDelete = { },
			   }
		   },
		   {
		    .Values = {{Time::SinceEver(), true},{Time().Add(10),false}},
		    .Range = { .Value = false, .Start = Time(), .End = Time().Add(10) },
		    .Expected = {
		                 .ToSet = { {Time(),false}},
		                 .ToDelete = { Time().Add(10) },
			   }
		   },
		   {
		    .Values = {
		               {Time::SinceEver(), true},
		               {Time().Add(10),false},
		               {Time().Add(20),true}},
		    .Range = { .Value = false, .Start = Time().Add(20), .End = Time().Add(30) },
		    .Expected = {
		                 .ToSet = { {Time().Add(30),true}},
		                 .ToDelete = { Time().Add(20) },
			   }
		   },
		   {
		    .Values = {
		               {Time::SinceEver(), true},
		               {Time().Add(10),false},
		               {Time().Add(20),true},
		               {Time().Add(30),false},
		               {Time().Add(40),true},
			   },
		    .Range = { .Value = false, .Start = Time().Add(10), .End = Time().Add(40) },
		    .Expected = {
		                 .ToSet = {{Time().Add(10),false}},
		                 .ToDelete = { fort::Time().Add(20),
		                               fort::Time().Add(30)},
			   }
		   },
		   {
		    .Values = {
		               {Time::SinceEver(), 0},
		               {Time().Add(10),2},
		               {Time().Add(20),0},
			   },
		    .Range = { .Value = 1, .Start = Time(), .End = Time().Add(10) },
		    .Expected = {
		                 .ToSet = {{Time(),1}},
		                 .ToDelete = { },
			   },
		   },
		   {
		    .Values = {
		               {Time::SinceEver(), 0},
		               {Time(), 1},
		               {Time().Add(10), 0},
			   },
		    .Range = { .Value = 2, .Start = Time::SinceEver(), .End = Time::Forever() },
		    .Expected = {
		                 .ToSet = {{Time::SinceEver(),2}},
		                 .ToDelete = {Time(),Time().Add(10)},
			   },
		   },

	};
	for ( const auto & d : testdata ) {
		d.Expect(ValueUtils::OverwriteRanges(d.Values,
		                                     d.Range));
	}
}

TEST_F(ValueUtilsUTest,MergeRanges) {
	std::vector<MergeData> testdata
		= {
		   {
		    .Values = {
		               {Time::SinceEver(), 0}
			   },
		    .Range = { .Value = 1, .Start = Time(), .End = Time() },
		    .Expected = {
		                 .ToSet = {},
		                 .ToDelete = {},
			   },
		   },
		   {
		    .Values = {
		               {Time::SinceEver(), 0},
			   },
		    .Range = { .Value = 1, .Start = Time(), .End = Time().Add(10) },
		    .Expected = {
		                 .ToSet = {{Time(),1},{Time().Add(10),0}},
		                 .ToDelete = {},
			   },
		   },
		   {
		    .Values = {
		               {Time::SinceEver(), 0},
			   },
		    .Range = { .Value = 1, .Start = Time(), .End = Time::Forever() },
		    .Expected = {
		                 .ToSet = {{Time(),1}},
		                 .ToDelete = {},
			   },
		   },
		   {
		    .Values = {
		               {Time::SinceEver(), 0},
		               {Time(), 1},
			   },
		    .Range = { .Value = 1, .Start = Time(), .End = Time().Add(10) },
		    .Expected = {
		                 .ToSet = {},
		                 .ToDelete = {},
			   },
		   },
		   {
		    .Values = {
		               {Time::SinceEver(), 0},
		               {Time(), 1},
		               {Time().Add(10), 0},
			   },
		    .Range = { .Value = 1, .Start = Time(), .End = Time().Add(30) },
		    .Expected = {
		                 .ToSet = {{Time().Add(30),0}},
		                 .ToDelete = {Time().Add(10)},
			   },
		   },
		   {
		    .Values = {
		               {Time::SinceEver(), 0},
		               {Time().Add(10),1},
		               {Time().Add(20),2},
		               {Time().Add(30),0},
		               {Time().Add(40),3},
		               {Time().Add(50),0},
			   },
		    .Range = { .Value = 1, .Start = Time(), .End = Time().Add(60) },
		    .Expected = {
		                 .ToSet = { {Time(),1},{Time().Add(30),1},{Time().Add(50),1},{Time().Add(60),0}},
		                 .ToDelete = { Time().Add(10) },
			   },
		   },
		   {
		    .Values = {
		               {Time::SinceEver(), 0},
		               {Time(), 1},
		               {Time().Add(10), 0},
			   },
		    .Range = { .Value = 2, .Start = Time::SinceEver(), .End = Time::Forever() },
		    .Expected = {
		                 .ToSet = {{Time::SinceEver(),2},{Time().Add(10),2}},
		                 .ToDelete = {},
			   },
		   },
	};

	for ( const auto & d : testdata ) {
		d.Expect(ValueUtils::MergeRanges(d.Values,
		                                 0,
		                                 d.Range));
	}
}

} // namespace myrmidon
} // namespace fort
