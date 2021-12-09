#include <type_traits>
#include <variant>

#include "ant.h"

#include "Rcpp.h"

#include "helper.h"
#include "time.h"

void fmAnt_show(const fort::myrmidon::Ant::Ptr * a) {
	Rcpp::Rcout << "fmAnt( $ID = " << (*a)->ID()
	            << " )\n";
}

IMPLEMENT_GETTER(Ant,const fort::myrmidon::IdentificationList &,Identifications);
IMPLEMENT_GETTER(Ant,uint32_t,ID);
IMPLEMENT_FIELD(Ant,const fort::myrmidon::Color &,DisplayColor);
IMPLEMENT_METHOD(Ant,
                 uint32_t,
                 IdentifiedAt,
                 const fort::Time &, time);
IMPLEMENT_METHOD(Ant,
                 fort::myrmidon::Value,
                 GetValue,
                 const std::string &, key,
                 const fort::Time &, time);

IMPLEMENT_VOID_METHOD(Ant,
                      SetValue,
                      const std::string &, key,
                      fort::myrmidon::Value, value,
                      const fort::Time &, time);

IMPLEMENT_VOID_METHOD(Ant,
                      DeleteValue,
                      const std::string &, key,
                      const fort::Time &, time);


inline void fmAnt_AddCapsule(fort::myrmidon::Ant::Ptr * a,
                             uint32_t shapeTypeID,
                             const fort::myrmidon::Capsule & c) {
	(*a)->AddCapsule(shapeTypeID,std::make_shared<fort::myrmidon::Capsule>(c));
}

inline void fmAnt_DeleteCapsule(fort::myrmidon::Ant::Ptr * a,
                                size_t index) {
	(*a)->DeleteCapsule(index-1);
}

IMPLEMENT_VOID_METHOD(Ant,
                      ClearCapsules)
IMPLEMENT_GETTER(Ant,fort::myrmidon::TypedCapsuleList,Capsules);

inline int fmAnt_DisplayStatus(fort::myrmidon::Ant::Ptr *a) {
	return int((*a)->DisplayStatus());
}

inline void fmAnt_SetDisplayStatus(fort::myrmidon::Ant::Ptr *a, int status) {
	(*a)->SetDisplayStatus(fort::myrmidon::Ant::DisplayState(status));
}

inline uint64_t fmAnt_get(const fort::myrmidon::Ant::Ptr * a) {
	return uint64_t(a->get());
}

std::map<fort::Time,fort::myrmidon::Value> fmAnt_GetValues(fort::myrmidon::Ant::Ptr *a, const std::string & key) {
	return (*a)->GetValues(key);
}


RCPP_MODULE(ant) {
	Rcpp::class_<fort::myrmidon::Ant::Ptr>("fmAnt")
		.const_method("show",
		              &fmAnt_show,
		              "shows this Ant")
		.property("identifications",
		          &fmAnt_Identifications,
		          "The fmIdentifications for this Ant")
		.property("ID",
		          &fmAnt_ID,
		          "The ID for this Ant")
		.property("displayColor",
		          &fmAnt_DisplayColor,
		          &fmAnt_SetDisplayColor,
		          "The display color for this Ant")
		.property("displayStatus",
		          &fmAnt_DisplayStatus,
		          &fmAnt_SetDisplayStatus,
		          "The display status for this Ant")
		.property("capsules",
		          &fmAnt_Capsules,
		          "The ants capsules")
		.method("identifiedAt",
		        &fmAnt_IdentifiedAt,
		        "Gets the tag that identifies this Ant for a time")
		.method("getValue",
		        &fmAnt_GetValue,
		        "Gets ant metadata")
		.method("setValue",
		        &fmAnt_SetValue,
		        "Sets ant metadata")
		.method("deleteValue",
		        &fmAnt_DeleteValue,
		        "Deletes ant metadata")
		.method("addCapsule",
		        &fmAnt_AddCapsule,
		        "Adds an Ant capsule")
		.method("deleteCapsule",
		        &fmAnt_DeleteCapsule,
		        "Deletes an Ant capsule")
		.method("clearCapsules",
		        &fmAnt_ClearCapsules,
		        "Deletes all ants capsules")
		.method("getValues",
		        &fmAnt_GetValues,
		        "Get values for a key.")
		.const_method("get",
		              &fmAnt_get)
		;


}


