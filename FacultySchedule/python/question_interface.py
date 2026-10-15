
import re



from prolog_engine import (
    prolog,
    check_faculty_availability,
    get_busy_reasons,
    get_faculty_classes,
    get_faculty_office_hours
)





def normalize_name(name):
    """Normalize faculty names for comparison."""

    name = name.lower().strip()

    # Remove common academic titles
    name = re.sub(r"^(dr|prof|professor)\.?\s+", "", name)

    # Ignore periods and commas
    name = re.sub(r"[.,]", " ", name)

    # Remove extra spaces
    return " ".join(name.split())


def find_faculty(name):
    """Find a faculty member using different name formats."""

    results = list(prolog.query("known_faculty(Faculty)"))

    faculty_names = {
        str(record["Faculty"]) for record in results
    }

    search_name = normalize_name(name)

    matches = []

    for faculty in sorted(faculty_names):

        aliases = [faculty]

        if "," in faculty:
            last, first = faculty.split(",", 1)

            # Stored name: Last, First
            # Natural name: First Last
            aliases.append(f"{first.strip()} {last.strip()}")

            # Surname only
            aliases.append(last.strip())

        else:
            parts = faculty.split()

            if len(parts) >= 2:
                aliases.append(parts[-1])

        normalized_aliases = {
            normalize_name(alias) for alias in aliases
        }

        if search_name in normalized_aliases:
            matches.append(faculty)

    # Return only an unambiguous match
    if len(matches) == 1:
        return matches[0]

    return None



def get_faculty_courses(faculty_name):
    """Retrieve courses taught by a faculty member."""

    # Retrieve scheduled classes from Prolog
    class_records = get_faculty_classes(faculty_name)

    # Store unique course IDs
    courses = set()

    for record in class_records:
        courses.add(str(record["Course"]))

    # Also include asynchronous courses
    safe_name = faculty_name.replace("\\", "\\\\")
    safe_name = safe_name.replace("'", "''")

    query = f"asynchronous_course('{safe_name}', Course)"

    for record in prolog.query(query):
        courses.add(str(record["Course"]))

    return sorted(courses)


def get_course_rooms(faculty_name, course_id):
    """Retrieve classroom locations for a faculty course."""

    safe_name = faculty_name.replace("\\", "\\\\")
    safe_name = safe_name.replace("'", "''")

    safe_course = course_id.replace("\\", "\\\\")
    safe_course = safe_course.replace("'", "''")

    query = (
        f"class_schedule('{safe_name}', '{safe_course}', "
        "Day, Start, End, Room)"
    )

    results = list(prolog.query(query))

    # Eliminate duplicate classroom values
    rooms = set()

    for record in results:
        rooms.add(str(record["Room"]))

    return sorted(rooms)


def get_course_schedule(faculty_name, course_id):
    """Retrieve a course's scheduled days, times, and rooms."""

    safe_name = faculty_name.replace("\\", "\\\\")
    safe_name = safe_name.replace("'", "''")

    safe_course = course_id.replace("\\", "\\\\")
    safe_course = safe_course.replace("'", "''")

    query = (
        f"class_schedule('{safe_name}', '{safe_course}', "
        "Day, Start, End, Room)"
    )

    results = list(prolog.query(query))

    schedules = []
    seen = set()

    for record in results:

        key = (
            str(record["Day"]),
            record["Start"],
            record["End"],
            str(record["Room"])
        )

        if key in seen:
            continue

        seen.add(key)

        schedules.append({
            "day": str(record["Day"]).capitalize(),
            "start": format_time(record["Start"]),
            "end": format_time(record["End"]),
            "room": str(record["Room"])
        })

    return schedules


def is_asynchronous_course(faculty_name, course_id):
    """Check whether a course is asynchronous."""

    safe_name = faculty_name.replace("\\", "\\\\")
    safe_name = safe_name.replace("'", "''")

    safe_course = course_id.replace("\\", "\\\\")
    safe_course = safe_course.replace("'", "''")

    query = (
        f"asynchronous_course('{safe_name}', "
        f"'{safe_course}')"
    )

    return bool(list(prolog.query(query, maxresult=1)))


def format_time(minutes):
    """Convert minutes after midnight to 12-hour time."""

    hours, mins = divmod(int(minutes), 60)

    period = "AM" if hours < 12 else "PM"

    display_hour = hours % 12

    if display_hour == 0:
        display_hour = 12

    return f"{display_hour}:{mins:02d} {period}"




