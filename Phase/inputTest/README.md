# Input/Output Test folder
This folder reproduces a typical DART simulation structure.
the phase executable expects an environment variable 'DART_LOCAL' pointing
to the folder containing this file

The simulation WON'T properly run on the present struture:
the SQL databases as well as the fluorescence scripts are missing.
 - Presently, this is only to compare the v5 and v6 XML structures
 - It can be used to make sure that phase.exe at least properly reads the XML,
   before failing when accessing the SQL databases.
 - Some minimal SQL databases, based on extraction on the much larger
   distributed ones, could be added later, as well as the python script,
   to ensure that phase fully run
