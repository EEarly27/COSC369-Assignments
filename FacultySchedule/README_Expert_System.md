
# Faculty Scheduling Expert System
### Python, PySwip, and SWI-Prolog

## Project Overview

The Faculty Scheduling Expert System is a knowledge-based application that allows users to ask questions about faculty teaching schedules, office hours, classroom locations, and scheduling availability.

The system combines Python with SWI-Prolog to process scheduling information and answer user questions using logical inference.

The project extends the existing Faculty Schedule Generator by introducing a question-and-answer interface that uses scheduling data from CSV files.

## Project Objectives

The main objectives of this project are to:

- Store faculty scheduling information as Prolog facts.
- Define logical rules for determining scheduling conflicts and availability.
- Use Python and PySwip to communicate with SWI-Prolog.
- Support common natural-language questions about faculty schedules.
- Retrieve class times, classroom locations, and office hours.
- Identify when faculty members have scheduled commitments.
- Provide explanations for scheduling conflicts.

## Technologies Used

- **Python 3** — Data processing and question-answer interface.
- **SWI-Prolog** — Knowledge representation and logical inference.
- **PySwip** — Communication between Python and Prolog.
- **CSV** — Storage of faculty schedules and office-hour information.
- **Visual Studio Code** — Development environment.
- **Git and GitHub** — Version control and project hosting.

## Project Directory Structure

The expert system is integrated into the existing FacultySchedule project.

```text
COSC369-Assignments/
└── FacultySchedule/
    ├── README.md
    ├── README_Expert_System.md
    ├── input/
    │   ├── class_schedule.csv
    │   └── faculty_info.csv
    ├── python/
    │   ├── load_data.py
    │   ├── prolog_engine.py
    │   └── question_interface.py
    └── prolog/
        ├── faculty_facts.pl
        └── faculty_rules.pl
```

The directory structure above highlights the files used by the expert system. The original C/SQLite scheduling application and its other project files remain part of the repository.

## System Components

### 1. CSV Data Sources

The `input/` directory contains two CSV files:

- `class_schedule.csv` — Faculty teaching assignments, course identifiers, meeting days, class times, and classroom locations.
- `faculty_info.csv` — Faculty names, office locations, and scheduled office hours.

### 2. Python Data Loader

**File:** `python/load_data.py`

Reads the CSV files and converts scheduling information into Prolog facts.

It also converts meeting days and clock times into formats suitable for logical queries.

### 3. Prolog Knowledge Base

**File:** `prolog/faculty_facts.pl`

Contains generated scheduling facts representing faculty teaching assignments, office hours, and asynchronous courses.

### 4. Prolog Inference Rules

**File:** `prolog/faculty_rules.pl`

Defines logical rules for determining whether a faculty member has a scheduled commitment.

These rules evaluate teaching schedules and office hours to identify scheduling conflicts.

### 5. Python-Prolog Integration

**File:** `python/prolog_engine.py`

Uses PySwip to load the Prolog knowledge base, execute queries, and return results to Python.

### 6. Question-and-Answer Interface

**File:** `python/question_interface.py`

Provides a terminal-based interface where users can enter supported natural-language questions.

The interface identifies the requested information, queries the Prolog knowledge base, and displays readable answers.

## Knowledge Representation and Inference Rules

The Faculty Scheduling Expert System uses Prolog facts and rules to represent faculty scheduling information and determine scheduling conflicts.

### Prolog Facts

Scheduling data is converted from CSV records into Prolog facts.

The knowledge base contains three main fact types:

**1. Class Schedule Facts**

```prolog
class_schedule(Faculty, Course, Day, Start, End, Room).
```

These facts describe faculty teaching assignments, including course identifiers, meeting days, class times, and classroom locations.

**2. Office Hour Facts**

```prolog
office_hour(Faculty, Day, Start, End, Office).
```

These facts represent scheduled faculty office hours and office locations.

**3. Asynchronous Course Facts**

```prolog
asynchronous_course(Faculty, Course).
```

These facts represent asynchronous courses that do not have a fixed meeting time.

### Prolog Inference Rules

The expert system uses logical rules to determine whether a faculty member has a scheduled commitment.

**Class Scheduling Conflict**

```prolog
busy(Faculty, Day, Time) :-
    class_schedule(Faculty, _, Day, Start, End, _),
    Time >= Start,
    Time < End.
```

A faculty member is considered busy when the requested time falls within a scheduled class.

**Office Hour Scheduling Conflict**

```prolog
busy(Faculty, Day, Time) :-
    office_hour(Faculty, Day, Start, End, _),
    Time >= Start,
    Time < End.
```

A faculty member is also considered busy during scheduled office hours.

**Availability Rule**

```prolog
available(Faculty, Day, Time) :-
    known_faculty(Faculty),
    \+ busy(Faculty, Day, Time).
```

This rule determines whether a known faculty member has no recorded scheduling conflict at the requested time.

### Availability Interpretation

The system considers both scheduled classes and office hours to be commitments.

A faculty member is considered unavailable if either commitment overlaps the requested time.

