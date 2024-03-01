# 🏥 Medical-Office-Simulator

## Task description:
 A given number of patient threads will be created that will wait for the release of some resources representing doctors. Clients will occupy the resource doctor for a random period that does not exceed a given limit. Every patient will be generated at a random interval for a period of time. After consultation, the patient  will display the waiting time and consultation time.

## Implementation:
 To represent the doctors, I used a struct, that contains a *bool* to check whether the doctor is available or not, a **mutex** to make sure that *only one* patient has access to the medical office, and an *integer* to keep the number of the patient.

 Because the number of patients cannot exceed the number of doctors *at a time*, the mutexes will help us to be certain that a doctor will not be disturbed during the current consultation (*a thread cannot access the mutex until the mutex is not freed by the current thread*).
