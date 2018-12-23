#include <nifparse/TypeDescription.h>
#include <nifparse/SerializerContext.h>
#include <nifparse/BytecodeReader.h>
#include <nifparse/Serializer.h>
#include <nifparse/INIFDataStream.h>

#include <half.h>

#include <sstream>
#include <regex>

namespace nifparse {
	static const std::regex fileVersionRegex1("^NetImmerse File Format, Version ([0-9]+)\\.([0-9]+)\\.([0-9]+)\\.([0-9]+)$");
	static const std::regex fileVersionRegex2("^Gamebryo File Format, Version ([0-9]+)\\.([0-9]+)\\.([0-9]+)\\.([0-9]+)$");
	
	TypeDescription::TypeDescription(SpecializationMarker) : m_type(Type::Null), m_arg(0), m_isTemplate(false) {

	}

	TypeDescription::TypeDescription() : m_type(Type::Null), m_arg(0), m_specialization(new TypeDescription(Specialization)) {

	}

	TypeDescription::~TypeDescription() {
		
	}

	TypeDescription::TypeDescription(const TypeDescription &other) : m_type(Type::Null), m_arg(0), m_isTemplate(false) {
		*this = other;
	}
	
	TypeDescription &TypeDescription::operator =(const TypeDescription &other) {
		m_type = other.m_type;
		m_dimensions = other.m_dimensions;
		m_typeName = other.m_typeName;
		m_arg = other.m_arg;
		if (other.m_specialization) {
			m_specialization = std::make_unique<TypeDescription>(*other.m_specialization);
		}
		else {
			m_specialization.reset();
		}
		m_isTemplate = other.m_isTemplate;
		return *this;
	}

	bool TypeDescription::parse(Opcode opcode, BytecodeReader &bytecode) {
		switch (opcode) {
		case Opcode::BOOL:
			m_type = Type::Bool;
			return true;

		case Opcode::BYTE:
			m_type = Type::Byte;
			return true;

		case Opcode::UINT:
			m_type = Type::UInt;
			return true;

		case Opcode::ULITTLE32:
		case Opcode::FILEVERSION:
			m_type = Type::ULittle32;
			return true;

		case Opcode::USHORT:
			m_type = Type::UShort;
			return true;

		case Opcode::INT:
			m_type = Type::Int;
			return true;

		case Opcode::SHORT:
			m_type = Type::Short;
			return true;

		case Opcode::BLOCKTYPEINDEX:
			m_type = Type::UShort;
			return true;

		case Opcode::CHAR:
			m_type = Type::Char;
			return true;
			
		case Opcode::FLAGS:
			m_type = Type::Flags;
			return true;

		case Opcode::FLOAT:
			m_type = Type::Float;
			return true;

		case Opcode::HFLOAT:
			m_type = Type::HFloat;
			return true;

		case Opcode::HEADER_STRING:
			m_type = Type::HeaderString;
			return true;

		case Opcode::LINE_STRING:
			m_type = Type::LineString;
			return true;

		case Opcode::PTR:
			m_type = Type::Ptr;
			return true;

		case Opcode::REF:
			m_type = Type::Ref;
			return true;

		case Opcode::STRINGOFFSET:
			m_type = Type::StringOffset;
			return true;

		case Opcode::STRINGINDEX:
			m_type = Type::StringIndex;
			return true;

		case Opcode::NAMED_TYPE:
			m_type = Type::NamedType;
			m_typeName = Symbol(bytecode.readVarInt());
			return true;
						
		default:
			return false;
		}
	}

	void TypeDescription::addArrayDimension(size_t dimension) {
		m_dimensions.emplace_back(dimension);
	}

	void TypeDescription::reset() {
		m_type = Type::Null;
		m_dimensions.clear();
		m_typeName = Symbol();
		m_arg = 0;
		if (m_specialization) {
			m_specialization->reset();
		}
		m_isTemplate = false;
	}