If no conflict is found, the system reports that the faculty member has **no known scheduling conflict**.

This does not guarantee that the faculty member is available for a meeting because the knowledge base contains only the scheduling information provided in the CSV files.

### Time Interval Logic

The system uses the following condition:

```text
Start <= Time < End
```

The start time is included, but the end time is excluded.

For example, if office hours are scheduled from 1:00 PM to 2:00 PM:

- At 1:00 PM, the faculty member is busy.
- At 1:30 PM, the faculty member is busy.
- At 2:00 PM, that office-hour commitment has ended.

This prevents the system from incorrectly treating a completed class or office-hour period as an ongoing commitment.

## Supported Natural-Language Questions

The Faculty Scheduling Expert System provides a terminal-based question-and-answer interface.

Users can enter supported questions in everyday English without writing Prolog queries.

The Python interface recognizes question patterns, identifies faculty members and course identifiers, and retrieves information from the Prolog knowledge base.

### 1. Faculty Teaching Assignments

Users can ask which courses a faculty member teaches.

Example questions:

```text
What classes does Anthony Adedoyin teach?
Which courses does Adedoyin teach?
Tell me the classes taught by Adedoyin
List all courses taught by Dr Stone
```

The system retrieves matching course identifiers from the knowledge base, including asynchronous courses.

### 2. Class Meeting Times

Users can ask when a faculty member teaches a particular course.

Example:

```text
When does Anthony Adedoyin teach CTEC 350.170?
```

The system retrieves the course's scheduled meeting days, start time, end time, and classroom location.

### 3. Classroom Locations

Users can ask where a particular course is taught.

Example:

```text
Where does Anthony Adedoyin teach CTEC 350.170?
```

The system identifies the classroom associated with the requested course.

### 4. Faculty Office Hours

Users can retrieve scheduled office hours.

Example questions:

```text
What are Bo Yang's office hours?
Show me Bo Yang's office hours
List Bo Yang's office hours
What are the office hours for Bo Yang?
```

The system retrieves office-hour days, times, and office locations.

### 5. Faculty Scheduling Availability

Users can ask whether a faculty member has a scheduling conflict at a specific time.

Example questions:

```text
Is Adedoyin available Monday at 5:00 PM?
Is Adedoyin free Monday at 3:00 PM?
Is Bo Yang available Monday at 1:30 PM?
Can I meet with Bo Yang on Wednesday at 1:00 PM?
```

The system checks scheduled classes and office hours using Prolog inference rules.

If a conflict exists, the system identifies the scheduled commitment and provides relevant details.

If no conflict exists, the system reports that no known scheduling conflict was found.

### 6. Asynchronous Courses

The system recognizes asynchronous courses that do not have fixed meeting times.

Example:

```text
What classes does Dr Stone teach?
```

Asynchronous courses appear in the faculty member's teaching assignments.

Because asynchronous courses do not have fixed meeting times, they do not automatically create a time-based scheduling conflict.

### 7. Faculty Name Recognition

The system supports several faculty-name formats.

Examples:

```text
Adedoyin
Anthony Adedoyin
Adedoyin, Anthony
Dr Stone
Bo Yang
BO YANG
```

Faculty names are matched against the records stored in the knowledge base.

### 8. Unsupported Questions

The interface uses predefined natural-language patterns rather than a general-purpose language model.

Questions outside the supported patterns may produce:

```text
I do not understand that question yet.
```

The system can be extended in the future to recognize additional question structures.

## Installation and Setup

The Faculty Scheduling Expert System requires Python, SWI-Prolog, and PySwip.

The following instructions are intended for Windows users running the project in Visual Studio Code.

### Prerequisites

Install the following software:

1. **Python 3** — Runs the data loader and question interface.
2. **SWI-Prolog** — Executes the Prolog knowledge base and inference rules.
3. **PySwip** — Connects Python to SWI-Prolog.
4. **Visual Studio Code** — Used to edit and run the project.
5. **Git** — Used to clone the repository.

The project was developed using:

- Python 3.14.8
- SWI-Prolog 10.0.2
- PySwip 0.3.3

### Step 1: Clone the Repository

Open a terminal and run:

```powershell
git clone https://github.com/EEarly27/COSC369-Assignments.git
```

Navigate to the FacultySchedule project:

```powershell
cd COSC369-Assignments\FacultySchedule
```

### Step 2: Verify Python Installation

```powershell
py --version
```

This command displays the installed Python version.

### Step 3: Verify SWI-Prolog Installation

```powershell
swipl --version
```

SWI-Prolog must be installed and accessible to PySwip.

If the command is not recognized, check the SWI-Prolog installation and system PATH configuration.

### Step 4: Install PySwip

```powershell
py -m pip install pyswip==0.3.3
```

PySwip provides the connection between Python and SWI-Prolog.

### Step 5: Verify PySwip

```powershell
py -c "from pyswip import Prolog; print('PySwip imported successfully')"
```

Expected output:

```text
PySwip imported successfully
```

