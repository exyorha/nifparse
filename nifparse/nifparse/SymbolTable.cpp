#include <nifparse/SymbolTable.h>
#include <nifparse/BytecodeReader.h>

#include <sstream>

namespace nifparse {
	SymbolTable::SymbolTable() {
		BytecodeReader reader(0);
		auto typeArraySize = reader.readVarInt();
		
		m_typeOffsetMap.reserve(typeArraySize);
		for (size_t index = 0; index < typeArraySize; index++) {
			auto typeId = reader.readVarInt();
			auto typeOffset = reader.readVarInt();
			m_typeOffsetMap.emplace(Symbol(typeId), typeOffset);
		}

		auto symbolArrayBytes = reader.readVarInt();
		auto symbolCount = reader.readVarInt();

		m_bytecodeStartOffset = reader.position() + symbolArrayBytes;

		m_symbolStrings.reserve(symbolCount);
		m_symbolLookup.reserve(symbolCount);
		for (size_t index = 0; index < symbolCount; index++) {
			auto string = reader.readAsciiz();

			m_symbolStrings.emplace_back(string);
			m_symbolLookup.emplace(string, static_cast<uint32_t>(index));
		}
	}

	SymbolTable::~SymbolTable() = default;

	uint32_t SymbolTable::lookupSymbol(const char *string) const {
		auto it = m_symbolLookup.find(string);

		if (it == m_symbolLookup.end()) {
			std::stringstream error;
			error << "Symbol not found: " << string;
			throw std::runtime_error(error.str());
		}

		return it->second;
	}

	bool SymbolTable::SymbolLookupEqual::operator()(const char *string1, const char *string2) const {
		return strcmp(string1, string2) == 0;
	}

	const char *SymbolTable::symbolToString(uint32_t value) const {
		if (value == 0xFFFFFFFF)
			return "<NIL>";
		else
			return m_symbolStrings[value];
	}

	size_t SymbolTable::SymbolLookupHash::operator()(const char *string) const {
		size_t i = 0;
		uint32_t hash = 0;
		while (*string) {
			hash += *string++;
			hash += hash << 10;
			hash ^= hash >> 6;
		}
		hash += hash << 3;
		hash ^= hash >> 11;
		hash += hash << 15;
		return hash;
	}

	size_t SymbolTable::bytecodeStartOffset(const Symbol &symbol) const {
		auto it = m_typeOffsetMap.find(symbol);
		if (it == m_typeOffsetMap.end()) {
			std::stringstream error;
			error << "Symbol does not represent a type: " << symbol.toString();
			throw std::runtime_error(error.str());
		}

		return m_bytecodeStartOffset + it->second;
	}

	bool SymbolTable::isTypeName(const Symbol &symbol) const {
		return m_typeOffsetMap.count(symbol) != 0;
	}
}
