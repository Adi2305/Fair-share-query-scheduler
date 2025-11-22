# Fair-share-query-scheduler
A C-based Fair Share Query Resolution System that simulates faculty–student query handling using the Round Robin Scheduling Algorithm. Includes dual priority queues, validated arrival times, and complete execution metrics like waiting time, turnaround time, and completion time.
Fair Share Query Resolution System (Round Robin Scheduling)
📌 Overview

The Fair Share Query Resolution System is a scheduling simulation implemented in C that models how a faculty expert handles live queries from faculty members and students during a limited session window (10:00 AM – 11:59 AM).
This project uses the Round Robin CPU Scheduling Algorithm, ensuring:

Fair distribution of time to all queries

No starvation for low-priority query types

Time-bound execution within a fixed session duration

Priority handling (Faculty → Student when arrival times match)

The system takes user input for queries, sorts them, and simulates the execution using Round Robin logic with a custom time quantum.

🎯 Features
✔ Handles Two Types of Queries

Faculty Queries (High Priority)

Student Queries (Normal Priority)
Faculty queries are given priority when arrival time is the same.

✔ Time-Bound Session

Session fixed to 120 minutes (10:00 AM to 11:59 AM)

Arrival time validated in HHMM format

Burst time validated to ensure it fits in session limits

✔ Round Robin Scheduling

User-provided Time Quantum

Simulates execution minute-by-minute

Tracks:

Remaining time

Completion time

Waiting time

Turnaround time

✔ Automatic Sorting

Queries are merged and sorted based on:

Arrival time

Priority (Faculty > Student if same arrival)
