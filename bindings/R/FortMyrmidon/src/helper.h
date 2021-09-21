#pragma once

#define IMPLEMENT_GETTER(ClassName,TypeName,Name) \
	inline const TypeName & fm ## ClassName ## _ ## Name( fort::myrmidon::ClassName::Ptr * object) { \
		return (*object)->Name(); \
	}

#define IMPLEMENT_SETTER(ClassName,TypeName,Name)	\
	inline void fm ## ClassName ## _ ## Set ## Name( fort::myrmidon::ClassName::Ptr * object, \
	                                          const TypeName & v) { \
		(*object)->Set ## Name (v); \
	}

#define IMPLEMENT_FIELD(ClassName,TypeName,Name) \
	IMPLEMENT_GETTER(ClassName,TypeName,Name)\
	IMPLEMENT_SETTER(ClassName,TypeName,Name)
