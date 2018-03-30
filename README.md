# MPI-CodeBlocks-ConsoleRunner
A modified ```cb_console_runner``` to allow users to run MPI programs using
multiple processes from Code::Blocks. Currently, the modifications has only
been made for the following platform(s):
  * Linux

## Pre-requisites
  * Linux (Test on Ubuntu 14.04)
  * [Code::Blocks IDE](http://www.codeblocks.org/) (Tested on Code::Blocks
    16.01)

## Usage
Append ```-mpi <path/to/mpiexec> -n <number of processes>``` to your program
arguments under *Project > Set program's arguments...* Both ```mpiexec``` and
```mpirun``` are supported. Both ```-n``` and ```-np``` can be used as well.

### Example
    -mpi /usr/bin/mpiexec -n 4

## Build
  1. Open the project file ```MPI-CodeBlocks-ConsoleRunner.cbp``` in
     Code::Blocks.
  2. Select the ```Release``` build target and click ```Build```.
  3. Check that ```cb_console_runner``` has been created in the
     ```bin/<build_target>/``` subdirectory.
  4. Backup your existing ```cb_console_runner``` (Optional but recommended)

    mv path/to/existing/cb_console_runner path/to/existing/cb_console_runner.old

  5. Make ```cb_console_runner``` and executable (If using the prebuilt version)
  
    chmod +x path/to/modified/cb_console_runner
    
  6. Copy the modified ```cb_console_runner``` to the original path of the
  existing ```cb_console_runner```.

    cp path/to/MPI-CodeBlocks-ConsoleRunner/bin/<build_target>/cb_console_runner \
    path/to/existing/cb_console_runner

# Change log
### Version 1.0.0 (2018-03-28)
  * Initial release
