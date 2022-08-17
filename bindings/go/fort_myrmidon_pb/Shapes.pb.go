// Code generated by protoc-gen-go. DO NOT EDIT.
// versions:
// 	protoc-gen-go v1.24.0-devel
// 	protoc        v3.6.1
// source: Shapes.proto

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

type Capsule struct {
	state         protoimpl.MessageState
	sizeCache     protoimpl.SizeCache
	unknownFields protoimpl.UnknownFields

	C1 *Vector2D `protobuf:"bytes,1,opt,name=C1,proto3" json:"C1,omitempty"`
	C2 *Vector2D `protobuf:"bytes,2,opt,name=C2,proto3" json:"C2,omitempty"`
	R1 float64   `protobuf:"fixed64,3,opt,name=R1,proto3" json:"R1,omitempty"`
	R2 float64   `protobuf:"fixed64,4,opt,name=R2,proto3" json:"R2,omitempty"`
}

func (x *Capsule) Reset() {
	*x = Capsule{}
	if protoimpl.UnsafeEnabled {
		mi := &file_Shapes_proto_msgTypes[0]
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		ms.StoreMessageInfo(mi)
	}
}

func (x *Capsule) String() string {
	return protoimpl.X.MessageStringOf(x)
}

func (*Capsule) ProtoMessage() {}

func (x *Capsule) ProtoReflect() protoreflect.Message {
	mi := &file_Shapes_proto_msgTypes[0]
	if protoimpl.UnsafeEnabled && x != nil {
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		if ms.LoadMessageInfo() == nil {
			ms.StoreMessageInfo(mi)
		}
		return ms
	}
	return mi.MessageOf(x)
}

// Deprecated: Use Capsule.ProtoReflect.Descriptor instead.
func (*Capsule) Descriptor() ([]byte, []int) {
	return file_Shapes_proto_rawDescGZIP(), []int{0}
}

func (x *Capsule) GetC1() *Vector2D {
	if x != nil {
		return x.C1
	}
	return nil
}

func (x *Capsule) GetC2() *Vector2D {
	if x != nil {
		return x.C2
	}
	return nil
}

func (x *Capsule) GetR1() float64 {
	if x != nil {
		return x.R1
	}
	return 0
}

func (x *Capsule) GetR2() float64 {
	if x != nil {
		return x.R2
	}
	return 0
}

type Circle struct {
	state         protoimpl.MessageState
	sizeCache     protoimpl.SizeCache
	unknownFields protoimpl.UnknownFields

	Center *Vector2D `protobuf:"bytes,1,opt,name=Center,proto3" json:"Center,omitempty"`
	Radius float64   `protobuf:"fixed64,2,opt,name=Radius,proto3" json:"Radius,omitempty"`
}

func (x *Circle) Reset() {
	*x = Circle{}
	if protoimpl.UnsafeEnabled {
		mi := &file_Shapes_proto_msgTypes[1]
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		ms.StoreMessageInfo(mi)
	}
}

func (x *Circle) String() string {
	return protoimpl.X.MessageStringOf(x)
}

func (*Circle) ProtoMessage() {}

func (x *Circle) ProtoReflect() protoreflect.Message {
	mi := &file_Shapes_proto_msgTypes[1]
	if protoimpl.UnsafeEnabled && x != nil {
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		if ms.LoadMessageInfo() == nil {
			ms.StoreMessageInfo(mi)
		}
		return ms
	}
	return mi.MessageOf(x)
}

// Deprecated: Use Circle.ProtoReflect.Descriptor instead.
func (*Circle) Descriptor() ([]byte, []int) {
	return file_Shapes_proto_rawDescGZIP(), []int{1}
}

func (x *Circle) GetCenter() *Vector2D {
	if x != nil {
		return x.Center
	}
	return nil
}

func (x *Circle) GetRadius() float64 {
	if x != nil {
		return x.Radius
	}
	return 0
}

type Polygon struct {
	state         protoimpl.MessageState
	sizeCache     protoimpl.SizeCache
	unknownFields protoimpl.UnknownFields

	Vertices []*Vector2D `protobuf:"bytes,1,rep,name=Vertices,proto3" json:"Vertices,omitempty"`
}

func (x *Polygon) Reset() {
	*x = Polygon{}
	if protoimpl.UnsafeEnabled {
		mi := &file_Shapes_proto_msgTypes[2]
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		ms.StoreMessageInfo(mi)
	}
}

