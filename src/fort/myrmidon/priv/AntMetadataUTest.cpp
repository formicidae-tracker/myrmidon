#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "AntMetadata.hpp"

#include <fort/myrmidon/UtilsUTest.hpp>
#include <fort/myrmidon/types/ValueUtils.hpp>
#include <fort/myrmidon/priv/DeletedReference.hpp>



namespace fort {
namespace myrmidon {
namespace priv {


class AntMetadataUTest : public ::testing::Test {
protected:
	void SetUp() {
		metadata =  std::make_shared<AntMetadata>();
	}
	void TearDown() {
		metadata.reset();
	}

	AntMetadata::Ptr metadata;
};

TEST_F(AntMetadataUTest,KeyHaveUniqueName) {
	AntMetadata::Key::Ptr foo,bar,baz;
	EXPECT_NO_THROW(foo = AntMetadata::SetKey(metadata,"foo",false););
	EXPECT_NO_THROW(bar = AntMetadata::SetKey(metadata,"bar",12););
	EXPECT_NO_THROW(baz = AntMetadata::SetKey(metadata,"baz",std::string()););

	ASSERT_EQ(baz->Type(), ValueType::STRING);

	EXPECT_THROW({
			foo->SetName("bar");
		},std::invalid_argument);

	foo->SetName("foobar");
	AntMetadata::SetKey(metadata,"foo",std::string(""));
}

TEST_F(AntMetadataUTest,ColumnAdditionDeletion) {
	AntMetadata::Key::Ptr foo,bar,baz;
	EXPECT_NO_THROW(foo = AntMetadata::SetKey(metadata,"foo",ValueUtils::Default(ValueType::BOOL)););
	EXPECT_NO_THROW(bar = AntMetadata::SetKey(metadata,"bar",ValueUtils::Default(ValueType::INT)););
	EXPECT_NO_THROW(baz = AntMetadata::SetKey(metadata,"baz",ValueUtils::Default(ValueType::STRING)););

	EXPECT_EQ(metadata->Keys().size(),3);

	EXPECT_EQ(metadata->Count("foo"), 1);
	EXPECT_EQ(metadata->Count("bar"), 1);
	EXPECT_EQ(metadata->Count("baz"), 1);
	EXPECT_EQ(metadata->Count("foobar"), 0);


	EXPECT_THROW({
			metadata->Delete("foobar");
		},std::out_of_range);

	EXPECT_NO_THROW({
			metadata->Delete("foo");
		});

	EXPECT_EQ(metadata->Count("foo"),0);
	EXPECT_EQ(metadata->Keys().size(),2);

	EXPECT_THROW({
			metadata->Delete("foo");
		},std::out_of_range);

	EXPECT_EQ(metadata->Keys().size(),2);
}



TEST_F(AntMetadataUTest,DataTypeStringValidation) {
	EXPECT_EQ(AntMetadata::Validate(ValueType::BOOL,"true"),AntMetadata::Validity::Valid);
	EXPECT_EQ(AntMetadata::Validate(ValueType::BOOL,"false"),AntMetadata::Validity::Valid);
	EXPECT_EQ(AntMetadata::Validate(ValueType::BOOL,"true2"),AntMetadata::Validity::Invalid);
	EXPECT_EQ(AntMetadata::Validate(ValueType::BOOL,"tru"),AntMetadata::Validity::Intermediate);


	EXPECT_EQ(AntMetadata::Validate(ValueType::INT,"123456"),AntMetadata::Validity::Valid);
	EXPECT_EQ(AntMetadata::Validate(ValueType::INT,"+123456"),AntMetadata::Validity::Valid);
	EXPECT_EQ(AntMetadata::Validate(ValueType::INT,"-123456"),AntMetadata::Validity::Valid);
	EXPECT_EQ(AntMetadata::Validate(ValueType::INT,"+"),AntMetadata::Validity::Intermediate);
	EXPECT_EQ(AntMetadata::Validate(ValueType::INT,"-"),AntMetadata::Validity::Intermediate);
	EXPECT_EQ(AntMetadata::Validate(ValueType::INT,"foo"),AntMetadata::Validity::Invalid);

	EXPECT_EQ(AntMetadata::Validate(ValueType::DOUBLE,"1.2345e-6"),AntMetadata::Validity::Valid);
	EXPECT_EQ(AntMetadata::Validate(ValueType::DOUBLE,"+123456"),AntMetadata::Validity::Valid);
	EXPECT_EQ(AntMetadata::Validate(ValueType::DOUBLE,"-1.234e-6"),AntMetadata::Validity::Valid);
	EXPECT_EQ(AntMetadata::Validate(ValueType::DOUBLE,"1.234e-"),AntMetadata::Validity::Intermediate);
	EXPECT_EQ(AntMetadata::Validate(ValueType::DOUBLE,"-"),AntMetadata::Validity::Intermediate);
	EXPECT_EQ(AntMetadata::Validate(ValueType::DOUBLE,"foo"),AntMetadata::Validity::Invalid);

	EXPECT_EQ(AntMetadata::Validate(ValueType::STRING,"sdbi wi \n fo"),AntMetadata::Validity::Valid);

	EXPECT_EQ(AntMetadata::Validate(ValueType::TIME,"2019-11-02T23:56:02.123456Z"),AntMetadata::Validity::Valid);

	EXPECT_EQ(AntMetadata::Validate(ValueType::TIME,"<any-string>"),AntMetadata::Validity::Intermediate);

	EXPECT_THROW(AntMetadata::Validate(ValueType(42), ""),std::invalid_argument);
}


TEST_F(AntMetadataUTest,ColumnPropertyCallbacks) {
	class MockAntMetadataCallback {
	public:
		MOCK_METHOD(void,OnNameChange,(const std::string &, const std::string),());
		MOCK_METHOD(void,OnTypeChange,(const std::string &, ValueType, ValueType),());
		MOCK_METHOD(void,OnDefaultChange,(const std::string &, const Value &, const Value&),());
	};

	MockAntMetadataCallback callbacks;
	metadata = std::make_shared<AntMetadata>([&callbacks](const std::string & oldName,
	                                                     const std::string & newName) {
		                                         callbacks.OnNameChange(oldName,newName);
	                                         },
	                                         [&callbacks](const std::string & name,
	                                                     ValueType oldType,
	                                                     ValueType newType) {
		                                         callbacks.OnTypeChange(name,oldType,newType);
	                                         },
	                                         [&callbacks](const std::string & name,
	                                                      const Value & oldDefault,
	                                                      const Value & newDefault) {
		                                         callbacks.OnDefaultChange(name,oldDefault,newDefault);
	                                         });


	auto column = AntMetadata::SetKey(metadata,
	                                  "foo",
	                                  false);
	EXPECT_CALL(callbacks,OnDefaultChange("foo",Value(false),Value(true))).Times(1);
	column->SetDefaultValue(true);


	EXPECT_CALL(callbacks,OnNameChange("foo","bar")).Times(1);
	EXPECT_CALL(callbacks,OnTypeChange("bar",ValueType::BOOL,ValueType::INT)).Times(1);
	EXPECT_CALL(callbacks,OnDefaultChange("bar",Value(true),Value(0))).Times(1);
	column->SetName("bar");
	ASSERT_EQ(column->Name(),"bar");
	column->SetDefaultValue(0);
	ASSERT_EQ(column->Type(),ValueType::INT);


	auto toDel = AntMetadata::SetKey(metadata,"foo",false);
	metadata->Delete("foo");


	auto & columns = const_cast<AntMetadata::KeysByName&>(metadata->Keys());
	columns.insert(std::make_pair("baz",column));
	columns.erase("bar");
	EXPECT_THROW(column->SetName("foobar"),std::logic_error);

	metadata.reset();

	EXPECT_THROW(column->SetName("baz"),DeletedReference<AntMetadata>);
	EXPECT_THROW(column->SetDefaultValue(false),DeletedReference<AntMetadata>);
	column.reset();

}

TEST_F(AntMetadataUTest,ColumnHaveDefaults) {
	AntMetadata::Key::Ptr boolCol,intCol,doubleCol,stringCol,timeCol;
	ASSERT_NO_THROW({
			boolCol = AntMetadata::SetKey(metadata,"bool",false);
			intCol = AntMetadata::SetKey(metadata,"int",0);
			doubleCol = AntMetadata::SetKey(metadata,"double",0.0);
			stringCol = AntMetadata::SetKey(metadata,"string",std::string());
			timeCol = AntMetadata::SetKey(metadata,"timeCol",fort::Time());
		});

	EXPECT_EQ(boolCol->DefaultValue(),Value(false));
	EXPECT_EQ(intCol->DefaultValue(),Value(0));
	EXPECT_EQ(doubleCol->DefaultValue(),Value(0.0));
	EXPECT_EQ(stringCol->DefaultValue(),Value(std::string()));
	EXPECT_EQ(timeCol->DefaultValue(),Value(Time()));

	EXPECT_NO_THROW({
			boolCol->SetDefaultValue(true);
		});

	EXPECT_NO_THROW({
			AntMetadata::SetKey(metadata,"bool",true);
		});

	EXPECT_NO_THROW({
			boolCol->SetDefaultValue(fort::Time());
		});

	EXPECT_EQ(boolCol->DefaultValue(),Value(Time()));

}




} // namespace priv
} // namespace myrmidon
} // namespace fort
