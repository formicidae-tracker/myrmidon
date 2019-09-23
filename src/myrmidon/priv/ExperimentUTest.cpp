#include "ExperimentUTest.hpp"

#include "Experiment.hpp"

#include "../utils/PosixCall.h"

#include <google/protobuf/util/delimited_message_util.h>
#include <google/protobuf/util/time_util.h>
#include <google/protobuf/io/gzip_stream.h>
#include <fort-hermes/Header.pb.h>

#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include <iterator>
#include <fstream>

#ifndef O_BINARY
#define O_BINARY 0
#endif


namespace fm= fort::myrmidon;
namespace fs = std::filesystem;
using namespace fm::priv;


void ReadAll(const fs::path & a, std::vector<uint8_t> & data) {
	data.clear();
	data.reserve(fs::file_size(a));
	std::ifstream f(a.c_str(),std::ios::binary);
	data =  std::vector<uint8_t>(std::istreambuf_iterator<char>(f),{});
}

TEST_F(ExperimentUTest,IOTest) {
	try{
		auto e = Experiment::Open(s_testdir / "test.myrmidon" );
		auto tdd = e->TrackingDataPaths();
		ASSERT_EQ(tdd.size(),1);
		ASSERT_EQ(tdd["foo.0000"].Path,"foo.0000");
		ASSERT_EQ(e->Ants().size(),3);
		EXPECT_EQ(e->Ants().find(1)->second->ID(),1);
		EXPECT_EQ(e->Ants().find(2)->second->ID(),2);
		EXPECT_EQ(e->Ants().find(3)->second->ID(),3);

		e->Save(s_testdir / "test2.myrmidon");
	} catch (const std::exception & e) {
		ADD_FAILURE() << "Got unexpected exception: " << e.what();
	}

	try {
		std::vector<uint8_t> originalData,newData;
		ReadAll(s_testdir/"test.myrmidon",originalData);
		ReadAll(s_testdir/"test2.myrmidon",newData);
		ASSERT_EQ(newData.size(),originalData.size());
		for(size_t i = 0; i < newData.size(); ++i) {
			ASSERT_EQ(newData[i],originalData[i]) << "At byte " << i;
		}
	} catch ( const std::exception & e) {
		ADD_FAILURE() << "Got unexpected exception: " << e.what();
	}

}

TEST_F(ExperimentUTest,AndsAreCreatedSequentially) {
	Experiment::Ptr e;

	try{
		e = Experiment::Open(s_testdir / "test.myrmidon" );
		size_t startSize = e->Ants().size();
		auto ant = e->CreateAnt();
		ASSERT_EQ(ant->ID(),e->Ants().size());
		ant = e->CreateAnt();
		ASSERT_EQ(ant->ID(),e->Ants().size());
		ant = e->CreateAnt();
		ASSERT_EQ(ant->ID(),e->Ants().size());
		ASSERT_EQ(e->Ants().size(),startSize + 3);

		e->DeleteAnt(2);
		e->DeleteAnt(5);

		ant = e->CreateAnt();
		ASSERT_EQ(ant->ID(),2);

		ant = e->CreateAnt();
		ASSERT_EQ(ant->ID(),5);

		ant = e->CreateAnt();
		ASSERT_EQ(ant->ID(),7);

	} catch ( const std::exception & e) {
		ADD_FAILURE() << "Got unexpected exception: " << e.what();
	}
}

TEST_F(ExperimentUTest,TestNewTrackingDataDirectories) {
	try {
		auto e = Experiment::Open(s_testdir / "test.myrmidon");
		e->AddTrackingDataDirectory(s_testdir/"foo.0001");
	} catch( const std::exception & e) {
		ADD_FAILURE() << "Got unexpected exception: " << e.what();
	}

}


std::filesystem::path ExperimentUTest::s_testdir;

