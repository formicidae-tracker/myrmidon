// Code generated by protoc-gen-go. DO NOT EDIT.
// versions:
// 	protoc-gen-go v1.24.0-devel
// 	protoc        v3.6.1
// source: ExperimentFile.proto

package fort_myrmidon_pb

import (
	proto "github.com/golang/protobuf/proto"
	protoreflect "google.golang.org/protobuf/reflect/protoreflect"
	protoimpl "google.golang.org/protobuf/runtime/protoimpl"
	reflect "reflect"
	sync "sync"
)

const (
	// Verify that this generated code is sufficiently up-to-date.
	_ = protoimpl.EnforceVersion(20 - protoimpl.MinVersion)
	// Verify that runtime/protoimpl is sufficiently up-to-date.
	_ = protoimpl.EnforceVersion(protoimpl.MaxVersion - 20)
)

// This is a compile-time assertion that a sufficiently up-to-date version
// of the legacy proto package is being used.
const _ = proto.ProtoPackageIsVersion4

type FileHeader struct {
	state         protoimpl.MessageState
	sizeCache     protoimpl.SizeCache
	unknownFields protoimpl.UnknownFields

	MajorVersion int32 `protobuf:"varint,1,opt,name=MajorVersion,proto3" json:"MajorVersion,omitempty"`
	MinorVersion int32 `protobuf:"varint,2,opt,name=MinorVersion,proto3" json:"MinorVersion,omitempty"`
}

func (x *FileHeader) Reset() {
	*x = FileHeader{}
	if protoimpl.UnsafeEnabled {
		mi := &file_ExperimentFile_proto_msgTypes[0]
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		ms.StoreMessageInfo(mi)
	}
}

func (x *FileHeader) String() string {
	return protoimpl.X.MessageStringOf(x)
}

func (*FileHeader) ProtoMessage() {}

func (x *FileHeader) ProtoReflect() protoreflect.Message {
	mi := &file_ExperimentFile_proto_msgTypes[0]
	if protoimpl.UnsafeEnabled && x != nil {
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		if ms.LoadMessageInfo() == nil {
			ms.StoreMessageInfo(mi)
		}
		return ms
	}
	return mi.MessageOf(x)
}

// Deprecated: Use FileHeader.ProtoReflect.Descriptor instead.
func (*FileHeader) Descriptor() ([]byte, []int) {
	return file_ExperimentFile_proto_rawDescGZIP(), []int{0}
}

func (x *FileHeader) GetMajorVersion() int32 {
	if x != nil {
		return x.MajorVersion
	}
	return 0
}

func (x *FileHeader) GetMinorVersion() int32 {
	if x != nil {
		return x.MinorVersion
	}
	return 0
}

type FileLine struct {
	state         protoimpl.MessageState
	sizeCache     protoimpl.SizeCache
	unknownFields protoimpl.UnknownFields

	Experiment     *Experiment     `protobuf:"bytes,1,opt,name=Experiment,proto3" json:"Experiment,omitempty"`
	AntDescription *AntDescription `protobuf:"bytes,2,opt,name=AntDescription,proto3" json:"AntDescription,omitempty"`
	Measurement    *Measurement    `protobuf:"bytes,3,opt,name=measurement,proto3" json:"measurement,omitempty"`
	Space          *Space          `protobuf:"bytes,4,opt,name=space,proto3" json:"space,omitempty"`
}

func (x *FileLine) Reset() {
	*x = FileLine{}
	if protoimpl.UnsafeEnabled {
		mi := &file_ExperimentFile_proto_msgTypes[1]
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		ms.StoreMessageInfo(mi)
	}
}

func (x *FileLine) String() string {
	return protoimpl.X.MessageStringOf(x)
}

func (*FileLine) ProtoMessage() {}

