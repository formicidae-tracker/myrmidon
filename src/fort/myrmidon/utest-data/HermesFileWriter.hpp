#pragma once

#include <fort/hermes/Header.pb.h>

#include <fort/myrmidon/utils/FileSystem.hpp>

#include "SegmentedDataWriter.hpp"
#include "Config.hpp"

namespace google {
namespace protobuf {
namespace io {
class FileOutputStream;
class GzipOutputStream;
} // namespace io
} // namespace protobuf
} // namespace google


namespace fort {
namespace myrmidon {

class HermesFileWriter : public SegmentedDataWriter {
public:
	static std::string HermesFileName(size_t i );

	HermesFileWriter(const fs::path & basepath,
	                 const Config & config);
	virtual ~HermesFileWriter();

	void Prepare(size_t index) override;
	void WriteFrom(const IdentifiedFrame & data,
	               uint64_t frameID) override;

	void Finalize(size_t index,bool last) override;
	void FillReadout(hermes::FrameReadout * ro,
	                 uint64_t frameID,
	                 const IdentifiedFrame & identified);
private:
	fs::path         d_basepath;
	hermes::FileLine d_line;
	std::unique_ptr<google::protobuf::io::FileOutputStream> d_file;
	std::unique_ptr<google::protobuf::io::GzipOutputStream> d_gzipped;
	Config d_config;
};


} // namespace fort
} // namespace myrmidon
