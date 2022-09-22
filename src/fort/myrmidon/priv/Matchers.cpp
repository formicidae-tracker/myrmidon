#include <fort/myrmidon/types/Value.hpp>
#include <sstream>

#include "Matchers.hpp"
#include "Ant.hpp"


namespace fort {
namespace myrmidon {
namespace priv {

Matcher::~Matcher() {}

Matcher::Ptr Matcher::And(const std::vector<Ptr>  &matchers) {
	class AndMatcher : public Matcher {
	private:
		std::vector<Ptr> d_matchers;
		std::vector<uint8_t> d_depths;
	public:
		AndMatcher(const std::vector<Ptr> & matchers)
			: d_matchers(matchers) {
			d_depths.push_back(0);
			for ( const auto & m : matchers) {
				d_depths.push_back(m->Depth() + d_depths.back());
			}
			if ( d_depths.back() > 64 ) {
				throw std::runtime_error("Maximal depth of this implementation is 64");
			}
		}
		virtual ~AndMatcher() {}
		void SetUpOnce(const AntByID & ants) override {
			std::for_each(d_matchers.begin(),d_matchers.end(),
			              [&ants](const Ptr & matcher) { matcher->SetUpOnce(ants); });
		}

		void SetUp(const IdentifiedFrame & identifiedFrame) override {
			std::for_each(d_matchers.begin(),d_matchers.end(),
			              [&](const Ptr & matcher) { matcher->SetUp(identifiedFrame); });

		}

		uint8_t Depth() const override {
			return d_depths.back();
		}


		uint64_t Match(fort::myrmidon::AntID ant1,
		               fort::myrmidon::AntID ant2,
		               const fort::myrmidon::InteractionTypes & type) override {
			uint64_t res = 0;
			size_t idx = 0;
			for ( const auto & m : d_matchers ) {
				auto d = d_depths[idx++];
				auto v = m->Match(ant1,ant2,type);
				if ( v == 0 ) {
					return 0;
				}
				res += v << d;
			}
			return res;
		}

		void Format(std::ostream & out ) const override {
			std::string prefix = "( ";
			for ( const auto & m : d_matchers ) {
				out << prefix;
				m->Format(out);
				prefix = " && ";
			}
			out << " )";
		}
	};

	return std::make_shared<AndMatcher>(matchers);
}

Matcher::Ptr Matcher::Or(const std::vector<Ptr> & matchers) {
	class OrMatcher : public Matcher {
	private:
		std::vector<Ptr> d_matchers;
		std::vector<uint8_t> d_depths;
	public:
		OrMatcher(const std::vector<Ptr> &  matchers)
			: d_matchers(matchers) {
			d_depths.push_back(0);
			for ( const auto & m : matchers) {
				d_depths.push_back(m->Depth() + d_depths.back());
			}
			if ( d_depths.back() > 64 ) {
				throw std::runtime_error("Maximal depth of this implementation is 64");
			}
		}
		virtual ~OrMatcher() {}
		void SetUpOnce(const AntByID & ants) override {
			std::for_each(d_matchers.begin(),d_matchers.end(),
			              [&ants](const Ptr & matcher) { matcher->SetUpOnce(ants); });
		}

		void SetUp(const IdentifiedFrame & identifiedFrame) override {
			std::for_each(d_matchers.begin(),d_matchers.end(),
			              [&](const Ptr & matcher) { matcher->SetUp(identifiedFrame); });
		}

		uint8_t Depth() const override {
			return d_depths.back();
		}


		uint64_t Match(fort::myrmidon::AntID ant1,
		               fort::myrmidon::AntID ant2,
		               const fort::myrmidon::InteractionTypes & types) override {
			uint64_t res = 0;
			size_t idx = 0;
			for ( const auto & m : d_matchers ) {
				auto d = d_depths[idx++];
				auto v = m->Match(ant1,ant2,types);
				res += v << d;
			}
			return res;
		}

		void Format(std::ostream & out ) const override {
			std::string prefix = "( ";
			for ( const auto & m : d_matchers ) {
				out << prefix;
				m->Format(out);
				prefix = " || ";
			}
			out << " )";
		}

	};
	return std::make_shared<OrMatcher>(matchers);
}

Matcher::Ptr Matcher::AntIDMatcher(AntID ID) {
	class AntIDMatcher : public Matcher {
	private:
		AntID d_id;
	public:
		AntIDMatcher (AntID ant)
			: d_id(ant) {
		}
		virtual ~AntIDMatcher() {}
		void SetUpOnce(const AntByID & ants) override {
		}

		void SetUp(const IdentifiedFrame & identifiedFrame) override {
		}

		uint64_t Match(fort::myrmidon::AntID ant1,
		               fort::myrmidon::AntID ant2,
		               const fort::myrmidon::InteractionTypes & types) override {
			if ( ant2 != 0 && ant2 == d_id ) {
				return 1;
			}
			return ant1 == d_id ? 1 : 0;
		}

		void Format(std::ostream & out ) const override {
			out << "Ant.ID == " << FormatAntID(d_id);
		}

	};
	return std::make_shared<AntIDMatcher>(ID);
}

Matcher::Ptr Matcher::AntColumnMatcher(const std::string & name, const Value & value) {
	class AntColumnMatcher : public Matcher {
	private:
		std::string d_name;
		Value    d_value;
		AntByID     d_ants;
		Time        d_time;
	public:
		AntColumnMatcher (const std::string & name,
		                  const Value & value)
			: d_name(name)
			, d_value(value){
		}
		virtual ~AntColumnMatcher() {}
		void SetUpOnce(const AntByID & ants) override {
			d_ants = ants;
		}

		void SetUp(const IdentifiedFrame & identifiedFrame) override {
			d_time = identifiedFrame.FrameTime;
		}

		uint64_t Match(fort::myrmidon::AntID ant1,
		               fort::myrmidon::AntID ant2,
		               const fort::myrmidon::InteractionTypes & types) override {
			auto fi = d_ants.find(ant2);
			if ( fi != d_ants.end()
			     && fi->second->GetValue(d_name,d_time) == d_value ) {
				return 1;
			}

			fi = d_ants.find(ant1);
			if ( fi == d_ants.end() ) {
				return 0;
			}
			return fi->second->GetValue(d_name,d_time) == d_value ? 1 : 0;
		}

		void Format(std::ostream & out ) const override {
			using fort::myrmidon::operator<<;
			out << "Ant.'" << d_name << "' == " << d_value;
		}

	};
	return std::make_shared<AntColumnMatcher>(name,value);
}


class AntGeometryMatcher : public Matcher {
protected:
	DenseMap<AntID,Eigen::Vector3d>          d_positions;
public:
	virtual ~AntGeometryMatcher(){}
	void SetUpOnce(const AntByID & ) override {}
	void SetUp(const IdentifiedFrame & identifiedFrame) override {
		d_positions.clear();
		for ( size_t i = 0; i < identifiedFrame.Positions.rows(); ++i) {
			d_positions.insert(std::make_pair(AntID(identifiedFrame.Positions(i,0)),
			                                  identifiedFrame.Positions.block<1,3>(i,1)));
		}
	}
};

class AntDistanceMatcher : public AntGeometryMatcher {
private:
	double                                   d_distanceSquare;
	bool                                     d_greater;
public:
	AntDistanceMatcher (double distance, bool greater)
		: d_distanceSquare(distance * distance)
		, d_greater(greater) {
	}
	virtual ~AntDistanceMatcher() {}
	uint64_t Match(fort::myrmidon::AntID ant1,
	               fort::myrmidon::AntID ant2,
	               const fort::myrmidon::InteractionTypes & types) override {
		auto fi1 = d_positions.find(ant1);
		auto fi2 = d_positions.find(ant2);
		if ( fi1 == d_positions.end() || fi2 == d_positions.end() ) {
			return 1;
		}
		double sDist = (fi1->second.block<2,1>(0,0) - fi2->second.block<2,1>(0,0)).squaredNorm();
		if ( d_greater == true ) {
			return d_distanceSquare < sDist ? 1 : 0;
		} else {
			return d_distanceSquare > sDist ? 1 : 0;
		}
	}

