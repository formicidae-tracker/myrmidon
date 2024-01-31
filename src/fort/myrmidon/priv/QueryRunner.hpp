#pragma once

#include <functional>
#include <utility>

#include <fort/time/Time.hpp>

#include <fort/myrmidon/types/Collision.hpp>
#include <fort/myrmidon/types/Reporter.hpp>
#include <fort/myrmidon/types/Typedefs.hpp>

#include "ForwardDeclaration.hpp"

namespace fort {
namespace myrmidon {
namespace priv {

class Experiment;

class QueryRunner {
public:
	struct RawData {
		SpaceID          Space;
		RawFrameConstPtr Frame;
		uint64_t         ID;
	};

	struct Args {
		Time                  Start;
		Time                  End;
		bool                  Localize;
		bool                  Collide;
		bool                  CollisionsIgnoreZones;
		TimeProgressReporter *Progress;
	};

	using OrderedCollisionData =
	    std::tuple<uint64_t, IdentifiedFrame::Ptr, CollisionFrame::Ptr>;

	using Computer = std::function<OrderedCollisionData(const RawData &)>;

	using Finalizer = std::function<void(const OrderedCollisionData &data)>;

	using Runner =
	    std::function<void(const Experiment &, const Args &, Finalizer)>;

	static void RunMultithread(
	    const Experiment &experiment, const Args &args, Finalizer finalizer
	);

	static void RunSingleThread(
	    const Experiment &experiment, const Args &args, Finalizer finalizer
	);

	static Runner RunnerFor(bool multithread);

private:
	static std::function<OrderedCollisionData(const RawData &)>
	computeData(const Experiment &experiment, const Args &args);
};

} // namespace priv
} // namespace myrmidon
} // namespace fort
