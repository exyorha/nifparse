#ifndef NIFPARSE_SYMBOL_H
#define NIFPARSE_SYMBOL_H

#include <stdint.h>
#include <utility>

namespace nifparse {
	class SymbolTable;

	class Symbol {
	public:
		constexpr Symbol() : m_value(static_cast<uint32_t>(~0)) {

		}

		explicit constexpr Symbol(uint32_t value) : m_value(value) {

		}

		Symbol(const char *string);

		inline constexpr operator uint32_t() const {
			return m_value;
		}

		constexpr bool isNull() const {
			return m_value == static_cast<uint32_t>(~0);
		}

		const char *toString() const;

		size_t typeBytecodeStartOffset() const;
		bool isTypeName() const;

	private:
		uint32_t m_value;

		static SymbolTable m_symbolTable;
	};
}

template<>
struct std::hash<nifparse::Symbol> {
	inline size_t operator()(nifparse::Symbol symbol) const {
		return std::hash<uint32_t>()(symbol);
	}
};

#endif
