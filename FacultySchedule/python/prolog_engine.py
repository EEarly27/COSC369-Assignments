
from pathlib import Path
from pyswip import Prolog

# Locate the FacultySchedule project folder
BASE_DIR = Path(__file__).resolve().parent.parent

# Locate the generated Prolog knowledge base
FACTS_FILE = BASE_DIR / "prolog" / "faculty_facts.pl"

# Initialize Prolog
prolog = Prolog()

# Load the knowledge base
prolog.consult(FACTS_FILE.as_posix())

# Load Prolog reasoning rules
RULES_FILE = BASE_DIR / "prolog" / "faculty_rules.pl"
prolog.consult(RULES_FILE.as_posix())



def get_faculty_classes(faculty_name):
    """Retrieve a faculty member's scheduled classes."""

    # Escape apostrophes for a quoted Prolog atom
    safe_name = faculty_name.replace("\\", "\\\\")
    safe_name = safe_name.replace("'", "''")

    query = (
        f"class_schedule('{safe_name}', "
        "Course, Day, Start, End, Room)"
    )

    results = list(prolog.query(query))

    return results


def get_faculty_office_hours(faculty_name):
    """Retrieve office hours for a faculty member."""

    safe_name = faculty_name.replace("\\", "\\\\")
    safe_name = safe_name.replace("'", "''")

    query = (
        f"office_hour('{safe_name}', "
        "Day, Start, End, Office)"
    )

    results = list(prolog.query(query))

    return results


def check_faculty_availability(faculty_name, day, time):
    """Check faculty availability using Prolog rules."""

    # Import our existing time conversion function
    from load_data import convert_time

    # Convert the requested time into minutes
    time_minutes = convert_time(time)

    # Safely format the faculty name for Prolog
    safe_name = faculty_name.replace("\\", "\\\\")
    safe_name = safe_name.replace("'", "''")

    # Check whether the faculty exists
    faculty_query = f"known_faculty('{safe_name}')"

    if not list(prolog.query(faculty_query, maxresult=1)):
        return "Faculty member not found."

    # Query Prolog to determine whether faculty is busy
    busy_query = (
        f"unavailable('{safe_name}', {day.lower()}, "
        f"{time_minutes})"
    )

    is_busy = bool(list(prolog.query(busy_query, maxresult=1)))

    if is_busy:
        return f"{faculty_name} is unavailable on {day} at {time}."

    return (
        f"{faculty_name} has no known scheduling conflict "
        f"on {day} at {time}."
    )

    
def get_busy_reasons(faculty_name, day, time):
    """Find scheduled commitments causing unavailability."""

    from load_data import convert_time
    from datetime import datetime, timedelta

    time_minutes = convert_time(time)

    safe_name = faculty_name.replace("\\", "\\\\")
    safe_name = safe_name.replace("'", "''")

    query = (
        f"busy_reason('{safe_name}', {day.lower()}, "
        f"{time_minutes}, Type, Course, Room, Start, End)"
    )

    results = list(prolog.query(query))

    # Convert minutes back into readable times
    def format_time(minutes):
        return (
            datetime(2000, 1, 1) + timedelta(minutes=minutes)
        ).strftime("%I:%M %p").lstrip("0")

    # Remove duplicate commitments, if any
    unique_results = []
    seen = set()

    for record in results:
        key = (
            record["Type"],
            record["Course"],
            record["Room"],
            record["Start"],
            record["End"]
        )

        if key in seen:
            continue

        seen.add(key)

        unique_results.append({
            "type": record["Type"],
            "course": record["Course"],
            "room": record["Room"],
            "start": format_time(record["Start"]),
            "end": format_time(record["End"])
        })

    return unique_results







def main():

    faculty_name = "Adedoyin, Anthony"
    day = "monday"
    time = "5:00 PM"

    print("\nFaculty Availability Check\n")

    result = check_faculty_availability(
        faculty_name, day, time
    )

    print(result)

    reasons = get_busy_reasons(
        faculty_name, day, time
    )

    for reason in reasons:

        print("\nReason:", reason["type"])

        if reason["type"] == "class":
            print("Course:", reason["course"])
            print("Classroom:", reason["room"])
        else:
            print("Office:", reason["room"])

        print("Start:", reason["start"])
        print("End:", reason["end"])
        print("------------------------")


if __name__ == "__main__":
    main()