func (x *Polygon) String() string {
	return protoimpl.X.MessageStringOf(x)
}

func (*Polygon) ProtoMessage() {}

func (x *Polygon) ProtoReflect() protoreflect.Message {
	mi := &file_Shapes_proto_msgTypes[2]
	if protoimpl.UnsafeEnabled && x != nil {
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		if ms.LoadMessageInfo() == nil {
			ms.StoreMessageInfo(mi)
		}
		return ms
	}
	return mi.MessageOf(x)
}

// Deprecated: Use Polygon.ProtoReflect.Descriptor instead.
func (*Polygon) Descriptor() ([]byte, []int) {
	return file_Shapes_proto_rawDescGZIP(), []int{2}
}

func (x *Polygon) GetVertices() []*Vector2D {
	if x != nil {
		return x.Vertices
	}
	return nil
}

type Shape struct {
	state         protoimpl.MessageState
	sizeCache     protoimpl.SizeCache
	unknownFields protoimpl.UnknownFields

	Capsule *Capsule `protobuf:"bytes,1,opt,name=capsule,proto3" json:"capsule,omitempty"`
	Circle  *Circle  `protobuf:"bytes,2,opt,name=circle,proto3" json:"circle,omitempty"`
	Polygon *Polygon `protobuf:"bytes,3,opt,name=polygon,proto3" json:"polygon,omitempty"`
}

func (x *Shape) Reset() {
	*x = Shape{}
	if protoimpl.UnsafeEnabled {
		mi := &file_Shapes_proto_msgTypes[3]
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		ms.StoreMessageInfo(mi)
	}
}

func (x *Shape) String() string {
	return protoimpl.X.MessageStringOf(x)
}

func (*Shape) ProtoMessage() {}

func (x *Shape) ProtoReflect() protoreflect.Message {
	mi := &file_Shapes_proto_msgTypes[3]
	if protoimpl.UnsafeEnabled && x != nil {
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		if ms.LoadMessageInfo() == nil {
			ms.StoreMessageInfo(mi)
		}
		return ms
	}
	return mi.MessageOf(x)
}

// Deprecated: Use Shape.ProtoReflect.Descriptor instead.
func (*Shape) Descriptor() ([]byte, []int) {
	return file_Shapes_proto_rawDescGZIP(), []int{3}
}

func (x *Shape) GetCapsule() *Capsule {
	if x != nil {
		return x.Capsule
	}
	return nil
}

func (x *Shape) GetCircle() *Circle {
	if x != nil {
		return x.Circle
	}
	return nil
}

func (x *Shape) GetPolygon() *Polygon {
	if x != nil {
		return x.Polygon
	}
	return nil
}

var File_Shapes_proto protoreflect.FileDescriptor