	NIFVariant TypeDescription::readValue(SerializerContext &ctx) {
		return doReadValue(ctx, m_dimensions.begin());
	}

	NIFVariant TypeDescription::doReadValue(SerializerContext &ctx, std::vector<size_t>::iterator it) {
		if (it == m_dimensions.end()) {
			return readSingleValue(ctx);
		}
		else {
			auto nextIt = it;
			++nextIt;

			size_t arraySize = *it;
			NIFVariant value;
			
			if (nextIt == m_dimensions.end() && m_type == Type::Byte) {
				// Byte array

				value = std::vector<unsigned char>(arraySize);

				auto &arrayData = std::get<std::vector<unsigned char>>(value);

				ctx.stream().readBytes(arrayData.data(), arrayData.size());
			}
			else if (nextIt == m_dimensions.end() && m_type == Type::Char) {
				// String

				value = std::string();

				auto &arrayData = std::get<std::string>(value);

				arrayData.resize(arraySize);
				ctx.stream().readBytes(reinterpret_cast<unsigned char *>(arrayData.data()), arrayData.size());
			}
			else {

				value = NIFArray();

				auto &arrayData = std::get<NIFArray>(value);

				arrayData.data.reserve(arraySize);

				for (size_t index = 0; index < arraySize; index++) {
					arrayData.data.emplace_back(doReadValue(ctx, nextIt));
				}
			}

			return value;
		}
	}