def process_question(question):
    """Interpret an availability question."""

    question = question.strip().rstrip("?")
    
    # Recognize alternative ways of asking about courses
    alternative_course_pattern = (
        r"^(?:tell me|list|show me)\s+"
        r"(?:all\s+)?(?:the\s+)?"
        r"(?:classes|courses)\s+"
        r"taught\s+by\s+(.+)$"
    )

    alternative_match = re.fullmatch(
        alternative_course_pattern,
        question,
        re.IGNORECASE
    )

    if alternative_match:

        name = alternative_match.group(1).strip()

        faculty = find_faculty(name)

        if faculty is None:
            return f"Faculty member '{name}' was not found."

        courses = get_faculty_courses(faculty)

        if not courses:
            return f"No courses found for {faculty}."

        response = [f"Courses taught by {faculty}:"]

        for course in courses:
            response.append(f"- {course}")

        return "\n".join(response)

    
    # Recognize alternative office-hour questions
    office_patterns = [
        r"^(?:show me|list|tell me)\s+(.+?)'s\s+office hours$",
        r"^what are the office hours for\s+(.+)$",
        r"^tell me (?:the )?office hours for\s+(.+)$",
        r"^show me (?:the )?office hours for\s+(.+)$",
    ]

    for pattern in office_patterns:
        match = re.fullmatch(pattern, question, re.IGNORECASE)

        if match:
            name = match.group(1).strip()
            faculty = find_faculty(name)

            if faculty is None:
                return f"Faculty member '{name}' was not found."

            hours = get_faculty_office_hours(faculty)

            if not hours:
                return f"No office hours found for {faculty}."

            day_names = {
                "monday": "Monday",
                "tuesday": "Tuesday",
                "wednesday": "Wednesday",
                "thursday": "Thursday",
                "friday": "Friday",
                "saturday": "Saturday",
                "sunday": "Sunday",
            }

            response = [f"Office hours for {faculty}:"]

            for record in hours:
                day = day_names.get(
                    str(record["Day"]).lower(),
                    str(record["Day"])
                )

                start = format_time(int(record["Start"]))
                end = format_time(int(record["End"]))
                office = record["Office"]

                response.append(
                    f"- {day}: {start} - {end}, Office {office}"
                )

            return "\n".join(response)


    # Recognize questions about scheduled course times
    schedule_pattern = (
        r"^(?:when|what time)\s+"
        r"does\s+(.+?)\s+teach\s+"
        r"([A-Za-z]{2,5}\s*\d{3}[A-Za-z]?"
        r"(?:\.\d+)?)$"
    )

    schedule_match = re.match(
        schedule_pattern, question, re.IGNORECASE
    )

    if schedule_match:

        name, course_id = schedule_match.groups()

        faculty = find_faculty(name)

        if faculty is None:
            return f"Faculty member '{name}' was not found."

        course_id = course_id.upper().strip()

        course_id = re.sub(
            r"^([A-Z]{2,5})\s*(\d)",
            r"\1 \2",
            course_id
        )

        schedules = get_course_schedule(faculty, course_id)
        
        if is_asynchronous_course(faculty, course_id):
            return (
                f"{faculty} teaches {course_id} online "
                f"asynchronously. This course has no fixed "
                f"class meeting days or times."
            )

        if not schedules:
            return (
                f"No fixed class schedule found for "
                f"{faculty} teaching {course_id}."
            )

        response = [
            f"Class schedule for {faculty}, {course_id}:"
        ]

        for schedule in schedules:
            response.append(
                f"{schedule['day']}: "
                f"{schedule['start']} - {schedule['end']}, "
                f"Room {schedule['room']}"
            )

        return "\n".join(response)

    
    # Recognize faculty office-hour questions
    office_pattern = (
        r"^(?:what|when|where)\s+"
        r"(?:are|is)\s+"
        r"(.+?)['’]s\s+"
        r"office(?:\s+hours?)?$"
    )

    office_match = re.match(
        office_pattern, question, re.IGNORECASE
    )

    if office_match:

        name = office_match.group(1)

        faculty = find_faculty(name)

        if faculty is None:
            return f"Faculty member '{name}' was not found."

        records = get_faculty_office_hours(faculty)

        if not records:
            return f"No office hours found for {faculty}."

        response = [
            f"Office hours for {faculty}:"
        ]

        # Prevent duplicate entries
        seen = set()

        for record in records:

            key = (
                str(record["Day"]),
                record["Start"],
                record["End"],
                str(record["Office"])
            )

            if key in seen:
                continue

            seen.add(key)

            day = str(record["Day"]).capitalize()
            start = format_time(record["Start"])
            end = format_time(record["End"])
            office = str(record["Office"])

            response.append(
                f"{day}: {start} - {end}, Office {office}"
            )

        return "\n".join(response)

    
    # Recognize classroom location questions
    room_pattern = (
        r"^(?:where does|which room does)\s+"
        r"(.+?)\s+teach\s+"
        r"([A-Za-z]{2,5}\s*\d{3}[A-Za-z]?"
        r"(?:\.\d+)?)"
        r"(?:\s+in)?$"
    )

    room_match = re.match(
        room_pattern, question, re.IGNORECASE
    )

    if room_match:

        name, course_id = room_match.groups()

        faculty = find_faculty(name)

        if faculty is None:
            return f"Faculty member '{name}' was not found."

        course_id = course_id.upper().strip()

        # Normalize spacing in course IDs
        course_id = re.sub(
            r"^([A-Z]{2,5})\s*(\d)",
            r"\1 \2",
            course_id
        )

        rooms = get_course_rooms(faculty, course_id)
        
        if is_asynchronous_course(faculty, course_id):
            return (
                f"{faculty} teaches {course_id} online "
                f"asynchronously. No physical classroom "
                f"is assigned."
            )

        if not rooms:
            return (
                f"No scheduled classroom found for "
                f"{faculty} teaching {course_id}."
            )

        return (
            f"{faculty} teaches {course_id} "
            f"in room(s): {', '.join(rooms)}."
        )

    # Recognize questions about courses taught
    course_pattern = (
        r"^(?:what|which)\s+"
        r"(?:classes|courses)\s+"
        r"does\s+(.+?)\s+teach$"
    )

    course_match = re.match(
        course_pattern, question, re.IGNORECASE
    )

    if course_match:

        name = course_match.group(1)

        faculty = find_faculty(name)

        if faculty is None:
            return f"Faculty member '{name}' was not found."

        courses = get_faculty_courses(faculty)

        if not courses:
            return f"No courses found for {faculty}."

        response = [
            f"\nCourses taught by {faculty}:"
        ]

        for course in courses:
            response.append(f"- {course}")

        return "\n".join(response)
    

    
    # Alternative ways of asking about faculty availability
    alternative_availability_patterns = [
        r"^is\s+(.+?)\s+free\s+"
        r"(?:on\s+)?"
        r"(monday|tuesday|wednesday|thursday|friday|saturday|sunday)"
        r"\s+at\s+(.+)$",

        r"^can i meet with\s+(.+?)\s+"
        r"(?:on\s+)?"
        r"(monday|tuesday|wednesday|thursday|friday|saturday|sunday)"
        r"\s+at\s+(.+)$",
    ]

    for alternative_pattern in alternative_availability_patterns:
        alternative_match = re.fullmatch(
            alternative_pattern, question, re.IGNORECASE
        )

        if alternative_match:
            name, day, time = alternative_match.groups()

            faculty = find_faculty(name.strip())

            if faculty is None:
                return f"Faculty member '{name}' was not found."

            
            try:
                status = check_faculty_availability(
                    faculty, day.lower(), time.strip()
                )

                reasons = get_busy_reasons(
                    faculty, day.lower(), time.strip()
                )

            except ValueError:
                return (
                    "Invalid time format. "
                    "Please use a time such as 5:00 PM."
                )

            response = [status]

            if reasons:
                response.append("\nScheduled commitments:")

                for reason in reasons:
                    if reason["type"] == "class":
                        response.append(
                            f"Class: {reason['course']}"
                        )
                        response.append(
                            f"Room: {reason['room']}"
                        )
                    else:
                        response.append("Office Hours")
                        response.append(
                            f"Office: {reason['room']}"
                        )

                    response.append(
                        f"Time: {reason['start']} - {reason['end']}"
                    )
                    response.append("------------------------")

            return "\n".join(response)


    pattern = (
        r"^is\s+(.+?)\s+"
        r"(available|unavailable)\s+"
        r"(?:on\s+)?"
        r"(monday|tuesday|wednesday|thursday|"
        r"friday|saturday|sunday)\s+"
        r"at\s+(\d{1,2}:\d{2}\s*[ap]m)$"
    )

    match = re.match(pattern, question, re.IGNORECASE)

    if not match:
        return "I do not understand that question yet."

    name, question_type, day, time = match.groups()

    faculty = find_faculty(name)

    if faculty is None:
        return f"Faculty member '{name}' was not found."

    day = day.lower()
    
    

    # Ask Prolog whether the faculty member is busy
    status = check_faculty_availability(
        faculty, day, time
    )

    # Retrieve the scheduled commitments
    reasons = get_busy_reasons(
        faculty, day, time
    )

    response = [status]

    if reasons:
        response.append("\nScheduled commitments:")

        for reason in reasons:

            if reason["type"] == "class":
                response.append(
                    f"Class: {reason['course']}"
                )
                response.append(
                    f"Room: {reason['room']}"
                )
            else:
                response.append("Office Hours")
                response.append(
                    f"Office: {reason['room']}"
                )

            response.append(
                f"Time: {reason['start']} - {reason['end']}"
            )
            response.append("------------------------")

    return "\n".join(response)


def main():
    print("\nFaculty Schedule Question System")
    print("--------------------------------")

    print("Ask a question about faculty availability.")
    print("Type 'exit' to quit.\n")

    while True:

        question = input("Enter your question: ").strip()

        if question.lower() in ("exit", "quit"):
            print("Exiting Faculty Schedule System.")
            break

        if not question:
            continue

        try:
            answer = process_question(question)
            print("\n" + answer + "\n")

        except (ValueError, RuntimeError) as error:
            print(f"\nUnable to process question: {error}\n")


if __name__ == "__main__":
    main()
