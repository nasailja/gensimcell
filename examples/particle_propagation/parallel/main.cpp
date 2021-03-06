/*
Parallel program propagating particles in a prescribed velocity field.

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

//! see ../serial.cpp and ../advection/parallel/* for basics

#include "array"
#include "boost/lexical_cast.hpp"
#include "cmath"
#include "cstdlib"
#include "iostream"
#include "vector"

#include "dccrg.hpp"
#include "dccrg_cartesian_geometry.hpp"
#include "mpi.h" // must be included before gensimcell
#include "gensimcell.hpp"

#include "particle_initialize.hpp"
#include "particle_save.hpp"
#include "particle_solve.hpp"
#include "particle_variables.hpp"

int main(int argc, char* argv[])
{
	// the cell type used by this program
	using Cell = particle::Cell;

	/*
	Set up MPI
	*/
	if (MPI_Init(&argc, &argv) != MPI_SUCCESS) {
		std::cerr << "Coudln't initialize MPI." << std::endl;
		abort();
	}

	MPI_Comm comm = MPI_COMM_WORLD;

	int rank = 0, comm_size = 0;
	MPI_Comm_rank(comm, &rank);
	MPI_Comm_size(comm, &comm_size);


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
	std::array<uint64_t, 3> grid_length = {{20, 20, 1}};
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
	geom_params.start[1] = -1;
	geom_params.start[2] = -1.0 / grid_length[0];
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

	particle::initialize<
		Cell,
		particle::Number_Of_Internal_Particles,
		particle::Number_Of_External_Particles,
		particle::Velocity,
		particle::Internal_Particles,
		particle::External_Particles
	>(grid);

	const std::vector<uint64_t>
		inner_cells = grid.get_local_cells_not_on_process_boundary(),
		outer_cells = grid.get_local_cells_on_process_boundary();

	const double particle_save_interval = 0.1;

	double particle_next_save = 0;

	double
		simulation_time = 0,
		time_step = 0;
	while (simulation_time < M_PI) {

		double next_time_step = std::numeric_limits<double>::max();

		/*
		Save simulation to disk
		*/
		Cell::set_transfer_all(
			false,
			particle::Number_Of_Internal_Particles(),
			particle::Number_Of_External_Particles(),
			particle::Velocity(),
			particle::Internal_Particles(),
			particle::External_Particles()
		);

		if (particle_next_save <= simulation_time) {
			particle_next_save += particle_save_interval;

			particle::save<
				Cell,
				particle::Number_Of_Internal_Particles,
				particle::Velocity,
				particle::Internal_Particles
			>(grid, simulation_time);
		}


		if (simulation_time >= M_PI) {
			break;
		}


		/*
		Propagate particles in outer cells first so the number of
		resulting external particles can be sent to other processes.
		*/
		next_time_step
			= std::min(
				next_time_step,
				particle::solve<
					Cell,
					particle::Number_Of_Internal_Particles,
					particle::Number_Of_External_Particles,
					particle::Velocity,
					particle::Internal_Particles,
					particle::External_Particles
				>(time_step, outer_cells, grid)
			);

		/*
		Update number of particles in external lists of remote neighbors
		so that receiving processes can allocate memory for coordinates.
		*/
		Cell::set_transfer_all(true, particle::Number_Of_External_Particles());
		grid.start_remote_neighbor_copy_updates();

		/*
		Propagate particles in inner cells while number of particles
		in external lists of remote neighbors is transferred.
		*/
		next_time_step
			= std::min(
				next_time_step,
				particle::solve<
					Cell,
					particle::Number_Of_Internal_Particles,
					particle::Number_Of_External_Particles,
					particle::Velocity,
					particle::Internal_Particles,
					particle::External_Particles
				>(time_step, inner_cells, grid)
			);

		/*
		Wait for particle counts in external lists of
		remote neighbors to arrive and allocate memory
		required for particle coordinates.
		*/
		grid.wait_remote_neighbor_copy_update_receives();
		particle::resize_receiving_containers<
			Cell,
			particle::Number_Of_External_Particles,
			particle::External_Particles
		>(grid);

		grid.wait_remote_neighbor_copy_update_sends();

		/*
		Start transferring coordinates of particles in external lists
		of outer cells between processes.
		*/
		Cell::set_transfer_all(false, particle::Number_Of_External_Particles());
		Cell::set_transfer_all(
			true,
			particle::Velocity(),
			particle::External_Particles()
		);
		grid.start_remote_neighbor_copy_updates();

		/*
		Copy particles in external lists of neighbors
		of inner cells to internal lists of inner cells.
		*/
		particle::incorporate_external_particles<
			Cell,
			particle::Number_Of_Internal_Particles,
			particle::Internal_Particles,
			particle::External_Particles
		>(inner_cells, grid);

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
		particle::incorporate_external_particles<
			Cell,
			particle::Number_Of_Internal_Particles,
			particle::Internal_Particles,
			particle::External_Particles
		>(outer_cells, grid);

		/*
		All local cells have incorporated the particles in
		external lists of inner cells so they can be removed.
		*/
		particle::remove_external_particles<
			Cell,
			particle::Number_Of_External_Particles,
			particle::External_Particles
		>(inner_cells, grid);

		/*
		Wait for coordinates of local particles in external
		lists of outer cells to arrive to other processes.
		*/
		grid.wait_remote_neighbor_copy_update_sends();
		Cell::set_transfer_all(
			false,
			particle::Velocity(),
			particle::External_Particles()
		);

		/*
		Once local external lists have arrived to other
		processes they can be removed on this one.
		*/
		particle::remove_external_particles<
			Cell,
			particle::Number_Of_External_Particles,
			particle::External_Particles
		>(outer_cells, grid);

		simulation_time += time_step;

		MPI_Allreduce(&next_time_step, &time_step, 1, MPI_DOUBLE, MPI_MIN, comm);
		const double CFL = 0.5;
		time_step *= CFL;
	}

	MPI_Finalize();

	return EXIT_SUCCESS;
}