	void Format(std::ostream & out ) const override {
		out << "Distance(Ant1, Ant2) " << (d_greater == true ? ">" : "<" ) << " " << std::sqrt(d_distanceSquare);
	}
};


class AntAngleMatcher : public AntGeometryMatcher {
private:
	double                 d_angle;
	bool                   d_greater;
public:
	AntAngleMatcher (double angle, bool greater)
		: d_angle(Isometry2Dd(angle,{0,0}).angle())
		, d_greater(greater) {
	}
	virtual ~AntAngleMatcher() {}

	uint64_t Match(fort::myrmidon::AntID ant1,
	               fort::myrmidon::AntID ant2,
	               const fort::myrmidon::InteractionTypes & types) override {
		auto fi1 = d_positions.find(ant1);
		auto fi2 = d_positions.find(ant2);
		if ( fi1 == d_positions.end() || fi2 == d_positions.end() ) {
			return 1;
		}
		double angle = std::abs(fi1->second.z() - fi2->second.z());
		if ( d_greater == true ) {
			return angle > d_angle ? 1 : 0;
		} else {
			return angle < d_angle ? 1 : 0;
		}
	};

	void Format(std::ostream & out ) const override {
		out << "Angle(Ant1, Ant2) " << (d_greater == true ? ">" : "<" ) << " " << d_angle;
	}


};


class InteractionTypeSingleMatcher : public Matcher {
private:
	AntShapeTypeID d_type;
public:
	InteractionTypeSingleMatcher (AntShapeTypeID type)
		: d_type(type) {
	}
	virtual ~InteractionTypeSingleMatcher() {}
	void SetUpOnce(const AntByID & ants) override {
	}

