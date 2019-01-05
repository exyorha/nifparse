#ifndef NIFPARSE_TYPE_DESCRIPTION_H
#define NIFPARSE_TYPE_DESCRIPTION_H

#include <nifparse/Types.h>

namespace nifparse {
	class BytecodeReader;
	class SerializerContext;

	class TypeDescription {
		enum SpecializationMarker {
			Specialization
		};

		TypeDescription(SpecializationMarker);

	public:
		enum class Type {
			Null,
			Bool,
			Byte,
			UInt,
			ULittle32,
			UShort,
			Int,
			Short,
			Char,
			Flags,
			Float,
			HFloat,
			HeaderString,
			LineString,
			Ptr,
			Ref,
			StringOffset,
			StringIndex,
			NamedType
		};

		TypeDescription();
		~TypeDescription();

		TypeDescription(const TypeDescription &other);
		TypeDescription &operator =(const TypeDescription &other);

		bool parse(Opcode opcode, BytecodeReader &bytecode);
		void addArrayDimension(StackValue dimension);
		void reset();

		NIFVariant readValue(SerializerContext &ctx);
		void writeValue(SerializerContext &ctx, const NIFVariant &value);

		inline const Type type() const { return m_type; }
		inline Symbol typeName() const { return m_typeName; }
		
		inline TypeDescription &specialization() { return *m_specialization; }

		inline void setArg(uint32_t arg) { m_arg = arg; }

		inline void setIsTemplate() { m_isTemplate = true; }

	private:
		NIFVariant doReadValue(SerializerContext &ctx, uint32_t outerIndex, std::vector<StackValue>::iterator it);
		NIFVariant readSingleValue(SerializerContext &ctx);

		void doWriteValue(SerializerContext &ctx, const NIFVariant &value, uint32_t outerIndex, std::vector<StackValue>::iterator it);
		void writeSingleValue(SerializerContext &ctx, const NIFVariant &value);

		Type m_type;
		std::vector<StackValue> m_dimensions;
		Symbol m_typeName;
		uint32_t m_arg;
		std::unique_ptr<TypeDescription> m_specialization;
		bool m_isTemplate;
	};
}

#endif
