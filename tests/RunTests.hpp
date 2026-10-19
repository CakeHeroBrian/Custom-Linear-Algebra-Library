#pragma once

#include "Vector2Tests.hpp"
#include "Vector3Tests.hpp"
#include "Vector4Tests.hpp"

namespace CLAL {
	bool tests() {
		return Vector2Tests::tests() && Vector3Tests::tests() && Vector4Tests::tests();
	}
}