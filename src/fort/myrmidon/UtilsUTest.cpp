#include "UtilsUTest.hpp"

#include <google/protobuf/util/message_differencer.h>

#include <Eigen/Geometry>

#define failure_helper(aExpr,bExpr,a,b,field) \
	::testing::AssertionFailure() \
	<< "Value of: " << aExpr << "." << #field << std::endl \
	<< "  Actual: " << a.field << std::endl \
	<< "Expected: " << bExpr << "." << #field << std::endl \
	<< "Which is: " << b.field

::testing::AssertionResult AssertTimeEqual(const char * aExpr,
                                           const char * bExpr,
                                           const fort::Time & a,
                                           const fort::Time & b) {
	if ( a.Equals(b) == false ) {
		return ::testing::AssertionFailure() << "Value of: " << aExpr <<".Equals(" << bExpr << ")" << std::endl
		                                     << "  Actual: false" << std::endl
		                                     << "Expected: true";
	}

	if ( google::protobuf::util::MessageDifferencer::Equals(a.ToTimestamp(),b.ToTimestamp()) == false ) {
		return ::testing::AssertionFailure() << "Value of: " << aExpr << ".Timestamp()" << std::endl
		                                     << "  Actual: " << a.ToTimestamp().DebugString() << std::endl
		                                     << "Expected: " << bExpr << ".Timestamp()" << std::endl
		                                     << "Which is: " << b.ToTimestamp().DebugString();
	}

	if ( a.HasMono() != b.HasMono() ) {
		return ::testing::AssertionFailure() << "Value of: " << aExpr << ".HasMono()" << std::endl
		                                     << "  Actual: " << std::boolalpha << a.HasMono() << std::endl
		                                     << "Expected: " << bExpr << ".HasMono()" << std::endl
		                                     << "Which is: " << std::boolalpha << b.HasMono();
	}

	if ( a.HasMono() == false ) {
		return ::testing::AssertionSuccess();
	}


	if ( a.MonoID() != b.MonoID() ) {
		return failure_helper(aExpr,bExpr,a,b,MonoID());
	}

	if ( a.MonotonicValue() != b.MonotonicValue() ) {
		return failure_helper(aExpr,bExpr,a,b,MonotonicValue());
	}

	return ::testing::AssertionSuccess();
}



::testing::AssertionResult AssertVectorAlmostEqual(const char * aExpr,
                                                   const char * bExpr,
                                                   const Eigen::Vector2d & a,
                                                   const Eigen::Vector2d & b) {



	auto xAssertion = ::testing::internal::CmpHelperFloatingPointEQ<double>((std::string(aExpr)+".x()").c_str(),
		  (std::string(bExpr)+".x()").c_str(),
		  a.x(),
		  b.x());
	if ( !xAssertion ) {
		return xAssertion;
	}
	return  ::testing::internal::CmpHelperFloatingPointEQ<double>((std::string(aExpr)+".y()").c_str(),
		  (std::string(bExpr)+".y()").c_str(),
		  a.y(),
		  b.y());
  }

::testing::AssertionResult AssertMessageEqual(const char * aExpr,
                                              const char * bExpr,
                                              const google::protobuf::Message &a,
                                              const google::protobuf::Message &b) {
	std::string differences;

	google::protobuf::util::MessageDifferencer diff;
	diff.set_float_comparison(google::protobuf::util::MessageDifferencer::APPROXIMATE);
	diff.ReportDifferencesToString(&differences);
	if ( diff.Compare(b,a) == true ) {
		return ::testing::AssertionSuccess();
	}
	return ::testing::AssertionFailure() << "Value Of: " << aExpr << std::endl
	                                     << "  Actual: " << a.DebugString() << std::endl
	                                     << "Expected: " << bExpr << std::endl
	                                     << "Which is: " << b.DebugString();
}


