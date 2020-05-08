#pragma once

namespace Framework {		namespace Template {

template<class Type>
struct Less
{
public:
	typedef	const Type
			CTYPE;

public:
	CINT	operator ()( CTYPE & rItemA, CTYPE & rItemB ) const;

};

} /* Template */			} /* Framework */