var file_Shapes_proto_rawDesc = []byte{
	0x0a, 0x0c, 0x53, 0x68, 0x61, 0x70, 0x65, 0x73, 0x2e, 0x70, 0x72, 0x6f, 0x74, 0x6f, 0x12, 0x10,
	0x66, 0x6f, 0x72, 0x74, 0x2e, 0x6d, 0x79, 0x72, 0x6d, 0x69, 0x64, 0x6f, 0x6e, 0x2e, 0x70, 0x62,
	0x1a, 0x0e, 0x56, 0x65, 0x63, 0x74, 0x6f, 0x72, 0x32, 0x64, 0x2e, 0x70, 0x72, 0x6f, 0x74, 0x6f,
	0x22, 0x81, 0x01, 0x0a, 0x07, 0x43, 0x61, 0x70, 0x73, 0x75, 0x6c, 0x65, 0x12, 0x2a, 0x0a, 0x02,
	0x43, 0x31, 0x18, 0x01, 0x20, 0x01, 0x28, 0x0b, 0x32, 0x1a, 0x2e, 0x66, 0x6f, 0x72, 0x74, 0x2e,
	0x6d, 0x79, 0x72, 0x6d, 0x69, 0x64, 0x6f, 0x6e, 0x2e, 0x70, 0x62, 0x2e, 0x56, 0x65, 0x63, 0x74,
	0x6f, 0x72, 0x32, 0x64, 0x52, 0x02, 0x43, 0x31, 0x12, 0x2a, 0x0a, 0x02, 0x43, 0x32, 0x18, 0x02,
	0x20, 0x01, 0x28, 0x0b, 0x32, 0x1a, 0x2e, 0x66, 0x6f, 0x72, 0x74, 0x2e, 0x6d, 0x79, 0x72, 0x6d,
	0x69, 0x64, 0x6f, 0x6e, 0x2e, 0x70, 0x62, 0x2e, 0x56, 0x65, 0x63, 0x74, 0x6f, 0x72, 0x32, 0x64,
	0x52, 0x02, 0x43, 0x32, 0x12, 0x0e, 0x0a, 0x02, 0x52, 0x31, 0x18, 0x03, 0x20, 0x01, 0x28, 0x01,
	0x52, 0x02, 0x52, 0x31, 0x12, 0x0e, 0x0a, 0x02, 0x52, 0x32, 0x18, 0x04, 0x20, 0x01, 0x28, 0x01,
	0x52, 0x02, 0x52, 0x32, 0x22, 0x54, 0x0a, 0x06, 0x43, 0x69, 0x72, 0x63, 0x6c, 0x65, 0x12, 0x32,
	0x0a, 0x06, 0x43, 0x65, 0x6e, 0x74, 0x65, 0x72, 0x18, 0x01, 0x20, 0x01, 0x28, 0x0b, 0x32, 0x1a,
	0x2e, 0x66, 0x6f, 0x72, 0x74, 0x2e, 0x6d, 0x79, 0x72, 0x6d, 0x69, 0x64, 0x6f, 0x6e, 0x2e, 0x70,
	0x62, 0x2e, 0x56, 0x65, 0x63, 0x74, 0x6f, 0x72, 0x32, 0x64, 0x52, 0x06, 0x43, 0x65, 0x6e, 0x74,
	0x65, 0x72, 0x12, 0x16, 0x0a, 0x06, 0x52, 0x61, 0x64, 0x69, 0x75, 0x73, 0x18, 0x02, 0x20, 0x01,
	0x28, 0x01, 0x52, 0x06, 0x52, 0x61, 0x64, 0x69, 0x75, 0x73, 0x22, 0x41, 0x0a, 0x07, 0x50, 0x6f,
	0x6c, 0x79, 0x67, 0x6f, 0x6e, 0x12, 0x36, 0x0a, 0x08, 0x56, 0x65, 0x72, 0x74, 0x69, 0x63, 0x65,
	0x73, 0x18, 0x01, 0x20, 0x03, 0x28, 0x0b, 0x32, 0x1a, 0x2e, 0x66, 0x6f, 0x72, 0x74, 0x2e, 0x6d,
	0x79, 0x72, 0x6d, 0x69, 0x64, 0x6f, 0x6e, 0x2e, 0x70, 0x62, 0x2e, 0x56, 0x65, 0x63, 0x74, 0x6f,
	0x72, 0x32, 0x64, 0x52, 0x08, 0x56, 0x65, 0x72, 0x74, 0x69, 0x63, 0x65, 0x73, 0x22, 0xa3, 0x01,
	0x0a, 0x05, 0x53, 0x68, 0x61, 0x70, 0x65, 0x12, 0x33, 0x0a, 0x07, 0x63, 0x61, 0x70, 0x73, 0x75,
	0x6c, 0x65, 0x18, 0x01, 0x20, 0x01, 0x28, 0x0b, 0x32, 0x19, 0x2e, 0x66, 0x6f, 0x72, 0x74, 0x2e,
	0x6d, 0x79, 0x72, 0x6d, 0x69, 0x64, 0x6f, 0x6e, 0x2e, 0x70, 0x62, 0x2e, 0x43, 0x61, 0x70, 0x73,
	0x75, 0x6c, 0x65, 0x52, 0x07, 0x63, 0x61, 0x70, 0x73, 0x75, 0x6c, 0x65, 0x12, 0x30, 0x0a, 0x06,
	0x63, 0x69, 0x72, 0x63, 0x6c, 0x65, 0x18, 0x02, 0x20, 0x01, 0x28, 0x0b, 0x32, 0x18, 0x2e, 0x66,
	0x6f, 0x72, 0x74, 0x2e, 0x6d, 0x79, 0x72, 0x6d, 0x69, 0x64, 0x6f, 0x6e, 0x2e, 0x70, 0x62, 0x2e,
	0x43, 0x69, 0x72, 0x63, 0x6c, 0x65, 0x52, 0x06, 0x63, 0x69, 0x72, 0x63, 0x6c, 0x65, 0x12, 0x33,
	0x0a, 0x07, 0x70, 0x6f, 0x6c, 0x79, 0x67, 0x6f, 0x6e, 0x18, 0x03, 0x20, 0x01, 0x28, 0x0b, 0x32,
	0x19, 0x2e, 0x66, 0x6f, 0x72, 0x74, 0x2e, 0x6d, 0x79, 0x72, 0x6d, 0x69, 0x64, 0x6f, 0x6e, 0x2e,
	0x70, 0x62, 0x2e, 0x50, 0x6f, 0x6c, 0x79, 0x67, 0x6f, 0x6e, 0x52, 0x07, 0x70, 0x6f, 0x6c, 0x79,
	0x67, 0x6f, 0x6e, 0x62, 0x06, 0x70, 0x72, 0x6f, 0x74, 0x6f, 0x33,
}

