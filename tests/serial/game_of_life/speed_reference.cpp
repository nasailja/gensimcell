/*
Reference Game of Life program implemented without the generic simulation cell.

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
#include "cstdlib"
#include "iostream"

using namespace std;
using namespace std::chrono;

struct cell_t
{
	// same layout as in generic cell version (with GCC)
	int live_neighbors = 0;
	bool is_alive = false;
};

int main(int, char**)
{
	// the game grid
	constexpr size_t
		width = 100,
		height = 100;

	array<array<cell_t, width>, height> game_grid;


	// initialize the game with a glider at upper left
	for (auto& row: game_grid) {
		for (auto& cell: row) {
			cell.is_alive = false;
			cell.live_neighbors = 0;
		}
	}
	game_grid[1][2].is_alive = true;
	game_grid[2][3].is_alive = true;
	game_grid[3][3].is_alive = true;
	game_grid[3][2].is_alive = true;
	game_grid[3][1].is_alive = true;

	const auto time_start = high_resolution_clock::now();

	constexpr size_t max_turns = 30000;
	for (size_t turn = 0; turn < max_turns; turn++) {

		// collect live neighbor counts, use periodic boundaries
		for (size_t row_i = 0; row_i < game_grid.size(); row_i++)
		for (size_t cell_i = 0; cell_i < game_grid[row_i].size(); cell_i++) {

			cell_t& current_cell = game_grid[row_i][cell_i];

			for (auto row_offset: {size_t(1), size_t(0), width - 1}) 
			for (auto cell_offset: {size_t(1), size_t(0), height - 1}) {

				if (row_offset == 0 and cell_offset == 0) {
					continue;
				}

				const cell_t& neighbor
					= game_grid[
						(row_i + row_offset) % height
					][
						(cell_i + cell_offset) % width
					];

				if (neighbor.is_alive) {
					current_cell.live_neighbors++;
				}
			}
		}

		// set new state
		for (size_t row_i = 0; row_i < game_grid.size(); row_i++)
		for (size_t cell_i = 0; cell_i < game_grid[row_i].size(); cell_i++) {

			cell_t& cell = game_grid[row_i][cell_i];
			if (cell.live_neighbors == 3) {
				cell.is_alive = true;
			} else if (cell.live_neighbors != 2) {
				cell.is_alive = false;
			}
			cell.live_neighbors = 0;
		}
	}

	const auto time_end = high_resolution_clock::now();

	size_t number_of_live_cells = 0;
	for (const auto& row: game_grid) {
	for (const auto& cell: row) {
		if (cell.is_alive) {
			number_of_live_cells++;
		}
	}}

	if (number_of_live_cells != 5) {
		std::cerr << __FILE__ << ":" << __LINE__ << " FAILED" << std::endl;
		abort();
	}

	cout << duration_cast<duration<double>>(time_end - time_start).count() << " s" << endl;

	return EXIT_SUCCESS;
}
