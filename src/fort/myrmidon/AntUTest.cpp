#include <gtest/gtest.h>

#include "Experiment.hpp"
#include "UtilsUTest.hpp"

namespace fort {
namespace myrmidon {

class PublicAntUTest : public ::testing::Test {
protected:

	void SetUp() {
		e = Experiment::Create("test.myrmidon");
	}

	void TearDown() {
		e.reset();
	}

	Experiment::Ptr e;
};


TEST_F(PublicAntUTest,AntHaveUniqueID) {
	for (size_t i = 0; i < 10; i++ ) {
		auto a = e->CreateAnt();
		ASSERT_TRUE(a);
		EXPECT_EQ(a->ID(),i+1);
	}
}

TEST_F(PublicAntUTest,AntHaveSortedIdentifications) {
	auto a = e->CreateAnt();
	auto t1 = Time::Now();
	auto t2 = t1.Add(Duration::Second);

	Identification::Ptr i1,i2,i3;

	ASSERT_NO_THROW({

			i3 = e->AddIdentification(a->ID(),
			                          2,
			                          t2,
			                          Time::Forever());

			i2 = e->AddIdentification(a->ID(),
			                          1,
			                          t1,
			                          t2);

			i1 = e->AddIdentification(a->ID(),
			                          0,
			                          Time::SinceEver(),
			                          t1);
		});


	// These expectation works as we returns the right smart pointer
	ASSERT_EQ(a->Identifications().size(),3);
	EXPECT_EQ(a->Identifications()[0],i1);
	EXPECT_EQ(a->Identifications()[1],i2);
	EXPECT_EQ(a->Identifications()[2],i3);

	e->DeleteIdentification(i1);

	ASSERT_EQ(a->Identifications().size(),2);
	EXPECT_EQ(a->Identifications()[0],i2);
	EXPECT_EQ(a->Identifications()[1],i3);

	EXPECT_EQ(a->IdentifiedAt(t2),2);

	EXPECT_THROW(a->IdentifiedAt(t1.Add(-1)),std::runtime_error);
}


TEST_F(PublicAntUTest,AntHaveDisplayStatus) {
	auto a = e->CreateAnt();

	EXPECT_EQ(a->DisplayColor(),DefaultPaletteColor(0));

	a->SetDisplayColor(DefaultPaletteColor(42));

	EXPECT_EQ(a->DisplayColor(),DefaultPaletteColor(42));

	EXPECT_EQ(a->DisplayStatus(),Ant::DisplayState::VISIBLE);
	a->SetDisplayStatus(Ant::DisplayState::HIDDEN);

	EXPECT_EQ(a->DisplayStatus(),Ant::DisplayState::HIDDEN);
}


TEST_F(PublicAntUTest,AntHaveStaticValue) {
	auto a = e->CreateAnt();

	auto t = Time::Now();

	e->SetMetaDataKey("alive",true);
	auto values = a->GetValues("alive");
	EXPECT_EQ(values.size(),1);
	ASSERT_EQ(values.count(Time::SinceEver()),1);
	EXPECT_VALUE_EQ(values.at(Time::SinceEver()),true);


	EXPECT_THROW(a->GetValue("isDead",t),std::out_of_range);

	EXPECT_THROW(a->SetValue("isDead",true,t),std::out_of_range);
	EXPECT_THROW(a->SetValue("alive",42,t),std::runtime_error);
	EXPECT_THROW(a->SetValue("alive",false,Time::Forever()),std::invalid_argument);


	EXPECT_NO_THROW(a->SetValue("alive",false,t));

	values = a->GetValues("alive");
	EXPECT_EQ(values.size(),2);
	ASSERT_EQ(values.count(Time::SinceEver()),1);
	ASSERT_EQ(values.count(t),1);
	EXPECT_VALUE_EQ(values.at(Time::SinceEver()),true);
	EXPECT_VALUE_EQ(values.at(t),false);

	EXPECT_THROW(a->GetValues("isDead"),std::out_of_range);


	EXPECT_VALUE_EQ(a->GetValue("alive",Time::SinceEver()),true);
	EXPECT_VALUE_EQ(a->GetValue("alive",t.Add(-1)),true);
	EXPECT_VALUE_EQ(a->GetValue("alive",t),false);
	EXPECT_VALUE_EQ(a->GetValue("alive",Time::Forever()),false);

	EXPECT_THROW(a->DeleteValue("isDead",t),std::out_of_range);
	EXPECT_THROW(a->DeleteValue("alive",t.Add(1)),std::out_of_range);

	EXPECT_NO_THROW(a->DeleteValue("alive",t));
	EXPECT_VALUE_EQ(a->GetValue("alive",t),true);
	EXPECT_VALUE_EQ(a->GetValue("alive",Time::Forever()),true);

	e->SetMetaDataKey("alive",false);

	EXPECT_VALUE_EQ(a->GetValue("alive",Time::SinceEver()),false);
	EXPECT_VALUE_EQ(a->GetValue("alive",t.Add(-1)),false);
	EXPECT_VALUE_EQ(a->GetValue("alive",t),false);
	EXPECT_VALUE_EQ(a->GetValue("alive",Time::Forever()),false);
}


TEST_F(PublicAntUTest,AntHaveVirtualShape) {
	e->CreateAntShapeType("body");
	e->CreateAntShapeType("antenna");

	auto a = e->CreateAnt();
	EXPECT_EQ(a->Capsules().size(),0);
	auto c1 = std::make_shared<Capsule>(Eigen::Vector2d(0,0),
	                                    Eigen::Vector2d(1,1),
	                                    1,
	                                    1);
	auto c2 = std::make_shared<Capsule>(Eigen::Vector2d(0,0),
	                                    Eigen::Vector2d(-1,-1),
	                                    1,
	                                    1);
	EXPECT_THROW(a->AddCapsule(42,c1),std::invalid_argument);
	EXPECT_NO_THROW(a->AddCapsule(1,c1));
	EXPECT_NO_THROW(a->AddCapsule(1,c2));
	EXPECT_NO_THROW(a->AddCapsule(2,c1)); // capsules can overlap

	EXPECT_EQ(a->Capsules().size(),3);
	EXPECT_EQ(a->Capsules()[0].second,c1);
	EXPECT_EQ(a->Capsules()[1].second,c2);
	EXPECT_EQ(a->Capsules()[2].second,c1); // they are the same shared object


	EXPECT_THROW(a->DeleteCapsule(42),std::out_of_range);
	EXPECT_NO_THROW(a->DeleteCapsule(1));

	EXPECT_EQ(a->Capsules().size(),2);
	EXPECT_EQ(a->Capsules()[0].second,c1);
	EXPECT_EQ(a->Capsules()[1].second,c1);


	EXPECT_NO_THROW(a->ClearCapsules());
	EXPECT_EQ(a->Capsules().size(),0);

}


TEST_F(PublicAntUTest,ScopeValidity) {
	auto a = e->CreateAnt();
	e->SetMetaDataKey("alive",true);
	e.reset();
	// setting a value require access to the global experiment, if no
	// handle mechanic, a DeletedReference would be thrown.
	EXPECT_NO_THROW(a->SetValue("alive",false,Time()));
}


} // namespace myrmidon
} // namespace fort