void ExperimentUTest::SetUpTestCase() {
	std::ostringstream os;
	os << "myrmidon-tests-" << getpid();


	EXPECT_NO_THROW({
			auto tmppath = std::filesystem::temp_directory_path() / os.str();
			std::filesystem::create_directories(tmppath / "foo.0000");
			std::filesystem::create_directories(tmppath / "foo.0001");
			s_testdir = tmppath;

			//creates data
			fm::pb::Experiment e;

			auto p = e.add_datadirectory();
			p->set_path("foo.0000");
			p->set_startframe(100);
			p->set_endframe(105);
			auto startDate = p->mutable_startdate();
			auto endDate = p->mutable_enddate();
			ASSERT_EQ(google::protobuf::util::TimeUtil::FromString("1972-01-01T10:00:20.021-05:00",startDate),true);
			ASSERT_EQ(google::protobuf::util::TimeUtil::FromString("1972-01-01T10:00:21.271-05:00",endDate),true);

			fm::pb::FileHeader header;

			header.set_majorversion(0);
			header.set_minorversion(1);
			fm::pb::FileLine l;

			auto myrmidonFile = s_testdir / "test.myrmidon";
			int fd = open(myrmidonFile.c_str(),O_CREAT | O_TRUNC | O_RDWR | O_BINARY,0644 );
			if ( fd <= 0 ) {
				throw std::system_error(errno,MYRMIDON_SYSTEM_CATEGORY(),"open('" + myrmidonFile.string() + "',O_RDONLY | O_BINARY)");
			}
			auto file = std::make_shared<google::protobuf::io::FileOutputStream>(fd);
			file->SetCloseOnDelete(true);
			auto gunziped = std::make_shared<google::protobuf::io::GzipOutputStream>(file.get());

			if (!google::protobuf::util::SerializeDelimitedToZeroCopyStream(header, gunziped.get()) ) {
				throw std::runtime_error("could not write header message");
			}

			l.set_allocated_experiment(&e);
			if (!google::protobuf::util::SerializeDelimitedToZeroCopyStream(l, gunziped.get()) ) {
				throw std::runtime_error("could not write experiment data");
			}
			l.release_experiment();

			for (size_t i = 1; i <=3; ++i) {
				fort::myrmidon::pb::AntMetadata a;
				a.set_id(i);
				l.set_allocated_antdata(&a);
				if (!google::protobuf::util::SerializeDelimitedToZeroCopyStream(l, gunziped.get()) ) {
					throw std::runtime_error("could not write ant data 1");
				}
				l.release_antdata();
			}

			//creates fake tracking data

			fort::hermes::Header hHeader;
			auto v = hHeader.mutable_version();
			v->set_vmajor(0);
			v->set_vminor(1);
			hHeader.set_type(fort::hermes::Header::Type::Header_Type_File);
			hHeader.set_width(640);
			hHeader.set_height(480);
			fort::hermes::FileLine hLineRo;
			fort::hermes::FileLine hLineFooter;
			auto hRo = hLineRo.mutable_readout();
			auto hFooter = hLineFooter.mutable_footer();


			fd = open( (s_testdir / "foo.0001/tracking.0000.hermes").c_str(),
			          O_CREAT | O_TRUNC | O_RDWR | O_BINARY,
			          0644);

			if ( fd <= 0 ) {
				throw std::system_error(errno,MYRMIDON_SYSTEM_CATEGORY(),"open('"
				                        + (s_testdir / "foo.0001/tracking.0000.hermes").string()
				                        + "',O_RDONLY | O_BINARY)");
			}

			auto fileT1 = std::make_shared<google::protobuf::io::FileOutputStream>(fd);
			file->SetCloseOnDelete(true);
			auto gunzipedT1 = std::make_shared<google::protobuf::io::GzipOutputStream>(fileT1.get());

			if (!google::protobuf::util::SerializeDelimitedToZeroCopyStream(hHeader, gunzipedT1.get()) ) {
				throw std::runtime_error("could not write readout message");
			}

			auto time = hRo->mutable_time();
			ASSERT_EQ(google::protobuf::util::TimeUtil::FromString("1972-01-01T10:00:22.271-05:00",time),true);
			hRo->set_frameid(5);
			hRo->set_timestamp(0);
			if (!google::protobuf::util::SerializeDelimitedToZeroCopyStream(hLineRo, gunzipedT1.get()) ) {
				throw std::runtime_error("could not write readout message");
			}
			ASSERT_EQ(google::protobuf::util::TimeUtil::FromString("1972-01-01T10:00:23.271-05:00",time),true);
			hRo->set_frameid(6);
			hRo->set_timestamp(120);
			if (!google::protobuf::util::SerializeDelimitedToZeroCopyStream(hLineRo, gunzipedT1.get()) ) {
				throw std::runtime_error("could not write readout message");
			}
			hFooter->set_next("tracking.0001.hermes");
			if (!google::protobuf::util::SerializeDelimitedToZeroCopyStream(hLineFooter, gunzipedT1.get()) ) {
				throw std::runtime_error("could not write footer message");
			}


			fd = open( (s_testdir / "foo.0001/tracking.0001.hermes").c_str(),
			           O_CREAT | O_TRUNC | O_RDWR | O_BINARY,
			           0644);

			if ( fd <= 0 ) {
				throw std::system_error(errno,MYRMIDON_SYSTEM_CATEGORY(),"open('"
				                        + (s_testdir / "foo.0001/tracking.0001.hermes").string()
				                        + "',O_RDONLY | O_BINARY)");
			}

			auto fileT2 = std::make_shared<google::protobuf::io::FileOutputStream>(fd);
			file->SetCloseOnDelete(true);
			auto gunzipedT2 = std::make_shared<google::protobuf::io::GzipOutputStream>(fileT2.get());

			hHeader.set_previous("tracking.0000.hermes");
			if (!google::protobuf::util::SerializeDelimitedToZeroCopyStream(hHeader, gunzipedT2.get()) ) {
				throw std::runtime_error("could not write readout message");
			}

			ASSERT_EQ(google::protobuf::util::TimeUtil::FromString("1972-01-01T10:00:24.271-05:00",time),true);
			hRo->set_frameid(7);
			hRo->set_timestamp(0);
			if (!google::protobuf::util::SerializeDelimitedToZeroCopyStream(hLineRo, gunzipedT2.get()) ) {
				throw std::runtime_error("could not write readout message");
			}
			ASSERT_EQ(google::protobuf::util::TimeUtil::FromString("1972-01-01T10:00:25.271-05:00",time),true);
			hRo->set_frameid(8);
			hRo->set_timestamp(120);
			if (!google::protobuf::util::SerializeDelimitedToZeroCopyStream(hLineRo, gunzipedT2.get()) ) {
				throw std::runtime_error("could not write readout message");
			}

			hFooter->Clear();
			if (!google::protobuf::util::SerializeDelimitedToZeroCopyStream(hLineFooter, gunzipedT2.get()) ) {
				throw std::runtime_error("could not write footer message");
			}

		});
	ASSERT_NE(fs::file_size(s_testdir / "test.myrmidon"),0);
}

void ExperimentUTest::TearDownTestCase() {
	if ( s_testdir.empty() ) {
		return;
	}
	EXPECT_NO_THROW({
			//std::filesystem::remove_all(s_testdir);
		});
}