	void SetUp(const IdentifiedFrame & identifiedFrame) override {
	}

	uint64_t Match(fort::myrmidon::AntID ant1,
	               fort::myrmidon::AntID ant2,
	               const fort::myrmidon::InteractionTypes & types) override {
		if (ant2 == 0) { return 1; }
		for ( size_t i = 0; i < types.rows(); ++i ) {
			if ( types.row(i) == Eigen::Matrix<uint32_t,1,2>(d_type,d_type) ) {
				return 1;
			}
		}
		return 0;
	}

	void Format(std::ostream & out ) const override {
		out << "InteractionType(" << d_type << " - " << d_type << ")";
	}
};

class InteractionTypeDualMatcher : public Matcher {
private:
	AntShapeTypeID d_type1,d_type2;
public:
	InteractionTypeDualMatcher(AntShapeTypeID type1,AntShapeTypeID type2) {
		if ( type1 < type2 ) {
			d_type1 = type1;
			d_type2 = type2;
		} else if ( type1 > type2 ) {
			d_type1 = type2;
			d_type2 = type1;
		} else {
			throw std::runtime_error("type1 must be different than type2");
		}
	}
	virtual ~InteractionTypeDualMatcher() {}
	void SetUpOnce(const AntByID & ants) override {
	}

	void SetUp(const IdentifiedFrame & identifiedFrame) override {
	}

	uint64_t Match(fort::myrmidon::AntID ant1,
	               fort::myrmidon::AntID ant2,
	               const fort::myrmidon::InteractionTypes & types) override {
		if (ant2 == 0) { return 1; }
		for ( size_t i = 0; i < types.rows(); ++i ) {
			if ( types.row(i) == Eigen::Matrix<uint32_t,1,2>(d_type1,d_type2)
			     || types.row(i) == Eigen::Matrix<uint32_t,1,2>(d_type2,d_type1)) {
				return 1;
			}
		}

		return 0;
	}

