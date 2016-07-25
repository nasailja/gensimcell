/*
+, -, * and / operators for generic simulation cell.

Copyright 2014, 2015, 2016 Ilja Honkonen
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice, this
  list of conditions and the following disclaimer in the documentation and/or
  other materials provided with the distribution.

* Neither the name of copyright holders nor the names of their contributors
  may be used to endorse or promote products derived from this software
  without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef GENSIMCELL_OPERATORS_HPP
#define GENSIMCELL_OPERATORS_HPP


#include "type_support.hpp"


namespace gensimcell {


#define GENSIMCELL_COMMA ,
#define GENSIMCELL_MAKE_FREE_OPERATOR(FREE_OPERATOR, MEMBER_OPERATOR) \
template < \
	class Cell \
> typename std::enable_if< \
	is_gensimcell<Cell>::value GENSIMCELL_COMMA \
	Cell \
>::type operator FREE_OPERATOR ( \
	Cell lhs GENSIMCELL_COMMA \
	const Cell& rhs \
) { \
	lhs MEMBER_OPERATOR rhs; \
	return lhs; \
}

GENSIMCELL_MAKE_FREE_OPERATOR(+, +=)
GENSIMCELL_MAKE_FREE_OPERATOR(-, -=)
GENSIMCELL_MAKE_FREE_OPERATOR(*, *=)
GENSIMCELL_MAKE_FREE_OPERATOR(/, /=)

#undef GENSIMCELL_MAKE_FREE_OPERATOR


#define GENSIMCELL_MAKE_FREE_OPERATOR_OTHER(FREE, MEMBER, OTHER_TYPE) \
template < \
	class Cell \
> typename std::enable_if< \
	is_gensimcell<Cell>::value GENSIMCELL_COMMA \
	Cell \
>::type operator FREE ( \
	Cell lhs GENSIMCELL_COMMA \
	const OTHER_TYPE& rhs \
) { \
	lhs MEMBER rhs; \
	return lhs; \
} \

GENSIMCELL_MAKE_FREE_OPERATOR_OTHER(+, +=, bool)
GENSIMCELL_MAKE_FREE_OPERATOR_OTHER(+, +=, char)
GENSIMCELL_MAKE_FREE_OPERATOR_OTHER(+, +=, char16_t)
GENSIMCELL_MAKE_FREE_OPERATOR_OTHER(+, +=, char32_t)
GENSIMCELL_MAKE_FREE_OPERATOR_OTHER(+, +=, wchar_t)
GENSIMCELL_MAKE_FREE_OPERATOR_OTHER(+, +=, short)
GENSIMCELL_MAKE_FREE_OPERATOR_OTHER(+, +=, int)
GENSIMCELL_MAKE_FREE_OPERATOR_OTHER(+, +=, long)
GENSIMCELL_MAKE_FREE_OPERATOR_OTHER(+, +=, long long)
GENSIMCELL_MAKE_FREE_OPERATOR_OTHER(+, +=, signed char)
GENSIMCELL_MAKE_FREE_OPERATOR_OTHER(+, +=, unsigned char)
GENSIMCELL_MAKE_FREE_OPERATOR_OTHER(+, +=, unsigned short)
GENSIMCELL_MAKE_FREE_OPERATOR_OTHER(+, +=, unsigned int)
GENSIMCELL_MAKE_FREE_OPERATOR_OTHER(+, +=, unsigned long)
GENSIMCELL_MAKE_FREE_OPERATOR_OTHER(+, +=, unsigned long long)
GENSIMCELL_MAKE_FREE_OPERATOR_OTHER(+, +=, float)
GENSIMCELL_MAKE_FREE_OPERATOR_OTHER(+, +=, double)
GENSIMCELL_MAKE_FREE_OPERATOR_OTHER(+, +=, long double)
GENSIMCELL_MAKE_FREE_OPERATOR_OTHER(-, -=, bool)
GENSIMCELL_MAKE_FREE_OPERATOR_OTHER(-, -=, char)
GENSIMCELL_MAKE_FREE_OPERATOR_OTHER(-, -=, char16_t)
GENSIMCELL_MAKE_FREE_OPERATOR_OTHER(-, -=, char32_t)
GENSIMCELL_MAKE_FREE_OPERATOR_OTHER(-, -=, wchar_t)
GENSIMCELL_MAKE_FREE_OPERATOR_OTHER(-, -=, short)
GENSIMCELL_MAKE_FREE_OPERATOR_OTHER(-, -=, int)
GENSIMCELL_MAKE_FREE_OPERATOR_OTHER(-, -=, long)
GENSIMCELL_MAKE_FREE_OPERATOR_OTHER(-, -=, long long)
GENSIMCELL_MAKE_FREE_OPERATOR_OTHER(-, -=, signed char)
GENSIMCELL_MAKE_FREE_OPERATOR_OTHER(-, -=, unsigned char)
GENSIMCELL_MAKE_FREE_OPERATOR_OTHER(-, -=, unsigned short)
GENSIMCELL_MAKE_FREE_OPERATOR_OTHER(-, -=, unsigned int)
GENSIMCELL_MAKE_FREE_OPERATOR_OTHER(-, -=, unsigned long)
GENSIMCELL_MAKE_FREE_OPERATOR_OTHER(-, -=, unsigned long long)
GENSIMCELL_MAKE_FREE_OPERATOR_OTHER(-, -=, float)
GENSIMCELL_MAKE_FREE_OPERATOR_OTHER(-, -=, double)
GENSIMCELL_MAKE_FREE_OPERATOR_OTHER(-, -=, long double)
GENSIMCELL_MAKE_FREE_OPERATOR_OTHER(*, *=, bool)
GENSIMCELL_MAKE_FREE_OPERATOR_OTHER(*, *=, char)
GENSIMCELL_MAKE_FREE_OPERATOR_OTHER(*, *=, char16_t)
GENSIMCELL_MAKE_FREE_OPERATOR_OTHER(*, *=, char32_t)
GENSIMCELL_MAKE_FREE_OPERATOR_OTHER(*, *=, wchar_t)
GENSIMCELL_MAKE_FREE_OPERATOR_OTHER(*, *=, short)
GENSIMCELL_MAKE_FREE_OPERATOR_OTHER(*, *=, int)
GENSIMCELL_MAKE_FREE_OPERATOR_OTHER(*, *=, long)
GENSIMCELL_MAKE_FREE_OPERATOR_OTHER(*, *=, long long)
GENSIMCELL_MAKE_FREE_OPERATOR_OTHER(*, *=, signed char)
GENSIMCELL_MAKE_FREE_OPERATOR_OTHER(*, *=, unsigned char)
GENSIMCELL_MAKE_FREE_OPERATOR_OTHER(*, *=, unsigned short)
GENSIMCELL_MAKE_FREE_OPERATOR_OTHER(*, *=, unsigned int)
GENSIMCELL_MAKE_FREE_OPERATOR_OTHER(*, *=, unsigned long)
GENSIMCELL_MAKE_FREE_OPERATOR_OTHER(*, *=, unsigned long long)
GENSIMCELL_MAKE_FREE_OPERATOR_OTHER(*, *=, float)
GENSIMCELL_MAKE_FREE_OPERATOR_OTHER(*, *=, double)
GENSIMCELL_MAKE_FREE_OPERATOR_OTHER(*, *=, long double)
GENSIMCELL_MAKE_FREE_OPERATOR_OTHER(/, /=, bool)
GENSIMCELL_MAKE_FREE_OPERATOR_OTHER(/, /=, char)
GENSIMCELL_MAKE_FREE_OPERATOR_OTHER(/, /=, char16_t)
GENSIMCELL_MAKE_FREE_OPERATOR_OTHER(/, /=, char32_t)
GENSIMCELL_MAKE_FREE_OPERATOR_OTHER(/, /=, wchar_t)
GENSIMCELL_MAKE_FREE_OPERATOR_OTHER(/, /=, short)
GENSIMCELL_MAKE_FREE_OPERATOR_OTHER(/, /=, int)
GENSIMCELL_MAKE_FREE_OPERATOR_OTHER(/, /=, long)
GENSIMCELL_MAKE_FREE_OPERATOR_OTHER(/, /=, long long)
GENSIMCELL_MAKE_FREE_OPERATOR_OTHER(/, /=, signed char)
GENSIMCELL_MAKE_FREE_OPERATOR_OTHER(/, /=, unsigned char)
GENSIMCELL_MAKE_FREE_OPERATOR_OTHER(/, /=, unsigned short)
GENSIMCELL_MAKE_FREE_OPERATOR_OTHER(/, /=, unsigned int)
GENSIMCELL_MAKE_FREE_OPERATOR_OTHER(/, /=, unsigned long)
GENSIMCELL_MAKE_FREE_OPERATOR_OTHER(/, /=, unsigned long long)
GENSIMCELL_MAKE_FREE_OPERATOR_OTHER(/, /=, float)
GENSIMCELL_MAKE_FREE_OPERATOR_OTHER(/, /=, double)
GENSIMCELL_MAKE_FREE_OPERATOR_OTHER(/, /=, long double)

#undef GENSIMCELL_MAKE_FREE_OPERATOR_OTHER
#undef GENSIMCELL_COMMA

} // namespace


#endif // ifndef GENSIMCELL_OPERATORS_HPP
