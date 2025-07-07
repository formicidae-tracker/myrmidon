#include "BindTypes.hpp"

#include <fort/myrmidon/Matchers.hpp>

namespace py = pybind11;

fort::myrmidon::Matcher::Ptr MatcherAnd(py::args args) {
	std::vector<fort::myrmidon::Matcher::Ptr> matchers;
	for( const auto & a : args ) {
		matchers.push_back(py::cast<fort::myrmidon::Matcher::Ptr>(a));
	}
	return fort::myrmidon::Matcher::And(matchers);
}

fort::myrmidon::Matcher::Ptr MatcherOr(py::args args) {
	std::vector<fort::myrmidon::Matcher::Ptr> matchers;
	for( const auto & a : args ) {
		matchers.push_back(py::cast<fort::myrmidon::Matcher::Ptr>(a));
	}
	return fort::myrmidon::Matcher::Or(matchers);
}

void BindMatchers(py::module_ &m) {
	using namespace fort::myrmidon;
	py::class_<Matcher, std::shared_ptr<Matcher>>(
	    m,
	    "Matcher",
	    R"pydoc(
A Matcher helps to build complex :class:`Query` by adding one or
several constraints.

Matchers works either on single Ant for trajectory computation, or on
a pair of Ant when considering interactions. Some matcher have no real
meaning outside of interaction (i.e. :meth:`InteractionType`) and
would match any trajectory.

One would use the following function to get a Matcher :

  * :meth:`AntID` : one of the considered Ant in the trajectory or
    interaction should match a given AntID
  * :meth:`AntMetaData` : one of the key-value meta-data for one of the
    considered Ant should match.
  * :meth:`AntDistanceSmallerThan`, :meth:`AntDistanceGreaterThan` : for
    interaction queries only, ensure some criterion for the distance
    between the two considered ants.
  * :meth:`AntAngleSmallerThan`, :meth:`AntAngleGreaterThan` : for
    interaction queries only, ensure that angle between Ant meets some
    criterion.
  * :meth:`InteractionType` : considers only interaction of a given
    type.
  * :meth:`AntDisplacement`: matches interaction were the displacement
    of either of the Ant is kept under a threshold.

Using :meth:`And` or :meth:`Or`, one can combine several Matcher
together to build more complex criterion.
Examples:

    .. code-block:: python

        import fort_myrmidon as fm
        # m will match ant 001 or 002
        m = fm.Matcher.Or(fm.Matcher.AntID(1),fm.Matcher.AntID(2))

  )pydoc"
	)
	    .def_static(
	        "AntID",
	        &Matcher::AntID,
	        py::arg("antID"),
	        R"pydoc(
Matches a given AntID.

In case of interaction, matches any interaction with one of the Ant
having **antID**.

Args:
    antID (int): the AntID to match against.

Returns:
    Matcher: a matcher that matches **antID**.
)pydoc"
	    )
	    .def_static(
	        "AntMetaData",
	        &Matcher::AntMetaData,
	        py::arg("key"),
	        py::arg("value"),
	        R"pydoc(
Matches a given user meta data value.

In case of interaction, matches any interaction where at least one of
the Ant meets the criterion.

Args:
    key (str): the key to match from
    value (str): the value for key. Should be a bool, int, float, str
        or :class:`Time`

Returns:
    Matcher: a Matcher that matches Ant who current **key** meta data
    value matches **value**.
)pydoc"
	    )
	    .def_static(
	        "AntDistanceSmallerThan",
	        &Matcher::AntDistanceSmallerThan,
	        py::arg("distance"),
	        R"pydoc(
A Matcher that matches ant distance.

In the case of trajectories, it matches anything

Args:
    distance (float): the distance in pixel to match

Returns:
    Matcher: a Matcher that matches when the two Ant are within
    **distance**.
)pydoc"
	    )
	    .def_static(
	        "AntDistanceGreaterThan",
	        &Matcher::AntDistanceGreaterThan,
	        py::arg("distance"),
	        R"pydoc(
A Matcher that matches ant distance.

In the case of trajectories, it matches anything

Args:
    distance (float): the distance in pixel to match

Returns:
    Matcher: a Matcher that matches when the two Ant are further than
    **distance**.
)pydoc"
	    )
	    .def_static(
	        "AntAngleSmallerThan",
	        &Matcher::AntAngleSmallerThan,
	        py::arg("angle"),
	        R"pydoc(
A Matcher that matches ant angles.

In the case of trajectories, it matches anything

Args:
    angle (float): the angle in radians

Returns:
    Matcher: a Matcher that matches when the two Ant are facing the
    same direction within **angle**
)pydoc"
	    )
	    .def_static(
	        "AntAngleGreaterThan",
	        &Matcher::AntAngleGreaterThan,
	        py::arg("angle"),
	        R"pydoc(
A Matcher that matches ant angles.

In the case of trajectories, it matches anything

Args:
    angle (float): the angle in radians

Returns:
    Matcher: a Matcher that matches when the two Ant are facing
    directions which are greater appart than **angle**.
)pydoc"
	    )
	    .def_static(
	        "And",
	        &MatcherAnd,
	        R"pydoc(
Combines several Matcher together in conjuction

Args:
    *args (Matcher): several other Matcher

Returns:
    Matcher: a Matcher that matches when all passed matcher also
    matches.
)pydoc"
	    )
	    .def_static(
	        "Or",
	        &MatcherOr,
	        R"pydoc(
Combines several Matcher together in disjunction

Args:
    *args (fort_myrmidon.Matcher): several other Matcher

Returns:
    Matcher: a Matcher that matches when any of the passed matcher
    matches.
)pydoc"
	    )
	    .def_static(
	        "InteractionType",
	        &Matcher::InteractionType,
	        py::arg("type1"),
	        py::arg("type2"),
	        R"pydoc(
Matches InteractionType (type1,type2) and (type2,type1).

In the case of trajectories it matches anything.

Args:
    type1 (int): the first AntShapeTypeID to match
    type2 (int): the second AntShapeTypeID to match

Returns:
    Matcher: A Matcher that matches interactions (type1,type2) or
    (type2,type1).
)pydoc"
	    )
	    .def_static(
	        "AntDisplacement",
	        &Matcher::AntDisplacement,
	        py::arg("under"),
	        py::arg("minimumGap") = fort::Duration(0),
	        R"pydoc(
A Matcher that rejects large ants displacement.

Discards any trajectories and interactions where an Ant shows a
displacement from one detected position to another larger than
**under**. If **minimumGap** is larger than ``0s``, this check is
enforced only if there are more than minimumGap time ellapsed between
two tracked positions.

Args:
    under (float): maximum allowed Ant displacement in pixels
    minimumGap (Duration): minimum tracking time between positions to
        enable the check.

Returns:
    Matcher: A Matcher that matches small Ant displacement.
)pydoc"
	    )
	    .def(
	        "__str__",
	        [](const fort::myrmidon::Matcher::Ptr &m) -> std::string {
		        std::ostringstream oss;
		        oss << *m;
		        return oss.str();
	        }
	    );
};
