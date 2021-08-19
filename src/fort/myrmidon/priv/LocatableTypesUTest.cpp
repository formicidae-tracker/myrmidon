#include <gtest/gtest.h>

#include "LocatableTypes.hpp"

namespace fort {
namespace myrmidon {
namespace priv {

class LocatableTypesUTest : public ::testing::Test {};

class A : public Identifiable {
public:

	A(const std::string & URI)
		: d_URI(URI) {
	}

	const std::string & URI() const override {
		return d_URI;
	}

private:
	std::string d_URI;
};


TEST_F(LocatableTypesUTest,IdentifiableAreComparable) {
	struct TestData {
		std::shared_ptr<Identifiable> A,B;
		bool                          Expected;
	};
	std::vector<TestData> testdata =
		{
		 {std::make_shared<A>("a"),std::make_shared<A>("a"),false},
		 {std::make_shared<A>("a"),std::make_shared<A>("b"),true},
		 {std::make_shared<A>("b"),std::make_shared<A>("a"),false},
		};

	for (const auto & d : testdata ) {
		Identifiable::Comparator c;
		EXPECT_EQ(c(d.A,d.B),
		          d.Expected);
		EXPECT_EQ(c(*d.A,*d.B),
		          d.Expected);
	}

}

} // namespace priv
} // namespace myrmidon
} // namespace fort
