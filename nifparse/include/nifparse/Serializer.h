#ifndef NIFPARSE_SERIALIZER_H
#define NIFPARSE_SERIALIZER_H

#include <nifparse/Types.h>
#include <nifparse/BytecodeReader.h>

namespace nifparse {
	class SerializerContext;
	class TypeDescription;

	class Serializer {
	public:
		enum class Mode {
			Serialize,
			Deserialize
		};

		Serializer(Mode mode, Symbol typeSymbol, NIFVariant &value);
		~Serializer();

		Serializer(const Serializer &other) = delete;
		Serializer &operator =(const Serializer &other) = delete;
		
		void execute(SerializerContext &ctx);

		static void serialize(SerializerContext &ctx, Symbol typeSymbol, NIFVariant &value);
		static void deserialize(SerializerContext &ctx, Symbol typeSymbol, NIFVariant &value);

		inline uint32_t arg() const { return m_arg; }
		inline void setArg(uint32_t arg) { m_arg = arg; }

		inline TypeDescription *specialization() const { return m_specialization; }
		inline void setSpecialization(TypeDescription *specialization) { m_specialization = specialization; }

	private:
		void executeCompound(SerializerContext &ctx);
		void executeEnum(SerializerContext &ctx, Opcode startOpcode);
		void doExecuteCompound(SerializerContext &ctx, NIFDictionary &dictionary);
		void executeUnary(Opcode op);
		void executeBinary(Opcode op);
		uint32_t coerceForStack(const NIFVariant &value);

		Mode m_mode;
		Symbol m_type;
		BytecodeReader m_bytecodeReader;
		NIFVariant &m_value;
		std::vector<uint32_t> m_stack;
		uint32_t m_arg;
		TypeDescription *m_specialization;
	};
}

#endif
