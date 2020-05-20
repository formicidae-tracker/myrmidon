#pragma once

#include <fort/myrmidon/Types.hpp>

#include "Identifier.hpp"

namespace fort {
namespace myrmidon {
namespace priv {

class Matcher {
public:
	typedef std::shared_ptr<Matcher> Ptr;

	static Ptr And(const std::vector<Ptr> & matcher);

	static Ptr Or(const std::vector<Ptr> & matcher);

	static Ptr AntIDMatcher(AntID ID);

	static Ptr AntColumnMatcher(const std::string & name, const AntStaticValue & value);

	static Ptr AntDistanceSmallerThan(double distance);

	static Ptr AntDistanceGreaterThan(double distance);

	static Ptr AntAngleGreaterThan(double angle);

	static Ptr AntAngleSmallerThan(double angle);

	virtual void SetUpOnce(const ConstAntByID & ants) = 0;

	virtual void SetUp(const IdentifiedFrame::ConstPtr & identifiedFrame,
	                   const InteractionFrame::ConstPtr & interactionFrame) = 0;

	virtual bool Match(fort::myrmidon::AntID ant1,
	                   fort::myrmidon::AntID ant2,
	                   const std::vector<InteractionType> & types) = 0;
};

} // namespace priv
} // namespace myrmidon
} // namespace fort