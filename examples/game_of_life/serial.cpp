/*
Serial example program playing Conway's Game of Life.

Copyright 2013, 2014, 2015, 2016 Ilja Honkonen
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
#include "cstdlib"
#include "iostream"

#include "gensimcell.hpp"

using namespace std;

/*
Variable that records whether
the cell is alive or not
*/
struct Is_Alive
{
	using data_type = bool;
};

/*
Variable that records the
number of cell's live neighbors
*/
struct Live_Neighbors
{
	using data_type = int;
};

/*
The cell type stored in the game grid
defined by the variables it holds 
*/
using Cell_T = gensimcell::Cell<
	gensimcell::Never_Transfer,
	Is_Alive,
	Live_Neighbors
>;


/*
Prints given game of life to standard output,
using 0 for live cells and . for dead cells.
*/
template<class Game_Grid> void print_game(const Game_Grid& grid)
{
	for (const auto& row: grid) {
		for (const auto& cell: row) {
			if (cell[Is_Alive()]) {
				cout << "0";
			} else {
				cout << ".";
			}
		}
		cout << endl;
	}
	cout << endl;
}


int main(int, char**)
{
	// the game grid
	constexpr size_t
		width = 6,
		height = 6;

	array<array<Cell_T, width>, height> grid;


	// shorthand notation for referring to variables
	const Is_Alive is_alive{};
	const Live_Neighbors live_neighbors{};


	// initialize the game with a glider at upper left
	for (auto& row: grid) {
		for (auto& cell: row) {
			cell[is_alive] = false;
			cell[live_neighbors] = 0;
		}
	}
	grid[1][2][is_alive] = true;
	grid[2][3][is_alive] = true;
	grid[3][3][is_alive] = true;
	grid[3][2][is_alive] = true;
	grid[3][1][is_alive] = true;


	constexpr size_t max_turns = 4;
	for (size_t turn = 0; turn < max_turns; turn++) {

		print_game(grid);

		// collect live neighbor counts, use periodic boundaries
		for (size_t row_i = 0; row_i < grid.size(); row_i++)
		for (size_t cell_i = 0; cell_i < grid[row_i].size(); cell_i++) {

			auto& current_cell = grid[row_i][cell_i];

			for (auto row_offset: {size_t(1), size_t(0), width - 1}) 
			for (auto cell_offset: {size_t(1), size_t(0), height - 1}) {

				if (row_offset == 0 and cell_offset == 0) {
					continue;
				}

				const auto& neighbor
					= grid[
						(row_i + row_offset) % height
					][
						(cell_i + cell_offset) % width
					];

				if (neighbor[is_alive]) {
					current_cell[live_neighbors]++;
				}
			}
		}

		// set new state
		for (size_t row_i = 0; row_i < grid.size(); row_i++)
		for (size_t cell_i = 0; cell_i < grid[row_i].size(); cell_i++) {

			auto& cell = grid[row_i][cell_i];
			if (cell[live_neighbors] == 3) {
				cell[is_alive] = true;
			} else if (cell[live_neighbors] != 2) {
				cell[is_alive] = false;
			}
			cell[live_neighbors] = 0;
		}
	}

	print_game(grid);

	return EXIT_SUCCESS;
}
