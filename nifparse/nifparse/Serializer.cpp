#include <nifparse/Serializer.h>
#include <nifparse/TypeDescription.h>
#include <nifparse/SerializerContext.h>

#include <sstream>

namespace nifparse {

	Serializer::Serializer(Mode mode, Symbol typeSymbol, NIFVariant &value) :
		m_mode(mode),
		m_type(typeSymbol),
		m_bytecodeReader(typeSymbol.typeBytecodeStartOffset()),
		m_value(value),
		m_arg(0) {

		auto beginOp = m_bytecodeReader.readByte();

		if (static_cast<Opcode>(beginOp) != Opcode::BEGIN) {
			throw std::runtime_error("no OP_BEGIN");
		}

		auto beginTypeId = m_bytecodeReader.readVarInt();
		if (beginTypeId != typeSymbol) {
			throw std::runtime_error("mismatched type ID");
		}
	}

	Serializer::~Serializer() {

	}

	void Serializer::serialize(SerializerContext &ctx, Symbol typeSymbol, NIFVariant &value) {
		Serializer serializer(Mode::Serialize, typeSymbol, value);
		serializer.execute(ctx);
	}

	void Serializer::deserialize(SerializerContext &ctx, Symbol typeSymbol, NIFVariant &value) {
		Serializer serializer(Mode::Deserialize, typeSymbol, value);
		serializer.execute(ctx);
	}

	void Serializer::execute(SerializerContext &ctx) {
		auto typeOp = static_cast<Opcode>(m_bytecodeReader.readByte());

		switch (typeOp) {
		case Opcode::COMPOUND:
			executeCompound(ctx);
			break;

		case Opcode::BITFLAGS:
		case Opcode::ENUM:
			executeEnum(ctx, typeOp);
			break;

		default:
		{
			std::stringstream error;
			error << "Unsupported type: " << static_cast<unsigned int>(typeOp);
			throw std::runtime_error(error.str());
		}
		}

	}

	void Serializer::executeCompound(SerializerContext &ctx) {
		if (m_mode == Mode::Deserialize && std::holds_alternative<std::monostate>(m_value)) {
			m_value = NIFDictionary();
		}

		doExecuteCompound(ctx, std::get<NIFDictionary>(m_value));
	}