::testing::AssertionResult AssertPolygonEqual(const char * aExpr,
                                              const char * bExpr,
                                              const fort::myrmidon::Polygon &a,
                                              const fort::myrmidon::Polygon &b) {
	if ( a.Size() != b.Size() ) {
		return failure_helper(aExpr,bExpr,a,b,Size());
	}
	for ( size_t i = 0; i < a.Size(); ++i ) {
		std::ostringstream as,bs;
		as << aExpr << ".Vertex(" << i << ")";
		bs << bExpr << ".Vertex(" << i << ")";
		auto inter = AssertVectorAlmostEqual(as.str().c_str(),
		                                     bs.str().c_str(),
		                                     a.Vertex(i),
		                                     b.Vertex(i));
		if ( !inter ) {
			return inter;
		}
	}
	return ::testing::AssertionSuccess();
}


testing::AssertionResult AssertCapsuleEqual(const char * aExpr,
                                            const char * bExpr,
                                            const fort::myrmidon::Capsule &a,
                                            const fort::myrmidon::Capsule &b) {
	auto inter = AssertVectorAlmostEqual((std::string(aExpr)+".C1()").c_str(),
	                                     (std::string(bExpr)+".C1()").c_str(),
	                                     a.C1(),
	                                     b.C1());
	if ( !inter ) {
		return inter;
	}
	inter = AssertVectorAlmostEqual((std::string(aExpr)+".C2()").c_str(),
	                                (std::string(bExpr)+".C2()").c_str(),
	                                a.C2(),
	                                b.C2());
	if ( !inter ) {
		return inter;
	}
	inter = ::testing::internal::CmpHelperFloatingPointEQ<double>((std::string(aExpr)+".R1()").c_str(),
	                                                              (std::string(bExpr)+".R1()").c_str(),
	                                                              a.R1(),
	                                                              b.R1());
	if ( !inter ) {
		return inter;
	}
	return ::testing::internal::CmpHelperFloatingPointEQ<double>((std::string(aExpr)+".R2()").c_str(),
	                                                             (std::string(bExpr)+".R2()").c_str(),
	                                                             a.R2(),
	                                                             b.R2());
}


testing::AssertionResult AssertCircleEqual(const char * aExpr,
                                           const char * bExpr,
                                           const fort::myrmidon::Circle &a,
                                           const fort::myrmidon::Circle &b) {
	auto inter = AssertVectorAlmostEqual((std::string(aExpr)+".Center()").c_str(),
	                                     (std::string(bExpr)+".Center()").c_str(),
	                                     a.Center(),
	                                     b.Center());
	if ( !inter ) {
		return inter;
	}
	return ::testing::internal::CmpHelperFloatingPointEQ<double>((std::string(aExpr)+".Radius()").c_str(),
	                                                             (std::string(bExpr)+".Radius()").c_str(),
	                                                             a.Radius(),
	                                                             b.Radius());
}



testing::AssertionResult AssertShapeEqual(const char * aExpr,
                                          const char * bExpr,
                                          const fort::myrmidon::Shape & a,
                                          const fort::myrmidon::Shape & b) {
	if ( a.ShapeType() != b.ShapeType() ) {
		return ::testing::AssertionFailure() << "Value Of: " << aExpr << ".ShapeType()" << std::endl
		                                     << "  Actual: " << int(a.ShapeType()) << std::endl
		                                     << "Expected: " << bExpr << ".ShapeType()" << std::endl
		                                     << "Which is: " << int(b.ShapeType());
	}

	switch(a.ShapeType()) {
	case fort::myrmidon::Shape::Type::CAPSULE: {
		return AssertCapsuleEqual(aExpr,
		                          bExpr,
		                          static_cast<const fort::myrmidon::Capsule &>(a),
		                          static_cast<const fort::myrmidon::Capsule &>(b));

	}
	case fort::myrmidon::Shape::Type::CIRCLE: {
		return AssertCircleEqual(aExpr,
		                         bExpr,
		                         static_cast<const fort::myrmidon::Circle &>(a),
		                         static_cast<const fort::myrmidon::Circle &>(b));
	}
	case fort::myrmidon::Shape::Type::POLYGON: {
		return AssertPolygonEqual(aExpr,
		                          bExpr,
		                          static_cast<const fort::myrmidon::Polygon &>(a),
		                          static_cast<const fort::myrmidon::Polygon &>(b));
	}
	}
	return ::testing::AssertionFailure() << "unsupported shape type";
}

