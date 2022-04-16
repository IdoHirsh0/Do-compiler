# Do Compiler
The Do Programming Language Compiler for Windows 💡🧐

The language I've created in order to learn about compilers and language design.


## Setup
Follow these steps to setup the **Do Programming Language Compiler for Windows** correctly on your machine.

### Install
1. Clone this repository to your machine.
2. Move the cloned project to the desired location on your machine.
	- `install.bat` will add the project's path to the user's `PATH` environment variable.
3. Make sure you have Powershell installed.
	- I used `PowerShell 7.2.2` while building the compiler.
4. Run `install.bat` in the `cmd`.
	- Adds the `bin` directory path to the user's `PATH` environment variable.

And thats it 😀

### Uninstall
1. Run `uninstall.bat` in the `cmd`.
	- Removes the `bin` directory from the user's `PATH` environment variable.
	- Deletes the project from your machine.

### Compile the compiler
Steps to rebuild the `do.exe` executable in the `bin` directory:
1. Make sure you have GCC installed.
    - I used `gcc (x86_64-win32-seh-rev0, Built by MinGW-W64 project) 8.1.0` while building the compiler.
2. `cd scripts`
3. Run `build.ps1`.


## Use
Now I'll explain how to use the do compiler in order to compile do code into x64 MASM assembly code into an executable.

### Compile Do code
Run `do /path/to/source.do [/path/to/destination.asm]` in the `cmd` to compile `.do` file into a `.asm` file.
- Run `do` with no arguments to see how to use the command.

### Compile x64 MASM assembly code
Run `ml64 filename.asm /link /subsystem:windows /entry:main` in the `cmd` to compile `.asm` file into a `.exe` executable.

## What can it do?
The following is an example do code that demonstrates some of the language features. You can use it to try out the do compiler.

```nim
prog main:
	# Variable declaration
	int result;
	int i;
	
	# Assignment
	set result = 7;
	set i = 1;

	# Loops
	while (i <= 10): # Recursive scopes
	
		# Conditions
		if (i >= 1 && i <= 10):
			set result = result * result;
		done
		else:
			# Data types
			char ch;
			set ch = 'A';
			
			# Complicated expressions: ||, &&, !, ==, !=, >, >=, <, <=, +, -, *, /, %
			set result = result + ch * i;
		done
		
		set i = i + 1;
	done
done :) # Program with a smile ;)
```


## Project structure
Now I'll explain the structure of the project. What is the purpose of each directory, and some important files.

### bin
Contains the binary files required for using the do compiler.
- `do.exe` - the do compiler.
- The rest of the files are used to compile the x64 MASM assembly code that is generated by the do compiler.

### scripts
Contains scrips for building and managing the project.
- `build.ps1` - compiles all the source files `.c .h` of the project, and generates the `do.exe` in the `bin` directory.
- `test.ps1` - builds the compiler, and runs all the tests that are in the `tests` directory. At the end outputs a summary for the passed an failed tests.
- `prepend_user_path.ps1` - adds the `bin` directory path to the user's `PATH` environment variable.
- `remove_user_path.ps1` - removes the `bin` directory path from the user's `PATH` environment variable.
- `clean.ps1` - deletes the `do.exe`, and all the `.asm` files in the project's directory.
- `code_lines.ps1` - prints how many lines of code there are in the project.

### src
Contains all the source file of the project, all the `.c .h` files.

### tests
Contains `.do` files for testing the compiler. All of those `.do` files should fail compilation with all of the possible compilation errors.

To run all of those tests against the do compiler:
1. `cd scripts`
2. Run `test.ps1`
