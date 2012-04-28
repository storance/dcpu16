#include "Common.hpp"

#include <boost/format.hpp>

using namespace std;

namespace dcpu { namespace ast {
	string str(ArgumentPosition position) {
		switch (position) {
		case ArgumentPosition::A:
			return "a";
		case ArgumentPosition::B:
			return "b";
		default:
			return str(boost::format("<Unknown ArgumentPosition %04x>") % static_cast<int>(position));
		}
	}

	string str(StackOperation operation) {
		switch (operation) {
		case StackOperation::PUSH:
			return "PUSH";
		case StackOperation::POP:
			return "POP";
		case StackOperation::PEEK:
			return "PEEK";
		default:
			return str(boost::format("<Unknown StackOperation %04x>") % static_cast<int>(operation));
		}
	}

	string str(UnaryOperator op) {
		switch (op) {
		case UnaryOperator::PLUS:
			return "+";
		case UnaryOperator::MINUS:
			return "-";
		case UnaryOperator::NOT:
			return "!";
		case UnaryOperator::BITWISE_NOT:
			return "~";
		}

		return str(boost::format("<Unknown UnaryOperator %04x>") % static_cast<int>(op));
	}

	string str(BinaryOperator op) {
		switch (op) {
		case BinaryOperator::PLUS:
			return "+";
		case BinaryOperator::MINUS:
			return "-";
		case BinaryOperator::MULTIPLY:
			return "*";
		case BinaryOperator::DIVIDE:
			return "/";
		case BinaryOperator::MODULO:
			return "%";
		case BinaryOperator::SHIFT_LEFT:
			return "<<";
		case BinaryOperator::SHIFT_RIGHT:
			return ">>";
		case BinaryOperator::AND:
			return "&";
		case BinaryOperator::OR:
			return "|";
		case BinaryOperator::XOR:
			return "^";
		}

		return str(boost::format("<Unknown BinaryOperator %04x>") % static_cast<int>(op));
	}

	std::string str(Opcode opcode) {
		switch(opcode) {
		case Opcode::SET:
			return "SET";
		case Opcode::ADD:
			return "ADD";
		case Opcode::SUB:
			return "SUB";
		case Opcode::MUL:
			return "MUL";
		case Opcode::MLI:
			return "MLI";
		case Opcode::DIV:
			return "DIV";
		case Opcode::DVI:
			return "DVI";
		case Opcode::MOD:
			return "MOD";
		case Opcode::MDI:
			return "MDI";
		case Opcode::AND:
			return "AND";
		case Opcode::BOR:
			return "BOR";
		case Opcode::XOR:
			return "XOR";
		case Opcode::SHR:
			return "SHR";
		case Opcode::ASR:
			return "ASR";
		case Opcode::SHL:
			return "SHL";
		case Opcode::STI:
			return "STI";
		case Opcode::STD:
			return "STD";
		case Opcode::IFB:
			return "IFB";
		case Opcode::IFC:
			return "IFC";
		case Opcode::IFE:
			return "IFE";
		case Opcode::IFN:
			return "IFN";
		case Opcode::IFG:
			return "IFG";
		case Opcode::IFA:
			return "IFA";
		case Opcode::IFL:
			return "IFL";
		case Opcode::IFU:
			return "IFU";
		case Opcode::ADX:
			return "ADX";
		case Opcode::SBX:
			return "SBX";
		case Opcode::JSR:
			return "JSR";
		case Opcode::HCF:
			return "HCF";
		case Opcode::INT:
			return "INT";
		case Opcode::IAG:
			return "IAG";
		case Opcode::IAS:
			return "IAS";
		case Opcode::RFI:
			return "RFI";
		case Opcode::IAQ:
			return "IAQ";
		case Opcode::HWN:
			return "HWN";
		case Opcode::HWQ:
			return "HWQ";
		case Opcode::HWI:
			return "HWI";
		case Opcode::JMP:
			return "JMP";
		}

		return str(boost::format("<Unknown Opcode %04x>") % static_cast<int>(opcode));
	}

	std::string str(Register _register) {
		switch (_register) {
		case Register::A:
			return "A";
		case Register::B:
			return "B";
		case Register::C:
			return "C";
		case Register::X:
			return "X";
		case Register::Y:
			return "Y";
		case Register::Z:
			return "Z";
		case Register::I:
			return "I";
		case Register::J:
			return "J";
		case Register::PC:
			return "PC";
		case Register::SP:
			return "SP";
		case Register::EX:
			return "EX";
		}

		return str(boost::format("<Unknown Register %04x>") % static_cast<int>(_register));
	}
}}