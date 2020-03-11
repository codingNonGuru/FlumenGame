#pragma once

#include "FlumenCore/Conventions.hpp"

class Settlement;

struct Road
{
	Settlement* Start_;

	Settlement* End_;

	Float Pavement_;

	Float Distance_;

	Road() {}

	Road(Settlement* Start, Settlement* End) : Start_(Start), End_(End) {}
};
