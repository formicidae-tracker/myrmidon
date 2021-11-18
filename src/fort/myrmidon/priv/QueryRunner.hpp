#pragma once

#include <utility>
#include <functional>

#include <fort/time/Time.hpp>

#include <fort/myrmidon/types/Typedefs.hpp>
#include <fort/myrmidon/types/Collision.hpp>

#include "ForwardDeclaration.hpp"


namespace fort {
namespace myrmidon {
namespace priv {

class Experiment;

class QueryRunner {
public:
	typedef std::pair<SpaceID,RawFrameConstPtr> RawData;

	struct Args {
		Time Start;
		Time End;
		bool Localize;
		bool Collide;
		bool CollisionsIgnoreZones;
	};

	typedef std::function<void (const CollisionData & data)> Finalizer;

	typedef std::function<void (const Experiment &,const Args &,Finalizer)> Runner;

	static void RunMultithread(const Experiment & experiment,
							   const Args & args,
							   Finalizer finalizer);

	static void RunMultithreadFinalizeInCurrent(const Experiment & experiment,
												const Args & args,
												Finalizer finalizer);


	static void RunSingleThread(const Experiment & experiment,
								const Args & args,
								Finalizer finalizer);



	static Runner RunnerFor(bool multithread,bool finalizerInCurrentThread);





private:


	static std::function<CollisionData(const RawData &)> computeData(const Experiment & experiment,
																			const Args & args);

};

} // namespace priv
} // namespace myrmidon
} // namespace fort
