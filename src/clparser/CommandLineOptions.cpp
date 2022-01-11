#include "CommandLineOptions.hpp"

void clparser::CommandLineOptions::addField(clparser::IField& field)
{
	fields.emplace_back(&field);
}

std::vector<clparser::IField*> clparser::CommandLineOptions::getFields() const
{
	return fields;
}
