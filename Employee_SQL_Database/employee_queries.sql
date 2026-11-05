
-- Employee and Department SQL Queries
-- Exercises 1 through 5

-- Exercise 1:
-- Find departments located in Boston
SELECT 'EXERCISE 1: Departments in Boston' AS Exercise;

SELECT DEPTNO, DNAME
FROM DEPT
WHERE LOC = 'BOSTON';


-- Exercise 2:
-- Find employees working in RESEARCH
SELECT 'EXERCISE 2: Research Employees' AS Exercise;

SELECT E.EMPNO, E.ENAME
FROM EMP AS E
JOIN DEPT AS D
    ON E.DEPTNO = D.DEPTNO
WHERE D.DNAME = 'RESEARCH';


-- Exercise 3:
-- Find managers and presidents
SELECT 'EXERCISE 3: Managers and Presidents' AS Exercise;

SELECT ENAME, JOB
FROM EMP
WHERE JOB IN ('MANAGER', 'PRESIDENT');


-- Exercise 4:
-- Find employees earning more than their managers
SELECT 'EXERCISE 4: Higher Salary Than Manager' AS Exercise;

SELECT
    E.ENAME AS Employee,
    E.SAL AS Employee_Salary,
    M.ENAME AS Manager,
    M.SAL AS Manager_Salary
FROM EMP AS E
JOIN EMP AS M
    ON E.MGR = M.EMPNO
WHERE E.SAL > M.SAL;


-- Exercise 5:
-- Find all employees directly or indirectly
-- managed by JONES
SELECT 'EXERCISE 5: Subordinates of JONES' AS Exercise;

WITH RECURSIVE Subordinates AS (

    SELECT E.EMPNO, E.ENAME, E.MGR, 1 AS Level
    FROM EMP AS E
    JOIN EMP AS M
        ON E.MGR = M.EMPNO
    WHERE M.ENAME = 'JONES'

    UNION ALL

    SELECT E.EMPNO, E.ENAME, E.MGR, S.Level + 1
    FROM EMP AS E
    JOIN Subordinates AS S
        ON E.MGR = S.EMPNO
)

SELECT ENAME, Level
FROM Subordinates
ORDER BY Level, ENAME;