namespace Rcpp {
template <> fort::myrmidon::Value as(SEXP s) {
	if ( is<bool>(s) ) {
		return as<bool>(s);
	}
	if ( is<int>(s) ) {
		return as<int>(s);
	}
	if ( is<double>(s) ) {
		return as<double>(s);
	}
	if ( is<std::string>(s) ) {
		return as<std::string>(s);
	}
	if ( is<fort::Time>(s) ) {
		return as<fort::Time>(s);
	}
	throw std::runtime_error("Could not convert to either BOOLEAN, Integer, Numerical, character or fmTime");
}

template <> SEXP wrap(const fort::myrmidon::Value & v) {
	switch(v.index()) {
	case 0:
		return wrap<bool>(std::get<bool>(v));
	case 1:
		return wrap<int>(std::get<int>(v));
	case 2:
		return wrap<double>(std::get<double>(v));
	case 3:
		return wrap<std::string>(std::get<std::string>(v));
	case 4:
		return wrap<fort::Time>(std::get<fort::Time>(v));
	default:
		throw std::runtime_error("Non exhaistive switch in fmValue conversion");
	}
}

template <> SEXP wrap(const fort::myrmidon::TypedCapsuleList & l) {
	List res;
	for (const auto & [typeID,c] : l) {
		res.push_back(List::create(Named("type") = typeID,
		                           Named("capsule") = wrap(*c)));
	}
	return res;
}

static DatetimeVector importKeys(const std::map<fort::Time,fort::myrmidon::Value> & values) {
	DatetimeVector res(values.size());
	size_t i = 0;
	for ( const auto & [t,v] : values ) {
		res[i++] = fmTime_asR(t);
	}
	res.attr("class") = "POSIXct";
	return res;
};

template <typename Container,typename T>
static SEXP importValues(const std::map<fort::Time,fort::myrmidon::Value> & values) {
	Container res(values.size());
	size_t i = 0;
	for ( const auto & [k,v] : values ) {
		res[i++] = std::get<T>(v);
	}
	return res;
}

template <>
SEXP importValues<DatetimeVector,fort::Time>(const std::map<fort::Time,fort::myrmidon::Value> & values) {
	DatetimeVector res(values.size());
	size_t i = 0;
	for ( const auto & [k,v] : values ) {
		res[i++] = fmTime_asR(std::get<fort::Time>(v));
	}
	res.attr("class") = "POSIXct";
	return res;
}

template<class> inline constexpr bool always_false_v = false;

template <> SEXP wrap(const std::map<fort::Time,fort::myrmidon::Value> & values) {
	using namespace fort;
	using namespace fort::myrmidon;

	if ( values.empty() ) {
		return R_NilValue;
	}
	fort::myrmidon::Value f = values.cbegin()->second;
	SEXP rValues = std::visit([&](auto&& arg) -> SEXP {
		using T = std::decay_t<decltype(arg)>;
		if constexpr (std::is_same_v<T, bool>) {
			return importValues<LogicalVector,bool>(values);
		} else if constexpr (std::is_same_v<T, int>) {
			return importValues<IntegerVector,int>(values);
		} else if constexpr (std::is_same_v<T, double>) {
			return importValues<NumericVector,double>(values);
		} else if constexpr (std::is_same_v<T, std::string>) {
			return importValues<CharacterVector,std::string>(values);
		} else if constexpr (std::is_same_v<T, fort::Time>) {
			return importValues<DatetimeVector,fort::Time>(values);
		} else {
			static_assert(always_false_v<T>, "non-exhaustive visitor!");
			return R_NilValue;
		}
	}, f);

	return DataFrame::create( _["times"]  = importKeys(values),
	                          _["values"]  = rValues);
}


}

//[[Rcpp::export]]
int pfmValueType(const fort::myrmidon::Value & v) {
	return v.index();
}

//[[Rcpp::export]]
fort::myrmidon::Value pfmBool() {
	return false;
}

//[[Rcpp::export]]
fort::myrmidon::Value pfmInt() {
	return 0;
}

//[[Rcpp::export]]
fort::myrmidon::Value pfmDouble() {
	return 0.0;
}

//[[Rcpp::export]]
fort::myrmidon::Value pfmString() {
	return std::string();
}

//[[Rcpp::export]]
fort::myrmidon::Value pfmTime() {
	return fort::Time();
}
