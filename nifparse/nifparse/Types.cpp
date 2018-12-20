#include <nifparse/Types.h>

namespace nifparse {
	bool NIFDictionary::isA(const Symbol &type) const {
		return !typeChain.empty() && typeChain.front() == type;
	}

	bool NIFDictionary::kindOf(const Symbol &type) const {
		return std::find(typeChain.begin(), typeChain.end(), type) != typeChain.end();
	}
}