func (x *FileLine) ProtoReflect() protoreflect.Message {
	mi := &file_ExperimentFile_proto_msgTypes[1]
	if protoimpl.UnsafeEnabled && x != nil {
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		if ms.LoadMessageInfo() == nil {
			ms.StoreMessageInfo(mi)
		}
		return ms
	}
	return mi.MessageOf(x)
}

// Deprecated: Use FileLine.ProtoReflect.Descriptor instead.
func (*FileLine) Descriptor() ([]byte, []int) {
	return file_ExperimentFile_proto_rawDescGZIP(), []int{1}
}

func (x *FileLine) GetExperiment() *Experiment {
	if x != nil {
		return x.Experiment
	}
	return nil
}

func (x *FileLine) GetAntDescription() *AntDescription {
	if x != nil {
		return x.AntDescription
	}
	return nil
}

func (x *FileLine) GetMeasurement() *Measurement {
	if x != nil {
		return x.Measurement
	}
	return nil
}

func (x *FileLine) GetSpace() *Space {
	if x != nil {
		return x.Space
	}
	return nil
}

var File_ExperimentFile_proto protoreflect.FileDescriptor

var file_ExperimentFile_proto_rawDesc = []byte{
	0x0a, 0x14, 0x45, 0x78, 0x70, 0x65, 0x72, 0x69, 0x6d, 0x65, 0x6e, 0x74, 0x46, 0x69, 0x6c, 0x65,
	0x2e, 0x70, 0x72, 0x6f, 0x74, 0x6f, 0x12, 0x10, 0x66, 0x6f, 0x72, 0x74, 0x2e, 0x6d, 0x79, 0x72,
	0x6d, 0x69, 0x64, 0x6f, 0x6e, 0x2e, 0x70, 0x62, 0x1a, 0x10, 0x45, 0x78, 0x70, 0x65, 0x72, 0x69,
	0x6d, 0x65, 0x6e, 0x74, 0x2e, 0x70, 0x72, 0x6f, 0x74, 0x6f, 0x1a, 0x1b, 0x54, 0x72, 0x61, 0x63,
	0x6b, 0x69, 0x6e, 0x67, 0x44, 0x61, 0x74, 0x61, 0x44, 0x69, 0x72, 0x65, 0x63, 0x74, 0x6f, 0x72,
	0x79, 0x2e, 0x70, 0x72, 0x6f, 0x74, 0x6f, 0x1a, 0x14, 0x41, 0x6e, 0x74, 0x44, 0x65, 0x73, 0x63,
	0x72, 0x69, 0x70, 0x74, 0x69, 0x6f, 0x6e, 0x2e, 0x70, 0x72, 0x6f, 0x74, 0x6f, 0x1a, 0x11, 0x4d,
	0x65, 0x61, 0x73, 0x75, 0x72, 0x65, 0x6d, 0x65, 0x6e, 0x74, 0x2e, 0x70, 0x72, 0x6f, 0x74, 0x6f,
	0x1a, 0x0b, 0x53, 0x70, 0x61, 0x63, 0x65, 0x2e, 0x70, 0x72, 0x6f, 0x74, 0x6f, 0x22, 0x54, 0x0a,
	0x0a, 0x46, 0x69, 0x6c, 0x65, 0x48, 0x65, 0x61, 0x64, 0x65, 0x72, 0x12, 0x22, 0x0a, 0x0c, 0x4d,
	0x61, 0x6a, 0x6f, 0x72, 0x56, 0x65, 0x72, 0x73, 0x69, 0x6f, 0x6e, 0x18, 0x01, 0x20, 0x01, 0x28,
	0x05, 0x52, 0x0c, 0x4d, 0x61, 0x6a, 0x6f, 0x72, 0x56, 0x65, 0x72, 0x73, 0x69, 0x6f, 0x6e, 0x12,
	0x22, 0x0a, 0x0c, 0x4d, 0x69, 0x6e, 0x6f, 0x72, 0x56, 0x65, 0x72, 0x73, 0x69, 0x6f, 0x6e, 0x18,
	0x02, 0x20, 0x01, 0x28, 0x05, 0x52, 0x0c, 0x4d, 0x69, 0x6e, 0x6f, 0x72, 0x56, 0x65, 0x72, 0x73,
	0x69, 0x6f, 0x6e, 0x22, 0x82, 0x02, 0x0a, 0x08, 0x46, 0x69, 0x6c, 0x65, 0x4c, 0x69, 0x6e, 0x65,
	0x12, 0x3c, 0x0a, 0x0a, 0x45, 0x78, 0x70, 0x65, 0x72, 0x69, 0x6d, 0x65, 0x6e, 0x74, 0x18, 0x01,
	0x20, 0x01, 0x28, 0x0b, 0x32, 0x1c, 0x2e, 0x66, 0x6f, 0x72, 0x74, 0x2e, 0x6d, 0x79, 0x72, 0x6d,
	0x69, 0x64, 0x6f, 0x6e, 0x2e, 0x70, 0x62, 0x2e, 0x45, 0x78, 0x70, 0x65, 0x72, 0x69, 0x6d, 0x65,
	0x6e, 0x74, 0x52, 0x0a, 0x45, 0x78, 0x70, 0x65, 0x72, 0x69, 0x6d, 0x65, 0x6e, 0x74, 0x12, 0x48,
	0x0a, 0x0e, 0x41, 0x6e, 0x74, 0x44, 0x65, 0x73, 0x63, 0x72, 0x69, 0x70, 0x74, 0x69, 0x6f, 0x6e,
	0x18, 0x02, 0x20, 0x01, 0x28, 0x0b, 0x32, 0x20, 0x2e, 0x66, 0x6f, 0x72, 0x74, 0x2e, 0x6d, 0x79,
	0x72, 0x6d, 0x69, 0x64, 0x6f, 0x6e, 0x2e, 0x70, 0x62, 0x2e, 0x41, 0x6e, 0x74, 0x44, 0x65, 0x73,
	0x63, 0x72, 0x69, 0x70, 0x74, 0x69, 0x6f, 0x6e, 0x52, 0x0e, 0x41, 0x6e, 0x74, 0x44, 0x65, 0x73,
	0x63, 0x72, 0x69, 0x70, 0x74, 0x69, 0x6f, 0x6e, 0x12, 0x3f, 0x0a, 0x0b, 0x6d, 0x65, 0x61, 0x73,
	0x75, 0x72, 0x65, 0x6d, 0x65, 0x6e, 0x74, 0x18, 0x03, 0x20, 0x01, 0x28, 0x0b, 0x32, 0x1d, 0x2e,
	0x66, 0x6f, 0x72, 0x74, 0x2e, 0x6d, 0x79, 0x72, 0x6d, 0x69, 0x64, 0x6f, 0x6e, 0x2e, 0x70, 0x62,
	0x2e, 0x4d, 0x65, 0x61, 0x73, 0x75, 0x72, 0x65, 0x6d, 0x65, 0x6e, 0x74, 0x52, 0x0b, 0x6d, 0x65,
	0x61, 0x73, 0x75, 0x72, 0x65, 0x6d, 0x65, 0x6e, 0x74, 0x12, 0x2d, 0x0a, 0x05, 0x73, 0x70, 0x61,
	0x63, 0x65, 0x18, 0x04, 0x20, 0x01, 0x28, 0x0b, 0x32, 0x17, 0x2e, 0x66, 0x6f, 0x72, 0x74, 0x2e,
	0x6d, 0x79, 0x72, 0x6d, 0x69, 0x64, 0x6f, 0x6e, 0x2e, 0x70, 0x62, 0x2e, 0x53, 0x70, 0x61, 0x63,
	0x65, 0x52, 0x05, 0x73, 0x70, 0x61, 0x63, 0x65, 0x62, 0x06, 0x70, 0x72, 0x6f, 0x74, 0x6f, 0x33,
}