var (
	file_Shapes_proto_rawDescOnce sync.Once
	file_Shapes_proto_rawDescData = file_Shapes_proto_rawDesc
)

func file_Shapes_proto_rawDescGZIP() []byte {
	file_Shapes_proto_rawDescOnce.Do(func() {
		file_Shapes_proto_rawDescData = protoimpl.X.CompressGZIP(file_Shapes_proto_rawDescData)
	})
	return file_Shapes_proto_rawDescData
}

var file_Shapes_proto_msgTypes = make([]protoimpl.MessageInfo, 4)
var file_Shapes_proto_goTypes = []interface{}{
	(*Capsule)(nil),  // 0: fort.myrmidon.pb.Capsule
	(*Circle)(nil),   // 1: fort.myrmidon.pb.Circle
	(*Polygon)(nil),  // 2: fort.myrmidon.pb.Polygon
	(*Shape)(nil),    // 3: fort.myrmidon.pb.Shape
	(*Vector2D)(nil), // 4: fort.myrmidon.pb.Vector2d
}
var file_Shapes_proto_depIdxs = []int32{
	4, // 0: fort.myrmidon.pb.Capsule.C1:type_name -> fort.myrmidon.pb.Vector2d
	4, // 1: fort.myrmidon.pb.Capsule.C2:type_name -> fort.myrmidon.pb.Vector2d
	4, // 2: fort.myrmidon.pb.Circle.Center:type_name -> fort.myrmidon.pb.Vector2d
	4, // 3: fort.myrmidon.pb.Polygon.Vertices:type_name -> fort.myrmidon.pb.Vector2d
	0, // 4: fort.myrmidon.pb.Shape.capsule:type_name -> fort.myrmidon.pb.Capsule
	1, // 5: fort.myrmidon.pb.Shape.circle:type_name -> fort.myrmidon.pb.Circle
	2, // 6: fort.myrmidon.pb.Shape.polygon:type_name -> fort.myrmidon.pb.Polygon
	7, // [7:7] is the sub-list for method output_type
	7, // [7:7] is the sub-list for method input_type
	7, // [7:7] is the sub-list for extension type_name
	7, // [7:7] is the sub-list for extension extendee
	0, // [0:7] is the sub-list for field type_name
}

func init() { file_Shapes_proto_init() }
func file_Shapes_proto_init() {
	if File_Shapes_proto != nil {
		return
	}
	file_Vector2d_proto_init()
	if !protoimpl.UnsafeEnabled {
		file_Shapes_proto_msgTypes[0].Exporter = func(v interface{}, i int) interface{} {
			switch v := v.(*Capsule); i {
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
		file_Shapes_proto_msgTypes[1].Exporter = func(v interface{}, i int) interface{} {
			switch v := v.(*Circle); i {
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
		file_Shapes_proto_msgTypes[2].Exporter = func(v interface{}, i int) interface{} {
			switch v := v.(*Polygon); i {
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
		file_Shapes_proto_msgTypes[3].Exporter = func(v interface{}, i int) interface{} {
			switch v := v.(*Shape); i {
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
			RawDescriptor: file_Shapes_proto_rawDesc,
			NumEnums:      0,
			NumMessages:   4,
			NumExtensions: 0,
			NumServices:   0,
		},
		GoTypes:           file_Shapes_proto_goTypes,
		DependencyIndexes: file_Shapes_proto_depIdxs,
		MessageInfos:      file_Shapes_proto_msgTypes,
	}.Build()
	File_Shapes_proto = out.File
	file_Shapes_proto_rawDesc = nil
	file_Shapes_proto_goTypes = nil
	file_Shapes_proto_depIdxs = nil
}