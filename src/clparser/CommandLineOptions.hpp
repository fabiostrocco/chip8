#pragma once

#include <vector>

#include "IField.hpp"

namespace clparser
{
	class CommandLineOptions
	{
	public:
		void addField(clparser::IField& field);
		std::vector<clparser::IField*> getFields() const;

	private:
		// I would have used std::reference_wrapper but I had some issues with Linux gcc compiler.
		std::vector<clparser::IField*> fields;
	};
}