var (
	file_ExperimentFile_proto_rawDescOnce sync.Once
	file_ExperimentFile_proto_rawDescData = file_ExperimentFile_proto_rawDesc
)

func file_ExperimentFile_proto_rawDescGZIP() []byte {
	file_ExperimentFile_proto_rawDescOnce.Do(func() {
		file_ExperimentFile_proto_rawDescData = protoimpl.X.CompressGZIP(file_ExperimentFile_proto_rawDescData)
	})
	return file_ExperimentFile_proto_rawDescData
}

var file_ExperimentFile_proto_msgTypes = make([]protoimpl.MessageInfo, 2)
var file_ExperimentFile_proto_goTypes = []interface{}{
	(*FileHeader)(nil),     // 0: fort.myrmidon.pb.FileHeader
	(*FileLine)(nil),       // 1: fort.myrmidon.pb.FileLine
	(*Experiment)(nil),     // 2: fort.myrmidon.pb.Experiment
	(*AntDescription)(nil), // 3: fort.myrmidon.pb.AntDescription
	(*Measurement)(nil),    // 4: fort.myrmidon.pb.Measurement
	(*Space)(nil),          // 5: fort.myrmidon.pb.Space
}
var file_ExperimentFile_proto_depIdxs = []int32{
	2, // 0: fort.myrmidon.pb.FileLine.Experiment:type_name -> fort.myrmidon.pb.Experiment
	3, // 1: fort.myrmidon.pb.FileLine.AntDescription:type_name -> fort.myrmidon.pb.AntDescription
	4, // 2: fort.myrmidon.pb.FileLine.measurement:type_name -> fort.myrmidon.pb.Measurement
	5, // 3: fort.myrmidon.pb.FileLine.space:type_name -> fort.myrmidon.pb.Space
	4, // [4:4] is the sub-list for method output_type
	4, // [4:4] is the sub-list for method input_type
	4, // [4:4] is the sub-list for extension type_name
	4, // [4:4] is the sub-list for extension extendee
	0, // [0:4] is the sub-list for field type_name
}

