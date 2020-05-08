#pragma once

//include "Windows.h"
#include "./System/Type.h"
#include "./System/Template/Singleton.h"
#include "./System/Template/String.h"
#include "./System/Template/Compare.h"
#include "./System/Template/Pool.h"
#include "./System/Template/Array.h"
#include "./System/Function.h"

class System
{
public:
	virtual					~System() {}

};

using namespace ::System;
using namespace ::System::Template;
