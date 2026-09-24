# Faculty Schedule Generator

## Project Overview

The Faculty Schedule Generator is a modular C and SQLite program that reads faculty class-schedule and office-hours information from two external CSV input files. The program stores the imported information in a SQLite database and automatically generates a combined faculty report containing each professor's class schedule and office hours.

The project demonstrates the use of SQL database operations, modular program organization, external file processing, static libraries, and Makefile-based build automation.

## Input Files

The program uses exactly two external input files located in the `input` directory:

1. `class_schedule.csv` - Contains the course ID, course title, meeting days, start time, end time, room, and faculty member assigned to each course.

2. `faculty_info.csv` - Contains the faculty member's name, subject, office location, office-hours day, office-hours start time, and office-hours end time.

The input files are stored locally in the project rather than downloaded from the Internet. This approach was selected because the schedule and faculty information are provided as source data for the project and do not require information from an online service. Keeping the files locally also makes the program reproducible and allows it to run without an Internet connection.

The C program accesses the two CSV files using standard file input operations with `fopen()`. The parser module reads each record, separates the CSV fields, and inserts the information into the appropriate SQLite database tables.

Office hours are read directly from `faculty_info.csv`. The program does not calculate or infer office hours from gaps in a professor's teaching schedule.

## Database Design and SQL

The project uses SQLite as its relational database management system. The database is created in `schedule.db`, and the table definitions are stored in `sql/schema.sql`.

The database contains four main tables:

- `faculty` - Stores each faculty member's name, subject, and office location.
- `courses` - Stores each unique course ID and course title.
- `class_schedule` - Stores teaching assignments, including the faculty member, course, meeting days, start time, end time, and room.
- `office_hours` - Stores each faculty member's office-hour day, start time, and end time.

The `class_schedule` table connects faculty members to courses by using foreign keys that reference the `faculty` and `courses` tables. The `office_hours` table uses a foreign key to associate each office-hour record with the appropriate faculty member.

SQL is used to create the database tables, insert imported information, retrieve schedule information, and order the records used in the generated report. The C program communicates with SQLite through the SQLite C API. Prepared SQL statements are used when inserting and retrieving data.

The database schema is recreated when the program runs so that repeated executions do not continually duplicate previously imported schedule records.

## Program Modules

The program is divided into separate C source modules and header files so that each major part of the program has a specific responsibility. This modular design makes the program easier to develop, test, maintain, and reuse.

### Main Program

- `src/main.c` - Controls the overall execution of the program. It opens the SQLite database, initializes the database schema, loads the two external input files, generates the combined faculty report, and closes the database.

### Database Module

- `src/database.c` - Contains functions for opening and closing the SQLite database and executing the SQL schema file.
- `include/database.h` - Declares the functions provided by the database module.

### Parser Module

- `src/parser.c` - Reads and parses the two CSV input files. It processes quoted CSV fields when necessary and inserts faculty, course, class-schedule, and office-hours information into the SQLite database.
- `include/parser.h` - Declares the functions used to load the two CSV input files.

### Schedule Module

- `src/schedule.c` - Contains functionality related to retrieving and formatting class-schedule information.
- `include/schedule.h` - Declares the functions provided by the schedule module.

### Office Hours Module

- `src/office_hours.c` - Contains functionality related to retrieving and formatting faculty office-hours information.
- `include/office_hours.h` - Declares the functions provided by the office-hours module.

### Output Module

- `src/output.c` - Generates the combined faculty report. It retrieves class schedules and office hours from SQLite, formats meeting-day codes, handles online asynchronous courses, orders office hours by day and time, and writes the final report.
- `include/output.h` - Declares the combined report-generation function.

Separating the program into these modules prevents all of the program logic from being placed in one source file. The modules can be compiled independently and their object files can be combined into a static library.

## Static Library

A static library is a collection of compiled object files that can be linked with another program during the build process. It allows related program modules to be grouped together and reused without placing all source code in the main program.

This project creates the static library:

`lib/libschedule.a`

The library contains the compiled object files for the major program modules:

- `database.o`
- `parser.o`
- `schedule.o`
- `office_hours.o`
- `output.o`

The `main.o` object file is intentionally kept outside the static library because it contains the program's `main()` function and serves as the entry point of the executable.

The static library is created by the Makefile using the GNU `ar` archiver:

`ar rcs lib/libschedule.a build/database.o build/parser.o build/schedule.o build/office_hours.o build/output.o`

The `r` option inserts or replaces object files in the archive, the `c` option creates the archive if necessary, and the `s` option creates an index that helps the linker locate symbols in the library.

After the library is created, the Makefile links `main.o` with the static library and SQLite:

`gcc build/main.o -Llib -lschedule -lsqlite3 -o schedule_generator.exe`

The `-Llib` option tells GCC to search the `lib` directory for libraries. The `-lschedule` option tells the linker to use `libschedule.a`, and `-lsqlite3` links the SQLite library required by the program.

Using a static library demonstrates modular software organization because the supporting program components are compiled separately, packaged together, and then linked with the main program to create the final executable.