### Step 6: Generate the Prolog Knowledge Base

From the `FacultySchedule` directory, run:

```powershell
py .\python\load_data.py
```

The Python data loader reads the CSV files and generates:

```text
prolog/faculty_facts.pl
```

This file contains the faculty scheduling facts used by the expert system.

Regenerate the facts whenever the source CSV scheduling data changes.

### Step 7: Start the Question Interface

Run:

```powershell
py .\python\question_interface.py
```

The application starts an interactive terminal session where users can enter supported scheduling questions.

Example:

```text
Is Adedoyin available Monday at 5:00 PM?
```

The system queries Prolog to determine whether a scheduled class or office-hour commitment exists.

### Step 8: Exit the Application

Type:

```text
exit
```

This ends the question-and-answer session.

## Troubleshooting

### Python Command Not Recognized

On Windows, use the Python launcher:

```powershell
py
```

instead of `python` if the latter is unavailable.

### PySwip Import Error

Install PySwip into the Python environment used to run the application:

```powershell
py -m pip install pyswip==0.3.3
```

### SWI-Prolog Connection Error

Verify that SWI-Prolog is installed correctly and that its required libraries are accessible to PySwip.

### Missing Knowledge Base

If `faculty_facts.pl` is missing or outdated, regenerate it:

```powershell
py .\python\load_data.py
```

Run the commands from the `FacultySchedule` project directory so the documented relative paths resolve correctly.

## Testing and Validation

The Faculty Scheduling Expert System was tested through its Python question-and-answer interface to verify that scheduling data could be retrieved and evaluated using Prolog.

### Functional Testing

The following capabilities were tested during development:

| Test | Expected Behavior | Result |
|---|---|---|
| Faculty teaching assignments | Retrieve courses taught by a faculty member | Passed |
| Class scheduling conflict | Identify a conflict during a scheduled class | Passed |
| No known scheduling conflict | Report no conflict when no commitment overlaps | Passed |
| Office-hour conflict | Identify a conflict during scheduled office hours | Passed |
| Start-time boundary | Treat the start of a commitment as busy | Passed |
| End-time boundary | Treat the end of a commitment as no longer busy | Passed |
| Asynchronous courses | Include asynchronous courses without assigning fixed meeting times | Passed |
| Unknown faculty | Reject faculty names not found in the knowledge base | Passed |
| Invalid time input | Reject an invalid 12-hour clock value | Passed |
| Duplicate surname check | Identify potential surname collisions in the dataset | No duplicates identified |
| Python syntax validation | Compile all Python modules without syntax errors | Passed |

### Example: Class Scheduling Conflict

Question:

```text
Is Adedoyin available Monday at 5:00 PM?
```

Result:

```text
Adedoyin, Anthony is unavailable on monday at 5:00 PM.
```

The system identifies scheduled teaching commitments that overlap the requested time.

### Example: Office-Hour Conflict

Question:

```text
Is Bo Yang available Monday at 1:30 PM?
```

The system identifies a scheduling conflict because Bo Yang has office hours from 1:00 PM to 2:00 PM.

### Example: Time Boundary Validation

Questions:

```text
Is Bo Yang available Monday at 1:00 PM?
Is Bo Yang available Monday at 2:00 PM?
```

The system correctly identifies a conflict at 1:00 PM and no known conflict at 2:00 PM.

This verifies the rule:

```text
Start <= Time < End
```

### Example: Asynchronous Course Handling

Question:

```text
What classes does Dr Stone teach?
```

The system retrieves both scheduled and asynchronous course assignments.

Asynchronous courses without fixed meeting times are not treated as time-based conflicts.

### Python Syntax Validation

The following command was used to check all three Python modules:

```powershell
py -m py_compile .\python\load_data.py .\python\prolog_engine.py .\python\question_interface.py
```

The command completed without syntax errors.

## System Limitations

Although the expert system supports several common scheduling questions, it has some limitations.

### 1. Pattern-Based Natural-Language Processing

The interface uses regular expressions to recognize supported question formats.

It does not use a general-purpose natural-language understanding model, so questions outside the implemented patterns may not be recognized.

### 2. Scheduling Data Dependency

The system relies on the information contained in the CSV files.

If scheduling records are missing, incorrect, or outdated, the answers may be incomplete or inaccurate.

### 3. Availability Does Not Guarantee a Meeting

The system checks only scheduled classes and office hours.

A result indicating no known scheduling conflict does not guarantee that a faculty member is available for an appointment.

### 4. Asynchronous Courses

Asynchronous courses are included in teaching assignments but do not create fixed-time scheduling conflicts unless meeting times are provided.

### 5. Terminal-Based Interface

The current application uses a command-line interface rather than a graphical or web-based interface.

## Possible Future Improvements

Potential enhancements include:

- A graphical or web-based question-and-answer interface.
- Support for additional natural-language question formats.
- More detailed handling of ambiguous faculty names.
- Integration with a live scheduling database.
- Automated regression tests.
- Improved scheduling-data validation.

These are possible future improvements and are not part of the current implementation.
