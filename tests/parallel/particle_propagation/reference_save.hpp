/*
Saves results of parallel reference particle propagator.

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

#ifndef REFERENCE_SAVE_HPP
#define REFERENCE_SAVE_HPP

#include "cstdlib"
#include "iomanip"
#include "mpi.h"
#include "sstream"
#include "string"
#include "tuple"

#include "dccrg.hpp"
#include "dccrg_cartesian_geometry.hpp"

#include "reference_cell.hpp"

namespace particle {

void reference_save(
	dccrg::Dccrg<Reference_Cell, dccrg::Cartesian_Geometry>& grid,
	const double simulation_time,
	const std::string& prefix = std::string()
) {
	Reference_Cell::transfers
		|= Reference_Cell::transfer_types::nr_of_int_particles
		| Reference_Cell::transfer_types::vel
		| Reference_Cell::transfer_types::int_particles;

	// get the file name
	std::ostringstream time_string;
	time_string
		<< std::setw(4)
		<< std::setfill('0')
		<< size_t(simulation_time * 1000);

	// use an empty header with a sane address
	char dummy;
	std::tuple<void*, int, MPI_Datatype> header{(void*) &dummy, 0, MPI_BYTE};

	grid.save_grid_data(
		prefix + "reference_" + time_string.str() + ".dc",
		0,
		header
	);

	Reference_Cell::transfers
		&= ~Reference_Cell::transfer_types::nr_of_int_particles
		& ~Reference_Cell::transfer_types::vel
		& ~Reference_Cell::transfer_types::int_particles;
}

} // namespace

#endif // ifndef REFERENCE_SAVE_HPP
