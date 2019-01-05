#ifndef NIFPARSE_TYPES_H
#define NIFPARSE_TYPES_H

#include <variant>
#include <unordered_map>
#include <nifparse/Symbol.h>
#include <sstream>

namespace nifparse {
	struct NIFDictionary;
	struct NIFArray;
	struct NIFReference;
	struct NIFPointer;
	struct NIFEnum;
	struct NIFBitflags;

	using NIFVariant = std::variant<
		std::monostate,		// Null
		uint32_t,			// Integer
		NIFDictionary,		// Generic dictionary
		NIFArray,			// Generic array
		NIFEnum,				// Symbol (enum)
		NIFBitflags,// Symbol array (bitflags)
		std::vector<unsigned char>,	// Byte array
		std::string,				// String
		NIFReference,				// Reference (strong pointer)
		NIFPointer,					// (Weak) pointer
		float						// Floating point number
	>;

	using StackValue = std::variant<uint32_t, NIFArray>;

	struct NIFDictionary {
		std::unordered_map<Symbol, NIFVariant> data;
		std::vector<Symbol> typeChain;
		bool isNiObject;

		template<typename T>
		T &getValue(Symbol key) {
			auto it = data.find(key);
			if (it == data.end()) {
				std::stringstream stream;
				stream << "No key " << key.toString() << " in dictionary";
				throw std::runtime_error(stream.str());
			}

			return std::get<T>(it->second);
		}

		template<typename T>
		const T &getValue(Symbol key) const {
			auto it = data.find(key);
			if (it == data.end()) {
				std::stringstream stream;
				stream << "No key " << key.toString() << " in dictionary";
				throw std::runtime_error(stream.str());
			}

			return std::get<T>(it->second);
		}

		bool isA(const Symbol &type) const;
		bool kindOf(const Symbol &type) const;
	};

	struct NIFArray {
		std::vector<NIFVariant> data;
	};

	enum class Opcode : uint8_t {
		BOOL = 1,
		BYTE = 2,
		UINT = 3,
		ULITTLE32 = 4,
		USHORT = 5,
		INT = 6,
		SHORT = 7,
		BLOCKTYPEINDEX = 8,
		CHAR = 9,
		FILEVERSION = 10,
		FLAGS = 11,
		FLOAT = 12,
		HFLOAT = 13,
		HEADER_STRING = 14,
		LINE_STRING = 15,
		PTR = 16,
		REF = 17,
		STRINGOFFSET = 18,
		STRINGINDEX = 19,
		IS_NIOBJECT = 20,
		INHERIT = 21,
		IS_TEMPLATE = 22,
		NAMED_TYPE = 23,
		SPECIALIZE = 24,
		FIELD = 25,
		TEMPLATE_ARGUMENT = 26,
		STATIC_ARRAY = 27,
		DYNAMIC_ARRAY = 28,
		FIELD_INDIRECTION = 29,
		FIELD_VALUE = 30,
		LITERAL = 31,
		NOT = 32,
		MUL = 33,
		DIV = 34,
		MOD = 35,
		ADD = 36,
		SUB = 37,
		LSHIFT = 38,
		RSHIFT = 39,
		LESSTHAN = 40,
		LESSOREQUAL = 41,
		GREATERTHAN = 42,
		GREATEROREQUAL = 43,
		EQUAL = 44,
		NOTEQUAL = 45,
		BITAND = 46,
		XOR = 47,
		BITOR = 48,
		LOGAND = 49,
		LOGOR = 50,
		HEADER_FIELD = 51,
		CONDITION = 52,
		ARG = 53,
		SETARG = 54,
		BEGIN = 55,
		BITFLAGS = 56,
		ENUM = 57,
		COMPOUND = 58,
		OPTION = 59,
		DUP = 60,
		BRANCHUNLESS = 61,
		BRANCHIF = 62,
		BRANCH = 63,
		FIELD_DEFAULT = 64,
		END = 255
	};

	struct NIFReference {
		Symbol type;
		int32_t target;
		std::shared_ptr<NIFVariant> ptr;
	};

	struct NIFPointer {
		Symbol type;
		int32_t target;
		std::weak_ptr<NIFVariant> ptr;
	};

	struct NIFEnum {
		uint32_t rawValue;
		Symbol symbolicValue;
	};

	struct NIFBitflags {
		uint32_t rawValue;
		std::vector<Symbol> symbolicValues;
	};
}

#endif
