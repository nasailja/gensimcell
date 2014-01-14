CPPFLAGS = -I source -I tests
CXXFLAGS = -std=c++0x -W -Wall -Wextra -pedantic -O3

include makefiles/macosx_macports

HEADERS = \
  source/gensimcell.hpp \
  source/gensimcell_impl.hpp \
  source/get_var_datatype.hpp

%.exe: %.cpp $(HEADERS) Makefile
	@echo "MPICXX "$< && $(MPICXX) $(CPPFLAGS) $(CXXFLAGS) $< -o $@

%.tst: %.exe
	@echo "MPIRUN "$< && $(MPIRUN) ./$< && echo PASS && touch $@

# these require dccrg (the c++11 version from c++11 branch,
# https://gitorious.org/dccrg) which also requires Zoltan
%.dexe: %.cpp $(HEADERS) Makefile
	@echo "MPICXX "$< && $(MPICXX) $(DCCRG_FLAGS) $(ZOLTAN_FLAGS) $(CPPFLAGS) $(CXXFLAGS) $< -o $@


EXECUTABLES = \
  tests/compile/get_var_datatype_included.exe \
  tests/compile/cell_included.exe \
  tests/compile/one_variable.exe \
  tests/compile/two_variables.exe \
  tests/compile/many_variables.exe \
  tests/compile/one_variable_recursive.exe \
  tests/compile/many_variables_recursive.exe \
  tests/serial/get_var_datatype_std.exe \
  tests/serial/one_variable.exe \
  tests/serial/many_variables.exe \
  tests/serial/one_variable_recursive.exe \
  tests/serial/many_variables_recursive.exe \
  tests/serial/transfer_one_cell_one_variable.exe \
  tests/serial/transfer_one_cell_many_variables.exe \
  tests/serial/transfer_many_cells_one_variable.exe \
  tests/serial/transfer_many_cells_many_variables.exe \
  tests/serial/transfer_recursive.exe \
  tests/parallel/one_variable.exe \
  tests/parallel/many_variables.exe \
  tests/compile/dccrg/enable_if.dexe \
  tests/compile/dccrg/get_cell_mpi_datatype.dexe \
  tests/compile/dccrg/included.dexe \
  tests/compile/dccrg/instantiated.dexe \
  tests/compile/dccrg/initialized.dexe \
  tests/compile/dccrg/updated.dexe \
  tests/compile/dccrg/saved.dexe \
  examples/game_of_life/serial.exe \
  examples/advection/serial.exe \
  examples/particle_propagation/serial.exe \
  examples/game_of_life/parallel/main.dexe \
  examples/game_of_life/parallel/gol2gnuplot.dexe

TESTS = \
  tests/serial/get_var_datatype_std.tst \
  tests/serial/one_variable.tst \
  tests/serial/many_variables.tst \
  tests/serial/one_variable_recursive.tst \
  tests/serial/many_variables_recursive.tst \
  tests/serial/transfer_one_cell_one_variable.tst \
  tests/serial/transfer_one_cell_many_variables.tst \
  tests/serial/transfer_many_cells_one_variable.tst \
  tests/serial/transfer_many_cells_many_variables.tst \
  tests/serial/transfer_recursive.tst \
  tests/parallel/one_variable.tst \
  tests/parallel/many_variables.tst

all: test

t: test
test: $(EXECUTABLES) $(TESTS)
	@echo && echo "All tests passed."

c: clean
clean:
	@echo "CLEAN" && rm -f $(EXECUTABLES) $(TESTS) examples/advection/serial*dat examples/advection/serial*png examples/particle_propagation/serial*dat examples/particle_propagation/serial*png examples/game_of_life/parallel/*.dc examples/game_of_life/parallel/*.png examples/game_of_life/parallel/*.dat
