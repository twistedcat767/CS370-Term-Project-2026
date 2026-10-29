Team Green Plant Monitor: Design Document

Contributors: Yuler Orwa and Catalina E. Alcaraz

Out team is building an alaways- on plant monitor on a Raberry Pi 4. Our idea is for the plant owners who are very busy but are often away for long periods of time or most of the day, these type of plant owners want the satisfaction of a healthy flourishing plant but may find themselves asking, “What is wrong with my plant, and what should I do about it?”. Two sensors, light and soil moisture, run in isolated processes that feed a hub. The hub writes to a crash-safe log, and a supervisor keeps everything alive unattended. The analysis code is ours, and the device runs with no network connection.

Contents
1. Problem and User
2. Architecture
3. Mechanism mapping
4. Intelligence: Diagnoses that needs both sensors
5. Storage and Data
6. Failure Mode Table
7. Constraints and Subsitions
8. Evaluation Plan
9. Ownership Map
10. AI-use plan
11. Open Questions

Problem and User