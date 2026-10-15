
% Faculty Schedule Rules
% Times are represented as minutes after midnight.

% A faculty member is busy during a scheduled class.
busy(Faculty, Day, Time) :-
    class_schedule(Faculty, _, Day, Start, End, _),
    Time >= Start,
    Time < End.

% A faculty member is busy during office hours.
busy(Faculty, Day, Time) :-
    office_hour(Faculty, Day, Start, End, _),
    Time >= Start,
    Time < End.

% A faculty member is unavailable when busy.
unavailable(Faculty, Day, Time) :-
    busy(Faculty, Day, Time).

% A faculty member is potentially available when
% no scheduled commitment exists at that time.
available(Faculty, Day, Time) :-
    known_faculty(Faculty),
    \+ busy(Faculty, Day, Time).

% Identify faculty members in the knowledge base.
known_faculty(Faculty) :-
    class_schedule(Faculty, _, _, _, _, _).

known_faculty(Faculty) :-
    office_hour(Faculty, _, _, _, _).

known_faculty(Faculty) :-
    asynchronous_course(Faculty, _).


% Explain unavailability caused by a scheduled class.
busy_reason(Faculty, Day, Time, class, Course, Room, Start, End) :-
    class_schedule(Faculty, Course, Day, Start, End, Room),
    Time >= Start,
    Time < End.

% Explain unavailability caused by office hours.
busy_reason(Faculty, Day, Time, office_hour, _, Office, Start, End) :-
    office_hour(Faculty, Day, Start, End, Office),
    Time >= Start,
    Time < End.


% Explain unavailability caused by office hours.
busy_reason(Faculty, Day, Time, office_hour, office_hours, Office, Start, End) :-
    office_hour(Faculty, Day, Start, End, Office),
    Time >= Start,
    Time < End.

