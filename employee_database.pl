
% =========================================
% Employee Relational Database
% DEPT Table - Slide 58
% =========================================

% dept(DepartmentNumber, DepartmentName, Location).

dept(10, 'ACCOUNTING', 'NEW YORK').
dept(20, 'RESEARCH', 'DALLAS').
dept(30, 'SALES', 'CHICAGO').
dept(40, 'OPERATIONS', 'BOSTON').


% =========================================
% EMP Table - Slide 59
% =========================================

% emp(EmployeeNumber, Name, Job, Manager, Salary, DepartmentNumber).

emp(7369, 'SMITH', 'CLERK', 7902, 800, 20).
emp(7499, 'ALLEN', 'SALESMAN', 7698, 1600, 30).
emp(7521, 'WARD', 'SALESMAN', 7698, 1250, 30).
emp(7566, 'JONES', 'MANAGER', 7839, 2975, 20).
emp(7654, 'MARTIN', 'SALESMAN', 7698, 1250, 30).
emp(7698, 'BLAKE', 'MANAGER', 7839, 2850, 30).
emp(7782, 'CLARK', 'MANAGER', 7839, 2450, 10).
emp(7788, 'SCOTT', 'ANALYST', 7566, 3000, 20).
emp(7839, 'KING', 'PRESIDENT', null, 5000, 10).
emp(7844, 'TURNER', 'SALESMAN', 7698, 1500, 30).
emp(7876, 'ADAMS', 'CLERK', 7788, 1100, 20).
emp(7900, 'JAMES', 'CLERK', 7698, 950, 30).
emp(7902, 'FORD', 'ANALYST', 7566, 3000, 20).
emp(7934, 'MILLER', 'CLERK', 7782, 1300, 10).


% =========================================
% Exercise 1 - Department in Boston
% =========================================

boston_department(DeptNo, DeptName) :-
    dept(DeptNo, DeptName, 'BOSTON').


% =========================================
% Exercise 2 - Employees in Research
% =========================================

research_employee(EmpNo, EmpName) :-
    dept(DeptNo, 'RESEARCH', _),
    emp(EmpNo, EmpName, _, _, _, DeptNo).


% =========================================
% Exercise 3 - Managers or President
% =========================================

manager_or_president(Name) :-
    emp(_, Name, 'MANAGER', _, _, _).

manager_or_president(Name) :- 
    emp(_, Name, 'PRESIDENT', _, _, _).


% =========================================
% Exercise 4 - Employees Earning More
% Than Their Direct Supervisor
% =========================================

earns_more_than_manager(EmpName, EmpSalary, ManagerName, ManagerSalary) :-
    emp(_, EmpName, _, ManagerID, EmpSalary, _),
    emp(ManagerID, ManagerName, _, _, ManagerSalary, _),
    EmpSalary > ManagerSalary.


% =========================================
% Exercise 5 - Employees Managed by JONES
% =========================================

% Base case: Employee is directly managed by ManagerID.

managed_by(EmployeeID, ManagerID) :-
    emp(EmployeeID, _, _, ManagerID, _, _).

% Recursive case: Employee is managed through another employee.

managed_by(EmployeeID, ManagerID) :-
    emp(EmployeeID, _, _, DirectManagerID, _, _),
    managed_by(DirectManagerID, ManagerID).

% Find employees directly or indirectly managed by JONES.

managed_by_jones(EmployeeName) :-
    emp(JonesID, 'JONES', _, _, _, _),
    managed_by(EmployeeID, JonesID),
    emp(EmployeeID, EmployeeName, _, _, _, _).

