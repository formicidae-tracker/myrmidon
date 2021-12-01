#include <gtest/gtest.h>

#include <fort/myrmidon/Shapes.hpp>

#include "Ant.hpp"
#include "AntShapeType.hpp"
#include "AntMetadata.hpp"
#include "Experiment.hpp"
#include "Identifier.hpp"

namespace fort {
namespace myrmidon {
namespace priv {

class AntUTest : public ::testing::Test {
protected:
	void SetUp() {
		shapeTypes = std::make_shared<AntShapeTypeContainer>();
		shapeTypes->Create("body",1);
		shapeTypes->Create("antennas",2);

		antMetadata = std::make_shared<AntMetadata>();
		auto dead = AntMetadata::SetKey(antMetadata,"dead",false);
		auto group = AntMetadata::SetKey(antMetadata,"group",std::string());
		ASSERT_EQ(dead->Type(),ValueType::BOOL);
		ASSERT_EQ(group->Type(),ValueType::STRING);
		ant = std::make_shared<Ant>(shapeTypes,
		                            antMetadata,
		                            1);


	}
	void TearDown() {
		ant.reset();
		antMetadata.reset();
		shapeTypes.reset();
	}

	AntShapeTypeContainerPtr shapeTypes;
	AntMetadataPtr           antMetadata;
	AntPtr                   ant;
};




TEST_F(AntUTest,CapsuleEdition) {
	Capsule capsule(Eigen::Vector2d(0,0),
	                Eigen::Vector2d(1,1),
	                0.1,
	                0.1);


	EXPECT_THROW(ant->AddCapsule(3,std::make_unique<Capsule>()),std::invalid_argument);
	EXPECT_NO_THROW({
			ant->AddCapsule(1,std::make_shared<Capsule>(capsule));
			ant->AddCapsule(2,std::make_shared<Capsule>(capsule));
			ant->AddCapsule(2,std::make_shared<Capsule>(capsule));
		});
	EXPECT_EQ(ant->Capsules().size(),3);
	EXPECT_THROW(ant->DeleteCapsule(3),std::out_of_range);
	EXPECT_NO_THROW(ant->DeleteCapsule(1));
	EXPECT_EQ(ant->Capsules().size(),2);
	EXPECT_NO_THROW(ant->ClearCapsules());
	EXPECT_EQ(ant->Capsules().size(),0);
}

TEST_F(AntUTest,StaticDataTest) {
	try {
		EXPECT_FALSE(std::get<bool>(ant->GetValue("dead",Time())));
		EXPECT_EQ(std::get<std::string>(ant->GetValue("group",Time())),"");
	} catch ( const std::exception & e ) {
		ADD_FAILURE() << "Got unexpected exception: " << e.what();
	}

	EXPECT_THROW(ant->SetValue("isQueen",true,Time::SinceEver()),std::out_of_range);
	EXPECT_THROW(ant->SetValue("dead",0,Time::SinceEver()),std::runtime_error);
	EXPECT_THROW({ant->SetValue("dead",false,Time::Forever());},std::invalid_argument);
	EXPECT_NO_THROW({
			ant->SetValue("dead",true,Time::SinceEver());
			ant->SetValue("dead",false,Time::FromTimeT(42));
			ant->SetValue("dead",false,Time::SinceEver());
			ant->SetValue("dead",true,Time::FromTimeT(42));
			EXPECT_EQ(ant->DataMap().size(),1);
			ant->SetValue("group",std::string("forager"),Time::SinceEver());



		});

	EXPECT_THROW(ant->DeleteValue("isQueen",Time::SinceEver()),std::out_of_range);
	EXPECT_THROW(ant->DeleteValue("dead",Time()),std::out_of_range);
	EXPECT_NO_THROW(ant->DeleteValue("dead",Time::SinceEver()));


	EXPECT_THROW(ant->GetBaseValue("dead"),std::out_of_range);


	EXPECT_NO_THROW(ant->SetValue("dead",false,Time(),true));
	EXPECT_THROW(ant->SetValue("dead",false,Time(),true),std::runtime_error);

	EXPECT_NO_THROW(ant->DeleteValue("group",Time::SinceEver()));
}

TEST_F(AntUTest,IDFormatting) {
	std::vector<std::pair<AntID,std::string>> testdata
		= {
		   {1,"001"},
		   {10,"010"},
	};

	for( const auto & d : testdata ) {
		EXPECT_EQ(FormatAntID(d.first),d.second);
	}

}

TEST_F(AntUTest,IdentificationAt) {
	auto e = Experiment::Create("/tmp/foo.myrmidon");
	auto a = e->CreateAnt();
	Identifier::AddIdentification(e->Identifier(),
		a->AntID(),0,Time::FromTimeT(0),Time::FromTimeT(1));
	EXPECT_NO_THROW({
			EXPECT_EQ(a->IdentifiedAt(Time::FromTimeT(0)),0);
		});
	EXPECT_THROW(a->IdentifiedAt(Time::FromTimeT(1)),std::runtime_error);
}

} // namespace priv
} // namespace myrmidon
} // namespace fort
