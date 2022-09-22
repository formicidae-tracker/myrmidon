#pragma once

#include <vector>
#include <string>

#include <fort/time/Time.hpp>

#include <fort/myrmidon/types/Typedefs.hpp>
#include <fort/myrmidon/types/Collision.hpp>
#include <fort/myrmidon/types/Value.hpp>

#include "ForwardDeclaration.hpp"


namespace fort {
namespace myrmidon {
namespace priv {

class Matcher {
public:
	typedef std::shared_ptr<Matcher> Ptr;

	static Ptr And(const std::vector<Ptr> & matcher);

	static Ptr Or(const std::vector<Ptr> & matcher);

	static Ptr AntIDMatcher(AntID ID);

	static Ptr AntColumnMatcher(const std::string & name, const Value & value);

	static Ptr AntDistanceSmallerThan(double distance);

	static Ptr AntDistanceGreaterThan(double distance);

	static Ptr AntAngleGreaterThan(double angle);

	static Ptr AntAngleSmallerThan(double angle);

	static Ptr InteractionType(AntShapeTypeID type1,
	                           AntShapeTypeID type2);

	static Ptr AntDisplacement(double under,
	                           Duration minimumGap);

	virtual void SetUpOnce(const priv::AntByID & ants) = 0;

	virtual uint8_t Depth() const { return 1; };


	virtual void SetUp(const IdentifiedFrame & identifiedFrame) = 0;

	virtual uint64_t Match(fort::myrmidon::AntID ant1,
	                       fort::myrmidon::AntID ant2,
	                       const fort::myrmidon::InteractionTypes & types) = 0;

	virtual void Format(std::ostream & out) const = 0;

	virtual ~Matcher();
};

std::ostream & operator<<(std::ostream & out, const fort::myrmidon::priv::Matcher & m);

} // namespace priv
} // namespace myrmidon
} // namespace fort
