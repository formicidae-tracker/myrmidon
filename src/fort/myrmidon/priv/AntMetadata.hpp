#pragma once

#include <functional>
#include <map>
#include <memory>
#include <string>

#include <fort/myrmidon/types/Value.hpp>

namespace fort {
namespace myrmidon {
namespace priv {

class AntMetadata {
public:
	typedef std::shared_ptr<AntMetadata>       Ptr;
	typedef std::shared_ptr<const AntMetadata> ConstPtr;

	enum class Validity {
		Valid        = 0,
		Intermediate = 1,
		Invalid      = 2,
	};

	class Key {
	public:
		typedef std::shared_ptr<Key>       Ptr;
		typedef std::shared_ptr<const Key> ConstPtr;

		Key(const std::weak_ptr<AntMetadata> &metadata,
		    const std::string                &name,
		    const Value                      &defaultValue);

		const std::string &Name() const;
		void               SetName(const std::string &name);

		ValueType Type() const;

		const Value &DefaultValue() const;

		void SetDefaultValue(const Value &value);

	private:
		std::weak_ptr<AntMetadata> d_metadata;
		std::string                d_name;
		Value                      d_default;
	};

	typedef std::map<std::string, Key::Ptr> KeysByName;

	typedef std::function<void(const std::string &, const std::string &)>
	    NameChangeCallback;
	typedef std::function<void(const std::string &, ValueType, ValueType)>
	    TypeChangeCallback;
	typedef std::function<
	    void(const std::string &, const Value &, const Value &)>
	    DefaultChangeCallback;

	static AntMetadata::Validity
	Validate(ValueType type, const std::string &value);

	static Key::Ptr
	SetKey(const Ptr &itself, const std::string &name, Value defaultValue);

	AntMetadata();

	AntMetadata(
	    const NameChangeCallback    &onNameChange,
	    const TypeChangeCallback    &onTypeChange,
	    const DefaultChangeCallback &onDefaultChange
	);

	size_t Count(const std::string &name) const;

	void Delete(const std::string &columnName);

	const KeysByName &Keys() const;

private:
	void CheckName(const std::string &name) const;

	KeysByName            d_keys;
	NameChangeCallback    d_onNameChange;
	TypeChangeCallback    d_onTypeChange;
	DefaultChangeCallback d_onDefaultChange;
};

} // namespace priv
} // namespace myrmidon
} // namespace fort