	NIFVariant TypeDescription::readSingleValue(SerializerContext &ctx) {
		NIFVariant value;

		switch (m_type) {
		case Type::Null:
			throw std::runtime_error("attempted to read null type");

		case Type::Bool:
			if (!ctx.useConstantLengths() && std::get<NIFDictionary>(ctx.header).getValue<uint32_t>("Version") > 0x04000002) {
				union {
					unsigned char bytes[1];
					uint8_t val;
				} u;

				ctx.stream().readBytes(u.bytes, sizeof(u.bytes));

				value = static_cast<uint32_t>(u.val);
			}
			else {
				union {
					unsigned char bytes[4];
					uint32_t val;
				} u;

				ctx.stream().readBytes(u.bytes, sizeof(u.bytes));

				value = u.val;
			}

			break;

		case Type::Byte:
		{
			union {
				unsigned char bytes[1];
				uint8_t val;
			} u;

			ctx.stream().readBytes(u.bytes, sizeof(u.bytes));

			value = static_cast<uint32_t>(u.val);
			break;
		}

		case Type::Char:
		{
			union {
				char bytes[1];
				uint8_t val;
			} u;

			ctx.stream().readBytes(reinterpret_cast<unsigned char *>(u.bytes), sizeof(u.bytes));

			value = static_cast<uint32_t>(u.val);
			break;
		}

		case Type::UInt:
		case Type::ULittle32:
		case Type::StringIndex:
		{
			union {
				unsigned char bytes[4];
				uint32_t val;
			} u;

			ctx.stream().readBytes(u.bytes, sizeof(u.bytes));

			value = static_cast<uint32_t>(u.val);
			break;
		}

		case Type::Int:
		{
			union {
				unsigned char bytes[4];
				int32_t val;
			} u;

			ctx.stream().readBytes(u.bytes, sizeof(u.bytes));

			value = static_cast<uint32_t>(u.val);
			break;
		}

		case Type::Float:
		{			
			union {
				unsigned char bytes[4];
				float val;
			} u;

			ctx.stream().readBytes(u.bytes, sizeof(u.bytes));

			value = u.val;
			break;
		}

		case Type::UShort:
		case Type::Flags:
		{
			union {
				unsigned char bytes[2];
				uint16_t val;
			} u;

			ctx.stream().readBytes(u.bytes, sizeof(u.bytes));

			value = static_cast<uint32_t>(u.val);
			break;
		}

		case Type::Short:
		{
			union {
				unsigned char bytes[2];
				int16_t val;
			} u;

			ctx.stream().readBytes(u.bytes, sizeof(u.bytes));

			value = static_cast<uint32_t>(u.val);
			break;
		}

		case Type::HeaderString:
		{
			std::string headerString;
			unsigned char byte;
			do {
				ctx.stream().readBytes(&byte, 1);

				if (byte != '\n')
					headerString.push_back(static_cast<char>(byte));
			} while (byte != '\n');

			std::smatch matches;

			if (!std::regex_match(headerString.cbegin(), headerString.cend(), matches, fileVersionRegex1)) {
				if (!std::regex_match(headerString.cbegin(), headerString.cend(), matches, fileVersionRegex2)) {
					throw std::runtime_error("malformed header string. Not a NIF file?");
				}
			}

			auto version = (std::stoul(matches[1].str()) << 24) |
				(std::stoul(matches[2].str()) << 16) |
				(std::stoul(matches[3].str()) << 8) |
				(std::stoul(matches[4].str()) << 0);

			value = static_cast<uint32_t>(version);
			break;
		}


		case Type::Ref:
		{			
			union {
				unsigned char bytes[4];
				int32_t val;
			} u;

			ctx.stream().readBytes(u.bytes, sizeof(u.bytes));

			if (!m_specialization || m_specialization->type() != Type::NamedType) {
				throw std::logic_error("Ref specialization has invalid type");
			}

			NIFReference ref;
			ref.type = m_specialization->typeName();
			ref.target = u.val;

			value = std::move(ref);
			break;
		}

		case Type::Ptr:
		{
			union {
				unsigned char bytes[4];
				int32_t val;
			} u;

			ctx.stream().readBytes(u.bytes, sizeof(u.bytes));

			if (!m_specialization || m_specialization->type() != Type::NamedType) {
				throw std::logic_error("Ref specialization has invalid type");
			}

			NIFPointer ref;
			ref.type = m_specialization->typeName();
			ref.target = u.val;

			value = std::move(ref);
			break;
		}

		case Type::HFloat:
		{
			union {
				unsigned char bytes[2];
				uint16_t val;
			} u;

			union {
				uint32_t i;
				float f;
			} u2;

			ctx.stream().readBytes(u.bytes, sizeof(u.bytes));

			u2.i = half_to_float(u.val);
			value = u2.f;
			break;
		}

		case Type::NamedType:
		{
			Serializer serializer(Serializer::Mode::Deserialize, m_typeName, value);
			serializer.setArg(m_arg);
			serializer.setSpecialization(m_specialization.get());
			serializer.execute(ctx);
			break;
		}
					
		default:
		{
			std::stringstream stream;
			stream << "Reading of type " << static_cast<unsigned int>(m_type) << " is not implemented";
			throw std::runtime_error(stream.str());
		}
		}

		return value;
	}

	void TypeDescription::writeValue(SerializerContext &ctx, const NIFVariant &value) {
		return doWriteValue(ctx, value, m_dimensions.begin());
	}


	void TypeDescription::doWriteValue(SerializerContext &ctx, const NIFVariant &value, std::vector<size_t>::iterator it) {
		if (it == m_dimensions.end()) {
			writeSingleValue(ctx, value);
		}
		else {
			size_t arraySize = *it;
			
			auto &arrayData = std::get<NIFArray>(value);

			if (arrayData.data.size() != arraySize)
				throw std::runtime_error("array size mismatch");

			auto nextIt = it;
			++nextIt;

			for (const auto &arrayValue : arrayData.data) {
				doWriteValue(ctx, arrayValue, nextIt);
			}
		}
	}

	void TypeDescription::writeSingleValue(SerializerContext &ctx, const NIFVariant &value) {
		switch (m_type) {
		case Type::Null:
			throw std::runtime_error("attempted to write null type");

		default:
		{
			std::stringstream stream;
			stream << "Writing of type " << static_cast<unsigned int>(m_type) << " is not implemented";
			throw std::runtime_error(stream.str());
		}
		}
	}
}
