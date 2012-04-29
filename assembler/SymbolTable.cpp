#include "SymbolTable.hpp"
#include <exception>

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

using namespace std;
using namespace dcpu::ast;
using namespace boost;
using namespace boost::algorithm;

namespace dcpu {
	DuplicateLabelError::DuplicateLabelError(const Label &duplicateLabel, const Label &existingLabel) {
		if (duplicateLabel.type == LabelType::Local) {
			message = str(format("redeclaration of local label '%' within current scope; previous declaration at %s") 
				% duplicateLabel.name % str(existingLabel.location));
		} else {
			message = str(format("redeclaration of label '%'; previous declaration at %s") 
				% duplicateLabel.name % str(existingLabel.location));
		}
	}

	DuplicateLabelError::~DuplicateLabelError() throw() {

	}

	const char *DuplicateLabelError::what() const throw() {
		return message.c_str();
	}

	NoGlobalLabelError::NoGlobalLabelError(const std::string &labelName) {
		message = str(format("Missing a global label declaration prior to local label %s") % labelName);
	}

	NoGlobalLabelError::~NoGlobalLabelError() throw() {

	}

	const char *NoGlobalLabelError::what() const throw() {
		return message.c_str();
	}

	SymbolEntry::SymbolEntry(const Label &label, std::uint16_t position)
		: label(label), position(position) {}


	SymbolEntry &SymbolTable::getLastGlobalLabel(const std::string &labelName) {
		for (auto it = _entries.rbegin(); it != _entries.rend(); it++) {
			if (it->label.type == LabelType::Global) {
				return *it;
			}
		}

		throw NoGlobalLabelError(labelName);
	}

	void SymbolTable::add(Label label, std::uint16_t position) {
		if (label.type == LabelType::Local) {
			SymbolEntry &entry = getLastGlobalLabel(label.name);

			label.name = str(format("%s%s") % entry.label.name % label.name);
		}

		auto existingEntry = _lookupTable.find(label.name);
		if (existingEntry != _lookupTable.end()) {
			throw DuplicateLabelError(label, existingEntry->second.label);
		}

		SymbolEntry entry(label, position);
		_entries.push_back(entry);
		_lookupTable.insert(pair<string, SymbolEntry&>(label.name, _entries.back()));
	}

	uint16_t *SymbolTable::lookup(const string &labelName) {
		auto labelOffset = _lookupTable.find(labelName);

		if (labelOffset == _lookupTable.end()) {
			return nullptr;
		}
		return &labelOffset->second.position;
	}

	string SymbolTable::getFullLabelName(const string &labelName) {
		if (starts_with(labelName, "..@") || !starts_with(labelName, ".")) {
			return labelName;
		}

		try {
			SymbolEntry &entry = getLastGlobalLabel(labelName);

			return str(format("%s%s") % entry.label.name % labelName);
		} catch (NoGlobalLabelError &e) {
			return labelName;
		}
	}
}