func init() { file_ExperimentFile_proto_init() }
func file_ExperimentFile_proto_init() {
	if File_ExperimentFile_proto != nil {
		return
	}
	file_Experiment_proto_init()
	file_TrackingDataDirectory_proto_init()
	file_AntDescription_proto_init()
	file_Measurement_proto_init()
	file_Space_proto_init()
	if !protoimpl.UnsafeEnabled {
		file_ExperimentFile_proto_msgTypes[0].Exporter = func(v interface{}, i int) interface{} {
			switch v := v.(*FileHeader); i {
			case 0:
				return &v.state
			case 1:
				return &v.sizeCache
			case 2:
				return &v.unknownFields
			default:
				return nil
			}
		}
		file_ExperimentFile_proto_msgTypes[1].Exporter = func(v interface{}, i int) interface{} {
			switch v := v.(*FileLine); i {
			case 0:
				return &v.state
			case 1:
				return &v.sizeCache
			case 2:
				return &v.unknownFields
			default:
				return nil
			}
		}
	}
	type x struct{}
	out := protoimpl.TypeBuilder{
		File: protoimpl.DescBuilder{
			GoPackagePath: reflect.TypeOf(x{}).PkgPath(),
			RawDescriptor: file_ExperimentFile_proto_rawDesc,
			NumEnums:      0,
			NumMessages:   2,
			NumExtensions: 0,
			NumServices:   0,
		},
		GoTypes:           file_ExperimentFile_proto_goTypes,
		DependencyIndexes: file_ExperimentFile_proto_depIdxs,
		MessageInfos:      file_ExperimentFile_proto_msgTypes,
	}.Build()
	File_ExperimentFile_proto = out.File
	file_ExperimentFile_proto_rawDesc = nil
	file_ExperimentFile_proto_goTypes = nil
	file_ExperimentFile_proto_depIdxs = nil
}