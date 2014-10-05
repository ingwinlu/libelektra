#ifndef ELEKTRA_KEY_EXCEPT_HPP
#define ELEKTRA_KEY_EXCEPT_HPP

#ifndef USER_DEFINED_EXCEPTIONS

namespace kdb
{

class Exception : public std::exception
{
public:
	virtual const char* what() const throw()
	{
		return "Exception thrown by Elektra";
	}
};

class KeyException : public Exception
{
public:
	virtual const char* what() const throw()
	{
		return  "Exception thrown by a Key, typically "
			"because you called a method on a null key. "
			"Make sure to check this with !key first";
	}
};

class KeyTypeMismatch: public KeyException
{
public:
	virtual const char* what() const throw()
	{
		return  "Binary/String key mismatch, use proper "
			"getString()/getBinary() or use getValue() to get both.";
	}
};

class KeyTypeConversion : public KeyException
{
public:
	virtual const char* what() const throw()
	{
		return  "Could not convert data to requested type."
			"get(Meta)<std::string> or use getMeta<const Key>."
			"or specialise these template methods";
	}
};


class KeyInvalidName : public KeyException
{
public:
	virtual const char* what() const throw()
	{
		return "Invalid Keyname: keyname needs to start with user/ or system/ "
			"or maybe you tried to change a key that is already in a KeySet.";
	}
};

}

#endif

#endif
