include makefiles/macosx_macports

CPPFLAGS += \
  -I source \
  -I tests \
  -I examples/game_of_life/parallel \
  -I examples/advection/parallel \
  -I examples/particle_propagation/parallel

CXXFLAGS += -std=c++0x -W -Wall -Wextra -pedantic -O3

HEADERS = \
  source/gensimcell.hpp \
  source/gensimcell_impl.hpp \
  source/get_var_mpi_datatype.hpp


%.exe: %.cpp $(HEADERS) Makefile
	@echo "CXX "$< && $(CXX) $(CPPFLAGS) $(CXXFLAGS) $< -o $@

%.tst: %.exe
	@echo "RUN "$< && $(RUN) ./$< && echo PASS && touch $@


# these require boost and MPI (for example open-mpi.org)
%.mexe: %.cpp $(HEADERS) Makefile
	@echo "MPICXX "$< && $(MPICXX) -DHAVE_MPI $(CPPFLAGS) $(CXXFLAGS) $(BOOST_CPPFLAGS) $< -o $@

%.mtst: %.mexe
	@echo "MPIRUN "$< && $(MPIRUN) ./$< && echo PASS && touch $@


# these require dccrg (the c++11 version from c++11 branch,
# https://gitorious.org/dccrg) which also requires Zoltan
%.dexe: %.cpp $(HEADERS) Makefile
	@echo "MPICXX "$< && $(MPICXX) $(CPPFLAGS) $(CXXFLAGS) $(DCCRG_CPPFLAGS) $(ZOLTAN_CPPFLAGS) $(ZOLTAN_LDFLAGS) $(ZOLTAN_LIBS) $< -o $@


EXECUTABLES = \
  tests/compile/enable_if.mexe \
  tests/compile/get_var_datatype_included.mexe \
  tests/compile/cell_included.mexe \
  tests/compile/one_variable.exe \
  tests/compile/one_variable.mexe \
  tests/compile/two_variables.exe \
  tests/compile/two_variables.mexe \
  tests/compile/many_variables.exe \
  tests/compile/many_variables.mexe \
  tests/compile/one_variable_recursive.exe \
  tests/compile/one_variable_recursive.mexe \
  tests/compile/many_variables_recursive.exe \
  tests/compile/many_variables_recursive.mexe \
  tests/compile/identical_names.exe \
  tests/compile/identical_names.mexe \
  tests/serial/get_var_datatype_std.mexe \
  tests/serial/one_variable.exe \
  tests/serial/many_variables.exe \
  tests/serial/one_variable_recursive.exe \
  tests/serial/many_variables_recursive.exe \
  tests/serial/transfer_one_cell_one_variable.mexe \
  tests/serial/transfer_one_cell_many_variables.mexe \
  tests/serial/transfer_many_cells_one_variable.mexe \
  tests/serial/transfer_many_cells_many_variables.mexe \
  tests/serial/transfer_recursive.mexe \
  tests/parallel/one_variable.mexe \
  tests/parallel/many_variables.mexe \
  tests/parallel/memory_ordering.mexe \
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
  examples/game_of_life/parallel/gol2gnuplot.dexe \
  examples/advection/parallel/main.dexe \
  examples/advection/parallel/advection2gnuplot.dexe \
  examples/particle_propagation/parallel/main.dexe \
  examples/particle_propagation/parallel/particle2gnuplot.dexe \
  examples/combined/parallel.dexe

TESTS = \
  tests/serial/get_var_datatype_std.mtst \
  tests/serial/one_variable.tst \
  tests/serial/many_variables.tst \
  tests/serial/one_variable_recursive.tst \
  tests/serial/many_variables_recursive.tst \
  tests/serial/transfer_one_cell_one_variable.mtst \
  tests/serial/transfer_one_cell_many_variables.mtst \
  tests/serial/transfer_many_cells_one_variable.mtst \
  tests/serial/transfer_many_cells_many_variables.mtst \
  tests/serial/transfer_recursive.mtst \
  tests/parallel/one_variable.mtst \
  tests/parallel/many_variables.mtst \
  tests/parallel/memory_ordering.mtst

all: test

t: test
test: $(EXECUTABLES) $(TESTS)
	@echo && echo "All tests passed."

d: data
data:
	rm -f examples/advection/serial*dat examples/advection/serial*png examples/particle_propagation/serial*dat examples/particle_propagation/serial*png examples/*/parallel/*.dc examples/*/parallel/*.png examples/*/parallel/*.dat examples/combined/*.dc examples/combined/*.dat examples/combined/*.png

c: clean
clean: data
	@echo "CLEAN" && rm -f $(EXECUTABLES) $(TESTS)
