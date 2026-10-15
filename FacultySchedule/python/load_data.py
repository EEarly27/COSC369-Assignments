
import csv
from pathlib import Path
from datetime import datetime


# Find the main FacultySchedule project folder
BASE_DIR = Path(__file__).resolve().parent.parent

# Define the locations of our CSV files
CLASS_FILE = BASE_DIR / "input" / "class_schedule.csv"
OFFICE_FILE = BASE_DIR / "input" / "faculty_info.csv"


def read_csv(file_path):
    """Read a CSV file and return its records."""

    with open(file_path, "r", encoding="utf-8-sig", newline="") as file:
        reader = csv.DictReader(file)
        return list(reader)



def convert_days(days):
    """Convert abbreviated days into full day names."""

    day_map = {
        "M": "monday",
        "T": "tuesday",
        "W": "wednesday",
        "R": "thursday",
        "F": "friday",
        "S": "saturday",
        "U": "sunday"
    }

    days = days.strip().upper()

    return [day_map[day] for day in days if day in day_map]



def convert_time(time_string):
    """Convert a time into minutes after midnight."""

    # Remove spaces and convert to uppercase
    time_string = time_string.strip().upper().replace(" ", "")

    # Convert the time into a datetime object
    time_object = datetime.strptime(time_string, "%I:%M%p")

    # Return total minutes after midnight
    return time_object.hour * 60 + time_object.minute



def prolog_string(value):
    """Safely format a string as a Prolog atom."""
    value = str(value).replace("\\", "\\\\")
    value = value.replace("'", "''")
    return f"'{value}'"


def generate_prolog_facts(classes, office_hours):
    """Generate Prolog facts from CSV records."""

    output_file = BASE_DIR / "prolog" / "faculty_facts.pl"

    with open(output_file, "w", encoding="utf-8") as file:

        file.write("% Faculty Schedule Knowledge Base\n")
        file.write("% Automatically generated from CSV files\n\n")
        
        # Allow Prolog predicates to appear in separate sections
        file.write(":- discontiguous class_schedule/6.\n")
        file.write(":- discontiguous asynchronous_course/2.\n")
        file.write(":- discontiguous office_hour/5.\n\n")

        # Generate class schedule facts
        file.write("% Class Schedule Facts\n\n")

        for record in classes:

            faculty = prolog_string(record["Faculty"].strip())
            course = prolog_string(record["Course ID"].strip())
            room = prolog_string(record["Room"].strip())

            
            # Check whether the class has a fixed meeting time
            start_text = record["Start Time"].strip()
            end_text = record["End Time"].strip()

            if "ASYNCHRONOUS" in start_text.upper():
                fact = (
                    f"asynchronous_course({faculty}, {course}).\n"
                )
                file.write(fact)
                continue

            # Convert scheduled class times
            start = convert_time(start_text)
            end = convert_time(end_text)

            days = convert_days(record["Days"])


            for day in days:
                fact = (
                    f"class_schedule({faculty}, {course}, "
                    f"{day}, {start}, {end}, {room}).\n"
                )
                file.write(fact)

        # Generate office hour facts
        file.write("\n% Faculty Office Hour Facts\n\n")

        for record in office_hours:

            faculty = prolog_string(record["Faculty Name"].strip())
            office = prolog_string(record["Office"].strip())

            start = convert_time(record["Start Time"])
            end = convert_time(record["End Time"])

            days = convert_days(record["Day"])

            for day in days:
                fact = (
                    f"office_hour({faculty}, {day}, "
                    f"{start}, {end}, {office}).\n"
                )
                file.write(fact)

    print("\nProlog facts generated successfully!")
    print("Output file:", output_file)


def main():
    # Read class schedules
    classes = read_csv(CLASS_FILE)

    # Read faculty office hours
    office_hours = read_csv(OFFICE_FILE)

    print("Faculty Schedule Data Loaded Successfully!")

    print("\nTotal Class Records:", len(classes))
    print("Total Office Hour Records:", len(office_hours))

    print("\nFirst Class Record:")
    print(classes[0] if classes else "No class records found")

    print("\nFirst Office Hour Record:")
    print(office_hours[0] if office_hours else "No office hour records found")

    
    print("\nTesting Day Conversion:")
    print("MW =", convert_days("MW"))
    print("TR =", convert_days("TR"))

    print("\nTesting Time Conversion:")
    print("4:00 PM =", convert_time("4:00 PM"))
    print("6:50 PM =", convert_time("6:50 PM"))
    print("1:00pm =", convert_time("1:00pm"))

    
    # Generate the Prolog knowledge base
    generate_prolog_facts(classes, office_hours)


if __name__ == "__main__":
    main() 
