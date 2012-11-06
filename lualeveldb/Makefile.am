#######################################
# The list of libraries we are building seperated by spaces.
# The 'lib_' indicates that these build products will be installed
# in the $(libdir) directory. For example /usr/lib
lib_LTLIBRARIES = liblualeveldb.la

#######################################
# Build information for each library

# Sources for lualeveldb
liblualeveldb_la_SOURCES = liblualeveldb.cc

# Linker options libTestProgram
liblualeveldb_la_LDFLAGS = -lleveldb

# Compiler options. Here we are adding the include directory
# to be searched for headers included in the source code.
liblualeveldb_la_CPPFLAGS = -I$(top_srcdir)/include