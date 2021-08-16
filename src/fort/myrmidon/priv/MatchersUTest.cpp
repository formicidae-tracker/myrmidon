#include "MatchersUTest.hpp"

#include "Matchers.hpp"
#include "Experiment.hpp"
#include "Ant.hpp"

#include <fort/myrmidon/TestSetup.hpp>

#include <gmock/gmock.h>

using ::testing::_;

namespace fort {
namespace myrmidon {
namespace priv {

class StaticMatcher : public Matcher {
private:
	bool d_value;
public:
	StaticMatcher(bool value) : d_value(value) {};

	void SetUpOnce(const AntByID & ants) override {
	}

	void SetUp(const IdentifiedFrame::Ptr & identifiedFrame,
	           const CollisionFrame::Ptr & collisionFrame) override{
	};

	bool Match(fort::myrmidon::AntID ant1,
	           fort::myrmidon::AntID ant2,
	           const fort::myrmidon::InteractionTypes & types) override {
		return d_value;
	};

	void Format(std::ostream & out) const override {
		out << std::boolalpha << d_value;
	}

	static Ptr Create(bool value) {
		return std::make_shared<StaticMatcher>(value);
	}

};

class MockMatcher : public Matcher {
public:
	MOCK_METHOD(void,SetUpOnce,(const AntByID & ants),(override));
	MOCK_METHOD(void,SetUp,(const IdentifiedFrame::Ptr & f,
	                        const CollisionFrame::Ptr & i),(override));
	MOCK_METHOD(bool,Match,(fort::myrmidon::AntID a,
	                        fort::myrmidon::AntID b,
	                        const fort::myrmidon::InteractionTypes & types), (override));
	MOCK_METHOD(void,Format,(std::ostream & out), (const override));
};

TEST_F(MatchersUTest,AndMatcher) {
	EXPECT_FALSE(Matcher::And({StaticMatcher::Create(false),StaticMatcher::Create(false)})->Match(0,0,{}));
	EXPECT_FALSE(Matcher::And({StaticMatcher::Create(false),StaticMatcher::Create(true)})->Match(0,0,{}));
	EXPECT_FALSE(Matcher::And({StaticMatcher::Create(true),StaticMatcher::Create(false)})->Match(0,0,{}));
	EXPECT_TRUE(Matcher::And({StaticMatcher::Create(true),StaticMatcher::Create(true)})->Match(0,0,{}));
}

TEST_F(MatchersUTest,OrMatcher) {
	EXPECT_FALSE(Matcher::Or({StaticMatcher::Create(false),StaticMatcher::Create(false)})->Match(0,0,{}));
	EXPECT_TRUE(Matcher::Or({StaticMatcher::Create(false),StaticMatcher::Create(true)})->Match(0,0,{}));
	EXPECT_TRUE(Matcher::Or({StaticMatcher::Create(true),StaticMatcher::Create(false)})->Match(0,0,{}));
	EXPECT_TRUE(Matcher::Or({StaticMatcher::Create(true),StaticMatcher::Create(true)})->Match(0,0,{}));
}


TEST_F(MatchersUTest,CombinedMatchersSetup) {
	std::vector<std::shared_ptr<MockMatcher>> matchers
		= {
		   std::make_shared<MockMatcher>(),
		   std::make_shared<MockMatcher>(),
	};

	EXPECT_CALL(*matchers[0],SetUpOnce(_)).Times(2);
	EXPECT_CALL(*matchers[1],SetUpOnce(_)).Times(2);

	EXPECT_CALL(*matchers[0],SetUp(_,_)).Times(2);
	EXPECT_CALL(*matchers[1],SetUp(_,_)).Times(2);


	auto andMatcher = Matcher::And({matchers[0],matchers[1]});
	auto orMatcher = Matcher::Or({matchers[0],matchers[1]});

	andMatcher->SetUpOnce({});
	orMatcher->SetUpOnce({});

	andMatcher->SetUp({},{});
	orMatcher->SetUp({},{});

	andMatcher.reset();
	orMatcher.reset();
	matchers.clear();

}


TEST_F(MatchersUTest,IDMatcher) {
	auto idMatcher = Matcher::AntIDMatcher(42);
	ASSERT_NO_THROW({
			idMatcher->SetUpOnce({});
			idMatcher->SetUp({},{});
		});

	//Matches as expected for trajectories
	EXPECT_TRUE(idMatcher->Match(42,0,{}));
	// Matches as expected for interactions
	EXPECT_TRUE(idMatcher->Match(42,43,{}));
	// Matches when other in interaction matches
	EXPECT_TRUE(idMatcher->Match(41,42,{}));

	// does not match as expected
	EXPECT_FALSE(idMatcher->Match(41,43,{}));
	EXPECT_FALSE(idMatcher->Match(0,0,{}));
}

TEST_F(MatchersUTest,ColumnMatcher) {
	auto columnMatcher = Matcher::AntColumnMatcher("bar",42);

	auto experiment = Experiment::Create(TestSetup::UTestData().Basedir() / "matcher.myrmidon");
	auto bar = experiment->SetMetaDataKey("bar",int(0));
	ASSERT_EQ(bar->Type(),AntMetaDataType::INT);

	auto a = experiment->CreateAnt();
	a->SetValue("bar",42,Time::SinceEver());
	a->SetValue("bar",43,Time());
	auto b = experiment->CreateAnt();
	b->SetValue("bar",42,Time::SinceEver());



	auto identifiedFrame = std::make_shared<IdentifiedFrame>();
	identifiedFrame->FrameTime = Time().Add(-1);
	auto collisionFrame = std::make_shared<CollisionFrame>();
	ASSERT_NO_THROW({
			columnMatcher->SetUpOnce(experiment->Identifier()->Ants());
		});

	ASSERT_THROW({
			columnMatcher->SetUp({},{});
		},std::runtime_error);

	ASSERT_NO_THROW({
			columnMatcher->SetUp(identifiedFrame,{});
		});

	EXPECT_FALSE(columnMatcher->Match(0,0,{}));
	EXPECT_TRUE(columnMatcher->Match(a->AntID(),0,{}));
	// it should also match with another ant in interaction.
	EXPECT_TRUE(columnMatcher->Match(a->AntID(),b->AntID(),{}));
	ASSERT_NO_THROW({
			columnMatcher->SetUp({},collisionFrame);
		});

	EXPECT_FALSE(columnMatcher->Match(a->AntID(),0,{}));

}


TEST_F(MatchersUTest,DistanceMatcher) {
	auto greaterMatcher = Matcher::AntDistanceGreaterThan(10);
	auto smallerMatcher = Matcher::AntDistanceSmallerThan(10);


	auto identifiedFrame = std::make_shared<IdentifiedFrame>();
	identifiedFrame->Positions.resize(3,5);
	identifiedFrame->Positions.row(0) << 1,0,0,0,0;
	identifiedFrame->Positions.row(1) << 2,0,12,0,0;
	identifiedFrame->Positions.row(2) << 3,0,8,0,0;
	auto collisionFrame = std::make_shared<CollisionFrame>();

	ASSERT_NO_THROW({ greaterMatcher->SetUpOnce({}); });

	ASSERT_THROW({
			greaterMatcher->SetUp({},{});
		},std::runtime_error);

	ASSERT_THROW({
			smallerMatcher->SetUp({},collisionFrame);
		},std::runtime_error);

	ASSERT_NO_THROW({
			greaterMatcher->SetUp(identifiedFrame,{});
			smallerMatcher->SetUp(identifiedFrame,{});
		});


	EXPECT_TRUE(greaterMatcher->Match(0,0,{}));
	EXPECT_TRUE(greaterMatcher->Match(1,2,{}));
	EXPECT_FALSE(greaterMatcher->Match(1,3,{}));

	EXPECT_TRUE(smallerMatcher->Match(0,0,{}));
	EXPECT_FALSE(smallerMatcher->Match(1,2,{}));
	EXPECT_TRUE(smallerMatcher->Match(1,3,{}));


}


TEST_F(MatchersUTest,AngleMatcher) {
	auto greaterMatcher = Matcher::AntAngleGreaterThan(M_PI/4);
	auto smallerMatcher = Matcher::AntAngleSmallerThan(M_PI/4);


	auto identifiedFrame = std::make_shared<IdentifiedFrame>();
	identifiedFrame->Positions.resize(3,5);
	identifiedFrame->Positions.row(0) << 1,0,0,0,0;
	identifiedFrame->Positions.row(1) << 2,0,0,M_PI/5,0;
	identifiedFrame->Positions.row(2) << 3,0,0,-M_PI/3,0;
	auto collisionFrame = std::make_shared<CollisionFrame>();

	ASSERT_NO_THROW({ greaterMatcher->SetUpOnce({}); });

	ASSERT_THROW({
			greaterMatcher->SetUp({},{});
		},std::runtime_error);

	ASSERT_THROW({
			smallerMatcher->SetUp({},collisionFrame);
		},std::runtime_error);

	ASSERT_NO_THROW({
			greaterMatcher->SetUp(identifiedFrame,{});
			smallerMatcher->SetUp(identifiedFrame,{});
		});


	EXPECT_TRUE(greaterMatcher->Match(0,0,{}));
	EXPECT_FALSE(greaterMatcher->Match(1,2,{}));
	EXPECT_TRUE(greaterMatcher->Match(1,3,{}));

	EXPECT_TRUE(smallerMatcher->Match(0,0,{}));
	EXPECT_TRUE(smallerMatcher->Match(1,2,{}));
	EXPECT_FALSE(smallerMatcher->Match(1,3,{}));


}

InteractionTypes BuildTypes(const std::vector<uint32_t> & types) {
	if ( (types.size() % 2) != 0 ) {
		throw std::invalid_argument("even number of types");
	}
	InteractionTypes res(types.size()/2,2);
	for (size_t i = 0; i < types.size()/2 ; ++i) {
		res(i,0) = types[2*i];
		res(i,1) = types[2*i+1];
	}
	return res;
}

TEST_F(MatchersUTest,InteractionsTypesMatcher) {
	auto oneOne = priv::Matcher::InteractionType(1,1);
	auto oneTwo = priv::Matcher::InteractionType(1,2);
	auto twoThree = priv::Matcher::InteractionType(3,2);

	EXPECT_NO_THROW({
			oneOne->SetUpOnce({});
			oneTwo->SetUpOnce({});
			oneOne->SetUp({},{});
			oneTwo->SetUp({},{});
		});
	// always matches single ant
	EXPECT_TRUE(oneOne->Match(1,0,{}));
	EXPECT_TRUE(oneTwo->Match(42,0,{}));


	EXPECT_TRUE(oneOne->Match(1,2,BuildTypes({1,1,1,2})));
	EXPECT_FALSE(oneOne->Match(1,2,BuildTypes({2,1,1,2})));


	EXPECT_TRUE(oneTwo->Match(1,2,BuildTypes({1,1,1,2})));
	EXPECT_TRUE(oneTwo->Match(1,2,BuildTypes({1,1,2,1})));
	EXPECT_FALSE(oneTwo->Match(1,2,BuildTypes({1,1,2,2})));


	EXPECT_TRUE(twoThree->Match(1,2,BuildTypes({1,1,2,3})));
	EXPECT_TRUE(twoThree->Match(1,2,BuildTypes({1,1,3,2})));
	EXPECT_FALSE(twoThree->Match(1,2,BuildTypes({1,1,2,2,3,3})));

}

TEST_F(MatchersUTest,Formatting) {
	struct TestData {
		Matcher::Ptr M;
		std::string Expected;
	};

	std::vector<TestData> testdata
		= {
		   {
		    Matcher::AntIDMatcher(1),
		    "Ant.ID == 001",
		   },
		   {
		    Matcher::AntColumnMatcher("foo",42.3),
		    "Ant.'foo' == 42.3",
		   },
		   {
		    Matcher::AntDistanceSmallerThan(42.3),
		    "Distance(Ant1, Ant2) < 42.3",
		   },
		   {
		    Matcher::AntDistanceGreaterThan(42.3),
		    "Distance(Ant1, Ant2) > 42.3",
		   },
		   {
		    Matcher::AntAngleSmallerThan(0.1),
		    "Angle(Ant1, Ant2) < 0.1",
		   },
		   {
		    Matcher::AntAngleGreaterThan(0.1),
		    "Angle(Ant1, Ant2) > 0.1",
		   },
		   {
		    Matcher::And({StaticMatcher::Create(false),
		                  StaticMatcher::Create(true),
		                  StaticMatcher::Create(false)}),
		    "( false && true && false )",
		   },
		   {
		    Matcher::Or({StaticMatcher::Create(false),
		                  StaticMatcher::Create(true),
		                  StaticMatcher::Create(false)}),
		    "( false || true || false )",
		   },
		   {
		    Matcher::InteractionType(4,2),
		    "InteractionType (2 - 4)",
		   },

		   {
		    Matcher::InteractionType(5,5),
		    "InteractionType (5 - 5)",
		   },

	};

	for ( const auto & d : testdata ) {
		std::ostringstream oss;
		oss << *d.M;
		EXPECT_EQ(oss.str(),d.Expected);
	}
}



} // namespace priv
} // namespace myrmidon
} // namespace fort