	void Serializer::doExecuteCompound(SerializerContext &ctx, NIFDictionary &dictionary) {
		if (m_mode == Mode::Deserialize) {
			dictionary.isNiObject = false;
			dictionary.typeChain.push_back(m_type);
		}

		TypeDescription description;
		bool fieldPresent = true;

		Opcode op;
		std::vector<NIFVariant *> indirectionStack;

		do {
			op = static_cast<Opcode>(m_bytecodeReader.readByte());

			switch (op) {
			case Opcode::INHERIT:
			{
				TypeDescription inheritedType;
				auto inheritedOp = static_cast<Opcode>(m_bytecodeReader.readByte());
				if (!inheritedType.parse(inheritedOp, m_bytecodeReader)) {
					std::stringstream error;
					error << "Unable to parse compound parent type, opcode " << static_cast<unsigned int>(inheritedOp);
					throw std::runtime_error(error.str());
				}

				if (inheritedType.type() != TypeDescription::Type::NamedType) {
					throw std::logic_error("Compound types should only inherit other compound types");
				}

				Serializer baseSerializer(m_mode, inheritedType.typeName(), m_value);
				baseSerializer.execute(ctx);
				break;
			}

			case Opcode::IS_NIOBJECT:
				if (m_mode == Mode::Deserialize) {
					dictionary.isNiObject = true;
				}

				break;

			case Opcode::SPECIALIZE:
			{
				auto specializationOp = static_cast<Opcode>(m_bytecodeReader.readByte());
				if (!description.specialization().parse(specializationOp, m_bytecodeReader)) {
					std::stringstream error;
					error << "Unable to parse specialization, opcode " << static_cast<unsigned int>(specializationOp);
					throw std::runtime_error(error.str());
				}

				break;
			}

			case Opcode::FIELD:
			{
				Symbol fieldName(m_bytecodeReader.readVarInt());
				printf("Field %s: %d\n", fieldName.toString(), fieldPresent);
				if (fieldPresent) {
					if (m_mode == Mode::Deserialize) {
						auto value = description.readValue(ctx);
						dictionary.data.emplace(fieldName, std::move(value));
					}
					else {
						auto it = dictionary.data.find(fieldName);
						if (it == dictionary.data.end()) {
							std::stringstream error;
							error << "Required field is not in dictionary: " << fieldName.toString();
							throw std::runtime_error(error.str());
						}
						description.writeValue(ctx, it->second);
					}
				}
				fieldPresent = true;
				description.reset();
				break;
			}

			case Opcode::STATIC_ARRAY:
				description.addArrayDimension(m_bytecodeReader.readVarInt());
				break;

			case Opcode::DYNAMIC_ARRAY:
				if (m_stack.empty())
					throw std::runtime_error("stack underflow");

				description.addArrayDimension(m_stack.back());
				m_stack.pop_back();

				break;

			case Opcode::FIELD_INDIRECTION:
			{
				NIFDictionary *dict;
				if(indirectionStack.empty())
					dict = &std::get<NIFDictionary>(m_value);
				else {
					dict = &std::get<NIFDictionary>(*indirectionStack.back());
					indirectionStack.pop_back();
				}

				Symbol fieldName(m_bytecodeReader.readVarInt());
				auto it = dict->data.find(fieldName);
				if (it == dict->data.end()) {
					std::stringstream error;
					error << "Required field is not in dictionary: " << fieldName.toString();
					throw std::runtime_error(error.str());
				}

				indirectionStack.push_back(&it->second);
				break;
			}

			case Opcode::FIELD_VALUE:
			{
				NIFDictionary *dict;
				if (indirectionStack.empty())
					dict = &std::get<NIFDictionary>(m_value);
				else {
					dict = &std::get<NIFDictionary>(*indirectionStack.back());
					indirectionStack.pop_back();
				}

				Symbol fieldName(m_bytecodeReader.readVarInt());
				if (fieldName.isTypeName()) {
					if (std::find(dict->typeChain.begin(), dict->typeChain.end(), fieldName) == dict->typeChain.end()) {
						m_stack.push_back(0);
					}
					else {
						m_stack.push_back(1);
					}
				}
				else {
					auto it = dict->data.find(fieldName);
					if (it == dict->data.end()) {
						std::stringstream error;
						error << "Required field is not in dictionary: " << fieldName.toString();
						throw std::runtime_error(error.str());
					}

					m_stack.push_back(coerceForStack(it->second));
				}
			}
				break;

			case Opcode::LITERAL:
				m_stack.push_back(m_bytecodeReader.readVarInt());
				break;

					
			case Opcode::NOT:
				executeUnary(op);
				break;

			case Opcode::MUL:
			case Opcode::DIV:
			case Opcode::MOD:
			case Opcode::ADD:
			case Opcode::SUB:
			case Opcode::LSHIFT:
			case Opcode::RSHIFT:
			case Opcode::LESSTHAN:
			case Opcode::LESSOREQUAL:
			case Opcode::GREATERTHAN:
			case Opcode::GREATEROREQUAL:
			case Opcode::EQUAL:
			case Opcode::NOTEQUAL:
			case Opcode::BITAND:
			case Opcode::XOR:
			case Opcode::BITOR:
			case Opcode::LOGAND:
			case Opcode::LOGOR:
				executeBinary(op);
				break;

			case Opcode::HEADER_FIELD:
			{
				Symbol fieldName(m_bytecodeReader.readVarInt());
				auto &header = std::get<NIFDictionary>(ctx.header).data;
				auto it = header.find(fieldName);
				if (it == header.end() && fieldName == Symbol("Version")) {
					it = header.find(Symbol("Header String"));
				}

				if (it == header.end()) {
					std::stringstream error;
					error << "Required field is not in dictionary: " << fieldName.toString();
					throw std::runtime_error(error.str());
				}

				m_stack.push_back(std::get<uint32_t>(it->second));
				break;
			}

			case Opcode::CONDITION:
				if (m_stack.empty())
					throw std::runtime_error("stack underflow");

				fieldPresent = m_stack.back() != 0;
				m_stack.pop_back();
				break;

			case Opcode::ARG:
				m_stack.push_back(m_arg);
				break;

			case Opcode::SETARG:
				if (m_stack.empty())
					throw std::runtime_error("stack underflow");

				description.setArg(m_stack.back());
				m_stack.pop_back();
				break;

			case Opcode::DUP:
				if (m_stack.empty())
					throw std::runtime_error("stack underflow");

				m_stack.push_back(m_stack.back());
				break;

			case Opcode::BRANCHUNLESS:
				if (m_stack.empty())
					throw std::runtime_error("stack underflow");

				{
					auto displacement = m_bytecodeReader.readU16();
					auto value = m_stack.back();
					m_stack.pop_back();

					if (value == 0) {
						m_bytecodeReader.branch(static_cast<int>(displacement) - 2);
					}
				}
				break;

			case Opcode::BRANCHIF:
				if (m_stack.empty())
					throw std::runtime_error("stack underflow");

				{
					auto displacement = m_bytecodeReader.readU16();
					auto value = m_stack.back();
					m_stack.pop_back();

					if (value != 0) {
						m_bytecodeReader.branch(static_cast<int>(displacement) - 2);
					}
				}
				break;

			case Opcode::END:
				break;

			default:
				if(!description.parse(op, m_bytecodeReader)) {
					std::stringstream error;
					error << "Unknown opcode " << static_cast<unsigned int>(op);
					throw std::runtime_error(error.str());
				}
			}
		} while (op != Opcode::END);
	}

