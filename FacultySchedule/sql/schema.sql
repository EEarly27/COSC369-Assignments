PRAGMA foreign_keys = ON;

DROP TABLE IF EXISTS office_hours;
DROP TABLE IF EXISTS class_schedule;
DROP TABLE IF EXISTS courses;
DROP TABLE IF EXISTS faculty;

CREATE TABLE faculty (
    faculty_id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL UNIQUE,
    subject TEXT,
    office TEXT
);

CREATE TABLE courses (
    course_id TEXT PRIMARY KEY,
    course_title TEXT NOT NULL
);

CREATE TABLE class_schedule (
    schedule_id INTEGER PRIMARY KEY AUTOINCREMENT,
    faculty_id INTEGER NOT NULL,
    course_id TEXT NOT NULL,
    days TEXT NOT NULL,
    start_time TEXT NOT NULL,
    end_time TEXT NOT NULL,
    room TEXT NOT NULL,

    FOREIGN KEY (faculty_id)
        REFERENCES faculty(faculty_id),

    FOREIGN KEY (course_id)
        REFERENCES courses(course_id)
);

CREATE TABLE office_hours (
    office_hour_id INTEGER PRIMARY KEY AUTOINCREMENT,
    faculty_id INTEGER NOT NULL,
    day TEXT NOT NULL,
    start_time TEXT NOT NULL,
    end_time TEXT NOT NULL,

    FOREIGN KEY (faculty_id)
        REFERENCES faculty(faculty_id)
);