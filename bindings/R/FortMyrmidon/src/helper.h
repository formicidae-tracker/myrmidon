#pragma once

#define DECLARATION0(...)
#define DECLARATION1(first,second,...) , first second
#define DECLARATION2(first,second,...) , first second DECLARATION1(__VA_ARGS__)
#define DECLARATION3(first,second,...) , first second DECLARATION2(__VA_ARGS__)
#define DECLARATION4(first,second,...) , first second DECLARATION3(__VA_ARGS__)

#define ARGUMENTS0(...)
#define ARGUMENTS1(first,second,...) second
#define ARGUMENTS2(first,second,...) second, ARGUMENTS1(__VA_ARGS__)
#define ARGUMENTS3(first,second,...) second, ARGUMENTS2(__VA_ARGS__)
#define ARGUMENTS4(first,second,...) second, ARGUMENTS3(__VA_ARGS__)

#define ARGUMENTS_COUNT(_1,__1,_2,__2,_3,__3,_4,__4,num,...) ARGUMENTS ## num
#define DECLARATION_COUNT(_1,__1,_2,__2,_3,__3,_4,__4,num,...) DECLARATION ## num
#define ARGUMENTS(...) ARGUMENTS_COUNT(__VA_ARGS__,4,4,3,3,2,2,1,0)(__VA_ARGS__)
#define DECLARATION(...) DECLARATION_COUNT(__VA_ARGS__,4,4,3,3,2,2,1,0)(__VA_ARGS__)


#define IMPLEMENT_VOID_METHOD_X(FmClassName,ClassType,Name,...)	  \
	inline void FmClassName ## _ ## Name(ClassType * object DECLARATION(__VA_ARGS__)) { \
		(*object)->Name(ARGUMENTS(__VA_ARGS__)); \
	}

#define IMPLEMENT_METHOD_X(FmClassName,ClassType,ReturnType,Name,...)	  \
	inline ReturnType FmClassName ## _ ## Name( ClassType * object DECLARATION(__VA_ARGS__)) { \
		return (*object)->Name(ARGUMENTS(__VA_ARGS__)); \
	}

#define IMPLEMENT_VOID_METHOD(ClassName,Name,...)	  \
	IMPLEMENT_VOID_METHOD_X(fm ## ClassName,fort::myrmidon::ClassName::Ptr,Name,__VA_ARGS__)

#define IMPLEMENT_METHOD(ClassName,ReturnType,Name,...)	  \
	IMPLEMENT_METHOD_X(fm ## ClassName,fort::myrmidon::ClassName::Ptr,ReturnType,Name,__VA_ARGS__)


#define IMPLEMENT_GETTER_X(FmClassName,ClassType,TypeName,Name) \
	IMPLEMENT_METHOD_X(FmClassName,ClassType,TypeName,Name)

#define IMPLEMENT_SETTER_X(FmClassName,ClassType,TypeName,Name) \
	IMPLEMENT_VOID_METHOD_X(FmClassName,ClassType,Set ## Name,TypeName,value)

#define IMPLEMENT_GETTER(ClassName,TypeName,Name)	  \
	IMPLEMENT_GETTER_X(fm ## ClassName,fort::myrmidon::ClassName::Ptr,TypeName,Name)

#define IMPLEMENT_SETTER(ClassName,TypeName,Name) \
	IMPLEMENT_SETTER_X(fm ## ClassName,fort::myrmidon::ClassName::Ptr,TypeName,Name)

#define IMPLEMENT_FIELD(ClassName,TypeName,Name) \
	IMPLEMENT_GETTER(ClassName,TypeName,Name) \
	IMPLEMENT_SETTER(ClassName,TypeName,Name)

#define IMPLEMENT_FIELD_X(FmClassName,ClassType,TypeName,Name)	  \
	IMPLEMENT_GETTER_X(FmClassName,ClassType,TypeName,Name) \
	IMPLEMENT_SETTER_X(fmClassName,ClassType,TypeName,Name)
