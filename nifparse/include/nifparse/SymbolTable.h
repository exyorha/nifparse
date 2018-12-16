#ifndef NIFPARSE_SYMBOLTABLE_H
#define NIFPARSE_SYMBOLTABLE_H

#include <stdint.h>
#include <unordered_map>
#include <nifparse/Symbol.h>

namespace nifparse {
	class SymbolTable {
	public:
		SymbolTable();
		~SymbolTable();

		SymbolTable(const SymbolTable &other) = delete;
		SymbolTable &operator =(const SymbolTable &other) = delete;

		uint32_t lookupSymbol(const char *string) const;
		const char *symbolToString(uint32_t value) const;
		size_t bytecodeStartOffset(const Symbol &symbol) const;
		bool isTypeName(const Symbol &symbol) const;

	private:
		struct SymbolLookupHash {
			size_t operator()(const char *string) const;
		};

		struct SymbolLookupEqual {
			bool operator()(const char *string1, const char *string2) const;
		};

		std::unordered_map<Symbol, uint32_t> m_typeOffsetMap;
		std::vector<const char *> m_symbolStrings;
		std::unordered_map<const char *, uint32_t, SymbolLookupHash, SymbolLookupEqual> m_symbolLookup;

		size_t m_bytecodeStartOffset;
	};
}

#endif