::testing::AssertionResult AssertAntStaticValueEqual(const char * aExpr,
                                                     const char * bExpr,
                                                     const fort::myrmidon::AntStaticValue &a,
                                                     const fort::myrmidon::AntStaticValue &b) {
	if ( a.index() != b.index() ) {
		return failure_helper(aExpr,bExpr,a,b,index());
	}

	switch( a.index() ) {
	case 0:
		if ( std::get<bool>(a) != std::get<bool>(b) ) {
			return ::testing::AssertionFailure() << "Value Of: std::get<bool>(" << aExpr << ")" << std::endl
			                                     << "  Actual: " << std::boolalpha << std::get<bool>(a) << std::endl
			                                     << "Expected: std::get<bool>(" << bExpr << ")" << std::endl
			                                     << "Which is: " << std::get<bool>(b);
		}
		break;
	case 1:
		if ( std::get<int>(a) != std::get<int>(b) ) {
			return ::testing::AssertionFailure() << "Value Of: std::get<int>(" << aExpr << ")" << std::endl
			                                     << "  Actual: "  << std::get<int>(a) << std::endl
			                                     << "Expected: std::get<int>(" << bExpr << ")" << std::endl
			                                     << "Which is: " << std::get<int>(b);

		}
		break;
	case 2:
		if ( std::get<double>(a) != std::get<double>(b) ) {
			return ::testing::AssertionFailure() << "Value Of: std::get<double>(" << aExpr << ")" << std::endl
			                                     << "  Actual: " << std::get<double>(a) << std::endl
			                                     << "Expected: std::get<double>(" << bExpr << ")" << std::endl
			                                     << "Which is: " << std::get<double>(b);
		}
		break;
	case 3:
		if ( std::get<std::string>(a) != std::get<std::string>(b) ) {
			return ::testing::AssertionFailure() << "Value Of: std::get<std::string>(" << aExpr << ")" << std::endl
			                                     << "  Actual: " << std::get<std::string>(a) << std::endl
			                                     << "Expected: std::get<std::string>(" << bExpr << ")" << std::endl
			                                     << "Which is: " << std::get<std::string>(b);

		}
		break;
	case 4:
		if ( std::get<fort::Time>(a).Equals(std::get<fort::Time>(b)) == false ) {
			return ::testing::AssertionFailure() << "Value Of: std::get<fort::Time>(" << aExpr << ")" << std::endl
			                                     << "  Actual: " << std::get<fort::Time>(a) << std::endl
			                                     << "Expected: std::get<fort::Time>(" << bExpr << ")" << std::endl
			                                     << "Which is: " << std::get<fort::Time>(b);

		}
		break;
	default:
		return ::testing::AssertionFailure() << "Unknown type index: " << a.index();
	}
	return ::testing::AssertionSuccess();
}

::testing::AssertionResult
AssertAABBAlmostEqual(const char * aExpr,
                      const char * bExpr,
                      const fort::myrmidon::AABB & a,
                      const fort::myrmidon::AABB & b) {
	auto min = AssertVectorAlmostEqual((std::string(aExpr)+".min()").c_str(),
	                                   (std::string(bExpr)+".min()").c_str(),
	                                   a.min(),
	                                   b.min());
	if ( !min == true ) {
		return min;
	}
	return AssertVectorAlmostEqual((std::string(aExpr)+".max()").c_str(),
	                               (std::string(bExpr)+".max()").c_str(),
	                               a.max(),
	                               b.max());
}


