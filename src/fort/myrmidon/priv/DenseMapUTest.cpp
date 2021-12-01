#include <gtest/gtest.h>

#include "DenseMap.hpp"

#include <random>

#include <fort/time/Time.hpp>


namespace fort {
namespace myrmidon {
namespace priv {

class DenseMapUTest : public::testing::Test {};


typedef DenseMap<uint32_t,uint32_t> DM;
typedef DenseMap<uint32_t,std::shared_ptr<uint32_t>> DMOfPtr;

TEST_F(DenseMapUTest,TestInsertion) {
	DM map;
	EXPECT_THROW({
			// could not insert object 0
			auto res = map.insert(std::make_pair(0,0));
		},std::invalid_argument);

	auto res = map.insert(std::make_pair(1,0));
	EXPECT_TRUE(res.second);
	EXPECT_EQ(res.first->first,1);
	EXPECT_EQ(res.first->second,0);
	EXPECT_EQ((*res.first).first,1);
	EXPECT_EQ((*res.first).second,0);

	res = map.insert(std::make_pair(1,100));
	EXPECT_FALSE(res.second);
	EXPECT_EQ(res.first->first,1);
	EXPECT_EQ(res.first->second,0);
	EXPECT_EQ((*res.first).first,1);
	EXPECT_EQ((*res.first).second,0);

	res = map.insert(std::make_pair(10,100));
	EXPECT_TRUE(res.second);
	EXPECT_EQ(res.first->first,10);
	EXPECT_EQ(res.first->second,100);
	EXPECT_EQ((*res.first).first,10);
	EXPECT_EQ((*res.first).second,100);
}

TEST_F(DenseMapUTest,IterationAndErase) {
	std::vector<uint32_t> values = {1,2,4,5,6,7,8,9,10,12};
	DM map;
	size_t i = 0;
	for ( const auto & v : values ) {
		auto res = map.insert(std::make_pair(2*i+1,v));
		ASSERT_TRUE(res.second);
		++i;
	}

	ASSERT_EQ(map.size(),values.size());
	i = 0;
	for ( const auto & [k,v] : map) {
		EXPECT_EQ(k,2*i+1);
		EXPECT_EQ(v,values[i]);
		++i;
	}

	for( auto it = map.end();
	     it != map.begin();
	     --it) {
	}

	for( auto it = map.cend();
	     it != map.cbegin();
	     --it) {
	}


	EXPECT_EQ(--(map.begin()),map.begin());
	EXPECT_EQ(++map.end(),map.end());
	EXPECT_EQ(--map.cbegin(),map.cbegin());
	EXPECT_EQ(++map.cend(),map.cend());

	//decrement bug if no first key.
	auto begin = map.begin();
	map.erase(begin);
	EXPECT_NO_THROW(map.erase(begin));
	EXPECT_NO_THROW(map.erase(1));
	EXPECT_THROW(map.at(1),std::out_of_range);
	EXPECT_EQ(--(map.begin()),map.begin());
	EXPECT_EQ(--(const_cast<const DM*>(&map)->begin()),map.cbegin());
	EXPECT_EQ(--(map.cbegin()),map.cbegin());



	map.insert({1,1});

	for ( i = 0; i < values.size(); ++i) {
		auto erased = map.erase(2*i+1);
		EXPECT_EQ(erased,1);
		EXPECT_EQ(map.size(),values.size() - i - 1);
		size_t ii = i+1;
		for ( const auto & [k,v] : map) {
			EXPECT_EQ(k,2*ii+1);
			EXPECT_EQ(v,values[ii]);
			++ii;
		}
	}

}

TEST_F(DenseMapUTest,Find) {
	std::vector<uint32_t> values = {1,2,4,5,6,7,8,9,10,12};
	DM map;
	size_t i = 0;
	for ( const auto & v : values ) {
		++i;
		auto res = map.insert(std::make_pair(i,v));
		ASSERT_TRUE(res.second);
	}

	DM::const_iterator cfi = map.find(4);
	EXPECT_TRUE(cfi != map.end());
	EXPECT_EQ(cfi->second,5);

	DM::iterator fi = map.find(8);
	EXPECT_TRUE(fi != map.end());
	EXPECT_EQ(fi->second,9);

	DM::const_iterator cnull = map.find(89);
	EXPECT_TRUE(cnull == map.end());


}

TEST_F(DenseMapUTest,ConsistencyAndTiming) {
	std::random_device r;
	// Choose a random mean between 1 and 6
    std::default_random_engine e1(r());

    const static uint32_t SIZE = 2000;

    std::uniform_int_distribution<uint32_t> uniform(0, SIZE);

    std::set<uint32_t> keys;
    std::map<uint32_t,std::shared_ptr<uint32_t>> map;
    DMOfPtr denseMap;

    for (size_t i = 0; i < SIZE; ++i ) {
	    // we must ensure key > 0 for DenseMap
	    auto toInsert = std::make_pair(uniform(e1)+1,std::make_shared<uint32_t>(uniform(e1)));

	    auto mapRes = map.insert(toInsert);
	    auto denseMapRes = denseMap.insert(toInsert);
	    EXPECT_EQ(denseMapRes.second,mapRes.second);
	    keys.insert(toInsert.first);
    }

    EXPECT_EQ(denseMap.size(),map.size());

    // Check map are the same
    auto start = map.cbegin();
    auto denseStart = denseMap.cend();
    while (true) {
	    if ( start == map.cend() || denseStart == denseMap.cend() ) {
		    break;
	    }
	    EXPECT_EQ(start->first,denseStart->first);
	    EXPECT_EQ(start->second,denseStart->second);
	    EXPECT_EQ(*start->second,*denseStart->second);

	    ++start;++denseStart;
    }

#ifdef MYRMIDON_TEST_TIMING
    // Random access time is faster. We compare here O(log(n)) and O(1).
    double meanMapTime;
    double meanDenseMapTime;
    for ( const auto & k :keys ) {
	    auto start = Time::Now();
	    auto res = map.at(k);
	    auto middle = Time::Now();
	    auto denseRes = denseMap.at(k);
	    auto end = Time::Now();

	    meanMapTime += middle.Sub(start).Microseconds() / keys.size();
	    meanDenseMapTime += end.Sub(middle).Microseconds() / keys.size();
    }

    EXPECT_TRUE( meanMapTime >=  meanDenseMapTime)
	    << "std::map mean random access time: " << meanMapTime << "μs "
	    << " DenseMap mean random access time: " << meanDenseMapTime << "μs";
#endif
}

TEST_F(DenseMapUTest,UpperLowerKey) {

	struct TestData {
		uint32_t Key,ExpectedLower,ExpectedUpper;
	};
	 auto performTests =
		 [](const std::vector<TestData> & testdata,
		    const DM & m) {
			 for ( const auto & d : testdata ) {
				 SCOPED_TRACE("Key: "+std::to_string(d.Key));
				 bool noError = true;
				 auto res = m.lower_key(d.Key);
				 EXPECT_EQ(res == m.cend(),d.ExpectedLower == 0)
					 << (noError = false);
				 if ( noError && d.ExpectedLower != 0 ) {
					 EXPECT_EQ(res->first,d.ExpectedLower);
				 }
				 noError = true;
				 res = m.upper_key(d.Key);
				 EXPECT_EQ(res == m.cend(),d.ExpectedUpper == 0)
					 << (noError = false);
				 if ( noError && d.ExpectedUpper != 0 ) {
					 EXPECT_EQ(res->first,d.ExpectedUpper);
				 }
			 }
		 };


	DM m;

	std::vector<TestData> testdata
		= {
		   {0,0,0},
		   {1,0,0},
		   {42,0,0},
	};
	{
		SCOPED_TRACE("Empty DenseMap");
		performTests(testdata,m);
	}

	m.insert({2,2});
	m.insert({4,4});
	m.insert({5,5});
	m.insert({6,6});

	testdata = {
		   {0,0,0},
		   {1,0,2},
		   {2,2,2},
		   {3,2,4},
		   {4,4,4},
		   {5,5,5},
		   {6,6,6},
		   {7,6,0},
	};

	{
		SCOPED_TRACE("Not Fully DenseMap");
		performTests(testdata,m);
	}

	m.insert({1,1});
	m.insert({3,3});

	testdata = {
		   {0,0,0},
		   {1,1,1},
		   {2,2,2},
		   {3,3,3},
		   {4,4,4},
		   {5,5,5},
		   {6,6,6},
		   {7,6,0},
	};


	{
		SCOPED_TRACE("Fully DenseMap");
		performTests(testdata,m);
	}


}


} // namespace priv
} // namespace myrmidon
} // namespace fort