	void Serializer::executeEnum(SerializerContext &ctx, Opcode startOpcode) {
		TypeDescription storageType;

		auto storageOp = static_cast<Opcode>(m_bytecodeReader.readByte());
		if (!storageType.parse(storageOp, m_bytecodeReader)) {
			std::stringstream error;
			error << "Unable to parse enum storage type, opcode " << static_cast<unsigned int>(storageOp);
			throw std::runtime_error(error.str());
		}

		uint32_t physicalValue = 0;

		if (m_mode == Mode::Deserialize) {
			physicalValue = std::get<uint32_t>(storageType.readValue(ctx));

			if (startOpcode == Opcode::BITFLAGS) {
				m_value = NIFBitflags();
				std::get<NIFBitflags>(m_value).rawValue = physicalValue;
			}
			else {
				m_value = NIFEnum();
				std::get<NIFEnum>(m_value).rawValue = physicalValue;
			}
		}
		
		Opcode op;

		do {
			op = static_cast<Opcode>(m_bytecodeReader.readByte());

			switch (op) {
			case Opcode::OPTION:
			{
				Symbol name(m_bytecodeReader.readVarInt());
				auto value = m_bytecodeReader.readVarInt();

				if (startOpcode == Opcode::BITFLAGS) {
					if (m_mode == Mode::Deserialize) {
						if (physicalValue & (1 << value)) {
							std::get<NIFBitflags>(m_value).symbolicValues.push_back(name);
						}
					}
					else if (m_mode == Mode::Serialize) {
						for (const auto &sym : std::get<NIFBitflags>(m_value).symbolicValues) {
							if (sym == name) {
								physicalValue |= (1 << value);
							}
						}
					}
				}
				else {
					if (m_mode == Mode::Deserialize) {
						if (physicalValue == value) {
							std::get<NIFEnum>(m_value).symbolicValue = name;
						}
					}
					else if (m_mode == Mode::Serialize) {
						if (std::get<NIFEnum>(m_value).symbolicValue == name) {
							physicalValue = value;
						}
					}
				}
			}

				break;

			case Opcode::END:
				break;

			default:
			{
				std::stringstream error;
				error << "Unknown opcode " << static_cast<unsigned int>(op);
				throw std::runtime_error(error.str());
			}
			}
		} while (op != Opcode::END);

		if (m_mode == Mode::Serialize) {
			if (startOpcode == Opcode::ENUM) {
				std::get<NIFEnum>(m_value).rawValue = physicalValue;
			}
			else {
				std::get<NIFBitflags>(m_value).rawValue = physicalValue;
			}

			storageType.writeValue(ctx, physicalValue);
		}
	}

	void Serializer::executeUnary(Opcode op) {
		if (m_stack.empty())
			throw std::runtime_error("stack underflow");

		auto val = m_stack.back();
		m_stack.pop_back();

		uint32_t result;

		switch (op) {
		case Opcode::NOT:
			result = val == 0;
			break;
		default:
			throw std::logic_error("unsupported unary opcode");
		}

		m_stack.push_back(result);

	}

	void Serializer::executeBinary(Opcode op) {
		if (m_stack.size() < 2)
			throw std::runtime_error("stack underflow");

		auto right = m_stack.back();
		m_stack.pop_back();

		auto left = m_stack.back();
		m_stack.pop_back();

		uint32_t result;

		switch (op) {

		case Opcode::MUL:
			result = left * right;
			break;

		case Opcode::DIV:
			result = left / right;
			break;

		case Opcode::MOD:
			result = left % right;

		case Opcode::ADD:
			result = left + right;
			break;

		case Opcode::SUB:
			result = left - right;
			break;

		case Opcode::LSHIFT:
			result = left << right;
			break;

		case Opcode::RSHIFT:
			result = left >> right;
			break;

		case Opcode::LESSTHAN:
			result = left < right;
			break;

		case Opcode::LESSOREQUAL:
			result = left <= right;
			break;

		case Opcode::GREATERTHAN:
			result = left > right;
			break;

		case Opcode::GREATEROREQUAL:
			result = left >= right;
			break;

		case Opcode::EQUAL:
			result = left == right;
			break;

		case Opcode::NOTEQUAL:
			result = left != right;
			break;

		case Opcode::BITAND:
			result = left & right;
			break;

		case Opcode::XOR:
			result = left ^ right;
			break;

		case Opcode::BITOR:
			result = left | right;
			break;

		case Opcode::LOGAND:
			result = left && right;
			break;

		case Opcode::LOGOR:
			result = left || right;
			break;

		default:
			throw std::logic_error("unsupported unary opcode");
		}

		m_stack.push_back(result);
	}

	uint32_t Serializer::coerceForStack(const NIFVariant &value) {
		auto enumval = std::get_if<NIFEnum>(&value);
		
		if(enumval) {
			return enumval->rawValue;
		}
		else {
			auto bitval = std::get_if<NIFBitflags>(&value);
			if (bitval) {
				return bitval->rawValue;
			}
			else {
				return std::get<uint32_t>(value);
			}
		}
	}
}