::testing::AssertionResult
AssertSingleStatsEqual(const char * aExpr,
                       const char * bExpr,
                       const fort::myrmidon::TagStatistics & a,
                       const fort::myrmidon::TagStatistics & b) {
		if ( a.ID != b.ID ) {
			return failure_helper(aExpr,bExpr,a,b,ID);
		}

		auto intermediary = AssertTimeEqual((std::string(aExpr)+".FirstSeen").c_str(),
		                                    (std::string(bExpr)+".FirstSeen").c_str(),
		                                    a.FirstSeen,
		                                    b.FirstSeen);
		if ( !intermediary ) {
			return intermediary;
		}
		intermediary = AssertTimeEqual((std::string(aExpr)+".LastSeen").c_str(),
		                               (std::string(bExpr)+".LastSeen").c_str(),
		                               a.LastSeen,
		                               b.LastSeen);
		if ( !intermediary ) {
			return intermediary;
		}

		if ( a.Counts.rows() != b.Counts.rows() ) {
			return failure_helper(aExpr,bExpr,a,b,Counts.rows());
		}

		for ( size_t i = 0; i < a.Counts.rows(); ++i) {
			if ( a.Counts(i) != b.Counts(i) ) {
				return ::testing::AssertionFailure() << "Value Of: " << aExpr << ".Counts" << std::endl
				                                     << "  Actual: " << a.Counts.transpose() << std::endl
				                                     << "Expected: " << bExpr << ".Counts" << std::endl
				                                     << "Which is: " << b.Counts.transpose() << std::endl
				                                     << "   Index: " << i;

			}
		}

		return ::testing::AssertionSuccess();
}


::testing::AssertionResult
AssertTagStatisticsEqual(const char * aExpr,
                         const char * bExpr,
                         const fort::myrmidon::TagStatistics::ByTagID & a,
                         const fort::myrmidon::TagStatistics::ByTagID & b) {
	if ( a.size() != b.size() ) {
		return failure_helper(aExpr,bExpr,a,b,size());
	}
	for ( const auto & [tagID,aStats] : a ) {
		if ( b.count(tagID) == 0 ) {
			return ::testing::AssertionFailure() << bExpr << " is missing TagID " << tagID;
		}
		const auto & bStats = b.at(tagID);

		auto intermediary = AssertSingleStatsEqual((std::string(aExpr)+"["+fort::myrmidon::FormatTagID(tagID)+"]").c_str(),
		                                           (std::string(bExpr)+"["+fort::myrmidon::FormatTagID(tagID)+"]").c_str(),
		                                            aStats,
		                                            bStats);
		if ( !intermediary ) {
			return intermediary;
		}
	}
	return ::testing::AssertionSuccess();
}

::testing::AssertionResult
AssertIdentifiedFrameEqual(const char * aExpr,
                           const char * bExpr,
                           const fort::myrmidon::IdentifiedFrame & a,
                           const fort::myrmidon::IdentifiedFrame & b) {
	auto intermediary = AssertTimeEqual((std::string(aExpr)+".FrameTime").c_str(),
	                                    (std::string(bExpr)+".FrameTime").c_str(),
	                                    a.FrameTime,
	                                    b.FrameTime);
	if ( !intermediary ) {
		return intermediary;
	}
	if ( a.Space != b.Space ) {
		return failure_helper(aExpr,bExpr,a,b,Space);
	}
	if ( a.Height != b.Height ) {
		return failure_helper(aExpr,bExpr,a,b,Height);
	}

	if ( a.Width != b.Width ) {
		return failure_helper(aExpr,bExpr,a,b,Width);
	}

	if ( a.Positions.rows() != b.Positions.rows() ) {
		return failure_helper(aExpr,bExpr,a,b,Positions.rows());
	}

	for ( size_t i = 0; i < b.Positions.rows(); ++i ) {
		fort::myrmidon::AntID antID = b.Positions(i,0);
		size_t j = 0;
		for ( ; j < a.Positions.rows(); ++j) {
			if (a.Positions(j,0) == antID) {
				break;
			}
		}
		if ( j == a.Positions.rows() ) {
			return ::testing::AssertionFailure() << "Could not find expected AntID " << antID << " in frame";
		}
		for ( const auto & c : {1,2,3,4}) {
			if ( std::abs(a.Positions(j,c) - b.Positions(i,c)) > 1e-3 ) {
				return ::testing::AssertionFailure()
					<< "Value of: " << aExpr << ".Positions(" << j << "," << c << ")" << std::endl
					<< "  Actual: " << a.Positions(j,c) << std::endl
					<< "  Within: " << 1e-3 << std::endl
					<< "      of: " << bExpr << ".Positions(" << i << "," << c << ")" << std::endl
					<< "Which is: " << b.Positions(j,c);
			}
		}
	}

	return ::testing::AssertionSuccess();
}