	void Format(std::ostream & out ) const override {
		out << "InteractionType(" << d_type1 << " - " << d_type2 << ")";
	}
};


Matcher::Ptr Matcher::AntDistanceSmallerThan(double distance) {
	return std::make_shared<AntDistanceMatcher>(distance,false);
}

Matcher::Ptr Matcher::AntDistanceGreaterThan(double distance) {
	return std::make_shared<AntDistanceMatcher>(distance,true);
}

Matcher::Ptr Matcher::AntAngleGreaterThan(double angle) {
	return std::make_shared<AntAngleMatcher>(angle,true);
}

Matcher::Ptr Matcher::AntAngleSmallerThan(double angle) {
	return std::make_shared<AntAngleMatcher>(angle,false);
}

Matcher::Ptr Matcher::InteractionType(AntShapeTypeID type1,
                                      AntShapeTypeID type2) {
	if ( type1 != type2 ) {
		return std::make_shared<InteractionTypeDualMatcher>(type1,type2);
	}
	return std::make_shared<InteractionTypeSingleMatcher>(type1);
}

class AntDisplacementMatcher : public Matcher {
public :
	AntDisplacementMatcher(double under,Duration minimumGap)
		: d_under(under)
		, d_under2(under*under)
		, d_minimumGap(minimumGap) {
	}
	virtual ~AntDisplacementMatcher() {}

	void SetUpOnce(const AntByID & ants) override {
	}

	void SetUp(const IdentifiedFrame & identifiedFrame) override {
		for ( size_t i = 0; i < identifiedFrame.Positions.rows(); ++i) {
			AntID antID = identifiedFrame.Positions(i,0);
			auto [fi,inserted] = d_displacements.insert(std::make_pair(antID,Displacement()));
			fi->second.Update(identifiedFrame.Space,
			                  identifiedFrame.FrameTime,
			                  identifiedFrame.Positions.block<1,2>(i,1).transpose(),
			                  d_minimumGap);
		}
	}

	uint64_t Match(fort::myrmidon::AntID ant1,
	               fort::myrmidon::AntID ant2,
	               const fort::myrmidon::InteractionTypes & types) override {
		if ( d_displacements.count(ant1) != 0
		     && d_displacements.at(ant1).Displacement2 > d_under2 ) {
			return 0;
		}
		if ( d_displacements.count(ant2) != 0
		     && d_displacements.at(ant2).Displacement2 > d_under2 ) {
			return 0;
		}

		return 1;
	}

	void Format(std::ostream & out) const override {
		out << "AntDisplacement(under: " << d_under
		    << ", minimumGap: " << d_minimumGap << ")";
	}

private:
	double   d_under,d_under2;
	Duration d_minimumGap;

	struct Displacement {
		double          Displacement2;
		Eigen::Vector2d Position;
		SpaceID         Space;
		fort::Time      At;
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW;

		Displacement()
			: Space(0) {
		}

		void Update(SpaceID spaceID,const fort::Time & time,const Eigen::Vector2d & position, Duration minimumGap) {
			auto gap = time.Sub(At);
			if ( Space != spaceID || gap <= minimumGap ) {
				Displacement2 = 0.0;
				Space = spaceID;
			} else {
				Displacement2 = (position - Position).squaredNorm();
			}
			Position = position;
			At = time;
		}
	};



	DenseMap<AntID,Displacement> d_displacements;

};

Matcher::Ptr Matcher::AntDisplacement(double under,
                             Duration minimumGap) {
	return std::make_shared<AntDisplacementMatcher>(under,minimumGap);
}

std::ostream & operator<<(std::ostream & out, const fort::myrmidon::priv::Matcher & m) {
	m.Format(out);
	return out;
}

} // namespace priv
} // namespace myrmidon
} // namespace fort
