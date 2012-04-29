#include "Common.hpp"

#include <boost/format.hpp>

using namespace std;

namespace dcpu { namespace ast {
	/*************************************************************************
	 *
	 * ArgumentFlags
	 *
	 *************************************************************************/

	ArgumentFlags::ArgumentFlags(ArgumentPosition position, bool indirection, bool forceNextWord) 
		: position(position), indirection(indirection), forceNextWord(forceNextWord) {}

	ArgumentPosition ArgumentFlags::getPosition() const {
		return position;
	}

	bool ArgumentFlags::isArgumentA() const {
		return position == ArgumentPosition::A;
	}

	bool ArgumentFlags::isArgumentB() const {
		return position == ArgumentPosition::B;
	}

	bool ArgumentFlags::isIndirection() const {
		return indirection;
	}

	bool ArgumentFlags::isForceNextWord() const {
		return forceNextWord;
	}

	bool ArgumentFlags::operator==(const ArgumentFlags &other) const {
		return position == other.position 
			&& indirection == other.indirection
			&& forceNextWord == other.forceNextWord;
	}

	/*************************************************************************
	 *
	 * Operators
	 *
	 *************************************************************************/

	std::ostream& operator<< (std::ostream& stream, ArgumentPosition position) {
		switch (position) {
		case ArgumentPosition::A:
			return stream << "a";
		case ArgumentPosition::B:
			return stream << "b";
		default:
			return stream << boost::format("<Unknown ArgumentPosition %04x>") % static_cast<int>(position);
		}
	}

	std::ostream& operator<< (std::ostream& stream, StackOperation operation) {
		switch (operation) {
		case StackOperation::PUSH:
			return stream << "PUSH";
		case StackOperation::POP:
			return stream << "POP";
		case StackOperation::PEEK:
			return stream << "PEEK";
		default:
			return stream << boost::format("<Unknown StackOperation %04x>") % static_cast<int>(operation);
		}
	}

	std::ostream& operator<< (std::ostream& stream, UnaryOperator op) {
		switch (op) {
		case UnaryOperator::PLUS:
			return stream << "+";
		case UnaryOperator::MINUS:
			return stream << "-";
		case UnaryOperator::NOT:
			return stream << "!";
		case UnaryOperator::BITWISE_NOT:
			return stream << "~";
		default:
			return stream << boost::format("<Unknown UnaryOperator %04x>") % static_cast<int>(op);
		}
	}

	std::ostream& operator<< (std::ostream& stream, BinaryOperator op) {
		switch (op) {
		case BinaryOperator::PLUS:
			return stream << "+";
		case BinaryOperator::MINUS:
			return stream << "-";
		case BinaryOperator::MULTIPLY:
			return stream << "*";
		case BinaryOperator::DIVIDE:
			return stream << "/";
		case BinaryOperator::MODULO:
			return stream << "%";
		case BinaryOperator::SHIFT_LEFT:
			return stream << "<<";
		case BinaryOperator::SHIFT_RIGHT:
			return stream << ">>";
		case BinaryOperator::AND:
			return stream << "&";
		case BinaryOperator::OR:
			return stream << "|";
		case BinaryOperator::XOR:
			return stream << "^";
		default:
			return stream << boost::format("<Unknown BinaryOperator %04x>") % static_cast<int>(op);
		}
	}

	std::ostream& operator<< (std::ostream& stream, Opcode opcode) {
		switch(opcode) {
		case Opcode::SET:
			return stream << "SET";
		case Opcode::ADD:
			return stream << "ADD";
		case Opcode::SUB:
			return stream << "SUB";
		case Opcode::MUL:
			return stream << "MUL";
		case Opcode::MLI:
			return stream << "MLI";
		case Opcode::DIV:
			return stream << "DIV";
		case Opcode::DVI:
			return stream << "DVI";
		case Opcode::MOD:
			return stream << "MOD";
		case Opcode::MDI:
			return stream << "MDI";
		case Opcode::AND:
			return stream << "AND";
		case Opcode::BOR:
			return stream << "BOR";
		case Opcode::XOR:
			return stream << "XOR";
		case Opcode::SHR:
			return stream << "SHR";
		case Opcode::ASR:
			return stream << "ASR";
		case Opcode::SHL:
			return stream << "SHL";
		case Opcode::STI:
			return stream << "STI";
		case Opcode::STD:
			return stream << "STD";
		case Opcode::IFB:
			return stream << "IFB";
		case Opcode::IFC:
			return stream << "IFC";
		case Opcode::IFE:
			return stream << "IFE";
		case Opcode::IFN:
			return stream << "IFN";
		case Opcode::IFG:
			return stream << "IFG";
		case Opcode::IFA:
			return stream << "IFA";
		case Opcode::IFL:
			return stream << "IFL";
		case Opcode::IFU:
			return stream << "IFU";
		case Opcode::ADX:
			return stream << "ADX";
		case Opcode::SBX:
			return stream << "SBX";
		case Opcode::JSR:
			return stream << "JSR";
		case Opcode::HCF:
			return stream << "HCF";
		case Opcode::INT:
			return stream << "INT";
		case Opcode::IAG:
			return stream << "IAG";
		case Opcode::IAS:
			return stream << "IAS";
		case Opcode::RFI:
			return stream << "RFI";
		case Opcode::IAQ:
			return stream << "IAQ";
		case Opcode::HWN:
			return stream << "HWN";
		case Opcode::HWQ:
			return stream << "HWQ";
		case Opcode::HWI:
			return stream << "HWI";
		case Opcode::JMP:
			return stream << "JMP";
		default:
			return stream << boost::format("<Unknown Opcode %04x>") % static_cast<int>(opcode);
		}
	}

	std::ostream& operator<< (std::ostream& stream, Register _register) {
		switch (_register) {
		case Register::A:
			return stream << "A";
		case Register::B:
			return stream << "B";
		case Register::C:
			return stream << "C";
		case Register::X:
			return stream << "X";
		case Register::Y:
			return stream << "Y";
		case Register::Z:
			return stream << "Z";
		case Register::I:
			return stream << "I";
		case Register::J:
			return stream << "J";
		case Register::PC:
			return stream << "PC";
		case Register::SP:
			return stream << "SP";
		case Register::EX:
			return stream << "EX";
		default:
			return stream << boost::format("<Unknown Register %04x>") % static_cast<int>(_register);
		}
	}
}}