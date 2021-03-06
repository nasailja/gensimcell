/*
Program testing the parallel speed of generic simulation cell with particles.

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

#include "array"
#include "chrono"
#include "cmath"
#include "cstdlib"
#include "iostream"
#include "vector"

#include "boost/lexical_cast.hpp"
#include "dccrg.hpp"
#include "dccrg_cartesian_geometry.hpp"
#include "mpi.h"

#include "reference_cell.hpp"
#include "reference_initialize.hpp"
#include "reference_save.hpp"
#include "reference_solve.hpp"

int main(int argc, char* argv[])
{
	// the cell type used by this program
	using Cell = particle::Reference_Cell;

	/*
	Set up MPI
	*/
	if (MPI_Init(&argc, &argv) != MPI_SUCCESS) {
		std::cerr << "Couldn't initialize MPI." << std::endl;
		abort();
	}

	MPI_Comm comm = MPI_COMM_WORLD;


	// intialize Zoltan
	float zoltan_version;
	if (Zoltan_Initialize(argc, argv, &zoltan_version) != ZOLTAN_OK) {
		std::cerr << "Zoltan_Initialize failed." << std::endl;
		abort();
	}


	/*
	Set up the grid in which the simulation will run
	*/
	dccrg::Dccrg<Cell, dccrg::Cartesian_Geometry> grid;

	// initialize the grid
	std::array<uint64_t, 3> grid_length = {{20, 20, 20}};
	const unsigned int neighborhood_size = 1;
	if (not grid.initialize(
		grid_length,
		comm,
		"RANDOM",
		neighborhood_size,
		0,
		false, false, false
	)) {
		std::cerr << __FILE__ << ":" << __LINE__
			<< ": Couldn't initialize grid."
			<< std::endl;
		abort();
	}

	// set the grid's geometry
	dccrg::Cartesian_Geometry::Parameters geom_params;
	geom_params.start[0] =
	geom_params.start[1] =
	geom_params.start[2] = -1;
	geom_params.level_0_cell_length[0] =
	geom_params.level_0_cell_length[1] =
	geom_params.level_0_cell_length[2] = 2.0 / grid_length[0];
	if (not grid.set_geometry(geom_params)) {
		std::cerr << __FILE__ << ":" << __LINE__
			<< ": Couldn't set grid geometry."
			<< std::endl;
		abort();
	}

	grid.balance_load();

	/*
	Simulate
	*/
	particle::reference_initialize(grid);

	const std::vector<uint64_t>
		inner_cells = grid.get_local_cells_not_on_process_boundary(),
		outer_cells = grid.get_local_cells_on_process_boundary();

	double
		simulation_time = 0,
		time_step = 0;

	const auto time_start = std::chrono::high_resolution_clock::now();

	while (simulation_time < M_PI) {

		double next_time_step = std::numeric_limits<double>::max();

		/*
		Propagate particles in outer cells first so the number
		of external particles can be sent to other processes.
		*/
		next_time_step
			= std::min(
				next_time_step,
				particle::reference_solve(time_step, outer_cells, grid)
			);

		/*
		Update number of particles in external lists of outer cells
		so that receiving processes can allocate memory for coordinates.
		*/
		Cell::transfers |= Cell::transfer_types::nr_of_ext_particles;
		Cell::transfers
			&= ~Cell::transfer_types::vel
			& ~Cell::transfer_types::ext_particles;
		grid.start_remote_neighbor_copy_updates();

		/*
		Propagate particles in inner cells while number of
		particles in external lists of outer cells is transferred.
		*/
		next_time_step
			= std::min(
				next_time_step,
				particle::reference_solve(time_step, inner_cells, grid)
			);

		/*
		Wait for particle counts in external lists of
		remote neighbors to arrive and allocate memory
		required for particle coordinates.
		*/
		grid.wait_remote_neighbor_copy_update_receives();

		particle::reference_resize(grid);

		grid.wait_remote_neighbor_copy_update_sends();

		/*
		Start transferring coordinates of particles in external lists
		of outer cells between processes.
		*/
		Cell::transfers &= ~Cell::transfer_types::nr_of_ext_particles;
		Cell::transfers
			|= Cell::transfer_types::vel
			| Cell::transfer_types::ext_particles;
		grid.start_remote_neighbor_copy_updates();

		/*
		Copy particles in external lists of neighbors
		of inner cells to internal lists of inner cells.
		*/
		particle::reference_incorporate_external(inner_cells, grid);

		/*
		Wait for particles in external lists of other
		processes' cells to arrive.
		*/
		grid.wait_remote_neighbor_copy_update_receives();

		/*
		After receiving external lists of neighbors of
		outer cells their particles can be copied to the
		internal lists of local cells.
		*/
		particle::reference_incorporate_external(outer_cells, grid);

		/*
		All local cells have incorporated the particles in
		external lists of inner cells so they can be removed.
		*/
		particle::reference_remove_external(inner_cells, grid);

		/*
		Wait for coordinates of local particles in external
		lists of outer cells to arrive to other processes.
		*/
		grid.wait_remote_neighbor_copy_update_sends();

		/*
		Once local external lists have arrived to other
		processes they can be removed on this one.
		*/
		particle::reference_remove_external(outer_cells, grid);

		simulation_time += time_step;

		MPI_Allreduce(&next_time_step, &time_step, 1, MPI_DOUBLE, MPI_MIN, comm);
		const double CFL = 0.5;
		time_step *= CFL;
	}

	const auto time_end = std::chrono::high_resolution_clock::now();
	const double
		run_time = std::chrono::duration_cast<
			std::chrono::duration<double>
		>(time_end - time_start).count();

	// print average runtime excluding two outliers
	std::vector<double> all_run_times(grid.get_comm_size(), -1);
	MPI_Gather(
		&run_time,
		1,
		MPI_DOUBLE,
		all_run_times.data(),
		1,
		MPI_DOUBLE,
		0,
		comm
	);
	if (grid.get_rank() == 0) {
		double sum = 0;
		if (grid.get_comm_size() > 3) {
			std::sort(all_run_times.begin(), all_run_times.end());
			for (size_t i = 1; i < all_run_times.size() - 1; i++) {
				sum += all_run_times[i];
			}
			std::cout << sum / (grid.get_comm_size() - 2) << " s" << std::endl;
		} else {
			for (size_t i = 0; i < all_run_times.size(); i++) {
				sum += all_run_times[i];
			}
			std::cout << sum / grid.get_comm_size() << " s" << std::endl;
		}
	}

	// save simulation to disk
	Cell::transfers
		&= ~Cell::transfer_types::nr_of_int_particles
		& ~Cell::transfer_types::nr_of_ext_particles
		& ~Cell::transfer_types::vel
		& ~Cell::transfer_types::int_particles
		& ~Cell::transfer_types::ext_particles;
	particle::reference_save(
		grid,
		simulation_time,
		"tests/parallel/particle_propagation/"
	);

	MPI_Finalize();

	return EXIT_SUCCESS;
}