## Makefile and Build Automation

The project uses a Makefile to automate compilation, static-library creation, linking, execution, and cleanup. Instead of manually compiling every source file each time the program changes, GNU Make examines the dependencies defined in the Makefile and rebuilds only the files affected by a change.

The Makefile compiles the C source files into object files stored in the `build` directory. The supporting object files are then combined into `lib/libschedule.a`. Finally, `build/main.o` is linked with the static library and SQLite to produce `schedule_generator.exe`.

The main Makefile targets are:

- `all` - Builds the final executable.
- `run` - Builds the executable if necessary and then runs the program to generate the faculty schedule report.
- `clean` - Removes previously generated object files, the static library, and the executable so that the project can be rebuilt from scratch.

Dependency rules are used so that GNU Make does not unnecessarily recompile files that have not changed. For example, after only `src/main.c` was modified, running `mingw32-make` recompiled `build/main.o` and relinked the executable without recompiling the other source modules.

This behavior demonstrates dependency-based build automation. It reduces unnecessary compilation and helps keep the executable synchronized with the project's source files.

## Compiling and Running the Program

The project was developed on Windows using Visual Studio Code, GCC from the MSYS2 UCRT64 environment, SQLite, and GNU Make.

### Requirements

The following tools are required:

- GCC C compiler
- SQLite 3 and the SQLite development library/header
- GNU Make
- GNU `ar` archiver

### Build the Project

From the root `FacultySchedule` directory, run:

```powershell
mingw32-make

## Project Structure

The project is organized into separate directories based on the purpose of each file:

FacultySchedule/
- `Makefile` - Defines compilation, library creation, linking, running, and cleanup rules.
- `README.md` - Documents the project, design, build process, and usage.
- `schedule.db` - SQLite database created and populated by the program.
- `schedule_generator.exe` - Final executable produced by the build process.
- `build/` - Contains compiled object (`.o`) files.
- `include/` - Contains the C header (`.h`) files for the program modules.
- `input/` - Contains the two external CSV input files.
  - `class_schedule.csv`
  - `faculty_info.csv`
- `lib/` - Contains the static library `libschedule.a`.
- `output/` - Contains the generated `faculty_schedule.txt` report.
- `sql/` - Contains `schema.sql`, which defines the SQLite database structure.
- `src/` - Contains the C source (`.c`) files.

This directory structure separates source code, headers, input data, SQL, compiled files, libraries, and generated output. This makes the project easier to understand and maintain.

## Generated Output

After the program processes the two input files and stores their information in SQLite, it generates the combined report:

`output/faculty_schedule.txt`

The report is organized by faculty member and can contain two sections:

- `CLASS SCHEDULE` - Displays the course ID, meeting time, location, and course title.
- `OFFICE HOURS` - Displays the faculty member's available office-hour days and times.

Meeting-day codes from the input data are converted into readable descriptions in the report. For example, `M` is displayed as Monday, `W` as Wednesday, `MW` as Monday & Wednesday, and `TR` as Thursday according to the supplied input-file convention.

Online asynchronous courses are displayed using their supplied description instead of being formatted as if they had conventional start and end meeting times.

When a faculty member has multiple office-hour periods on the same day, the periods are ordered chronologically. The program uses the information supplied by the input files and does not create missing schedule or office-hour information.

## Sample Output

A portion of the generated `output/faculty_schedule.txt` report has the following format:

```text
FACULTY: Dr. Stone, D. B.

CLASS SCHEDULE
------------------------------------------------------------------------
Course             Times                                      Location
------------------------------------------------------------------------
CTEC 120.170       Monday & Wednesday 12:00 PM to 3:15 AM     116
  (Prin of SecureCoding Using Jav (7R1))

CTEC 125.555       On-line Asynchronous Course                 Online
  (Intro to Python programming (7R1) )

CTEC 125.556       On-line Asynchronous Course                 Online
  (Intro to Python programming (7R2) )

CTEC 324.555       Monday & Wednesday 4:00 PM to 6:50 PM       online
  (Database Appl Dev (7R2))


OFFICE HOURS
------------------------------------------------------------
Monday       10:00am - 11:00am
Monday       2:00pm - 3:00pm
Monday       5:00pm - 6:00pm
Wednesday    10:00am - 11:00am
Wednesday    2:00pm - 3:00pm
Wednesday    5:00pm - 6:00pm
```

This output is generated from the supplied input data. Values are not automatically corrected merely because they appear unusual. For example, the `12:00 PM` to `3:15 AM` class period shown above is preserved because those values are contained in the class-schedule input file.

## Conclusion

The Faculty Schedule Generator demonstrates how C, SQLite, modular programming, static libraries, external input files, and Makefile automation can be combined in a single software project. The program reads two locally stored CSV files, organizes the information in a relational SQLite database, and produces a readable faculty class-schedule and office-hours report.

The project separates database access, input parsing, schedule processing, office-hours processing, and output generation into individual modules. These modules are compiled into object files and packaged into a static library that is linked with the main program. The Makefile automates this process and uses dependency rules to rebuild only the portions of the project that require recompilation.