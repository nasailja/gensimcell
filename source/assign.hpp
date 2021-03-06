/*
Assignment function for generic simulation cell.

Copyright 2015, 2016 Ilja Honkonen
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

#ifndef GENSIMCELL_ASSIGN_HPP
#define GENSIMCELL_ASSIGN_HPP


#include "type_support.hpp"

#include "boost/mpl/contains.hpp"
#include "boost/mpl/filter_view.hpp"
#include "boost/mpl/for_each.hpp"
#include "boost/mpl/vector.hpp"


namespace gensimcell {
namespace detail {

template<class C1, class C2> struct Assigner
{
	const C1& source;
	C2& target;

	Assigner(
		const C1& given_source,
		C2& given_target
	) :
		source(given_source),
		target(given_target)
	{}

    template<typename V> void operator()(V variable)
    {
        target[variable] = source[variable];
    }
};

} // namespace detail


// forward declare Cell type used in assign
template<template<class> class Transfer_Policy, class... Variables> class Cell;

/*!
Assigns all common variables from source to target cell.

For each common variable of given cells does:
target[Variable()] = source[Variable()]

Does not change whether variables are transferred with MPI.
*/
template<
	template<class> class Transfer_Policy1,
	template<class> class Transfer_Policy2,
	class... Variables1,
	class... Variables2
> void assign(
	Cell<Transfer_Policy1, Variables1...>& target,
	const Cell<Transfer_Policy2, Variables2...>& source
) {
	using Var1_List = boost::mpl::vector<Variables1...>;
	using Var2_List = boost::mpl::vector<Variables2...>;
	using Common_Variables
		= boost::mpl::filter_view<
			Var1_List,
			boost::mpl::contains<Var2_List, boost::mpl::placeholders::_>
		>;

	boost::mpl::for_each<Common_Variables>(
		detail::Assigner<
			Cell<Transfer_Policy2, Variables2...>,
			Cell<Transfer_Policy1, Variables1...>
		>(source, target)
	);
}


} // namespace gensimcell


#endif // ifndef GENSIMCELL_ASSIGN_HPP
