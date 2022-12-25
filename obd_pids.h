#ifndef OBD_PIDS_H
#define OBD_PIDS_H

#define SET_LINE_BRK "atl1\r"
#define SET_NO_SPACES "ats0\r"
#define DEVICE_INFO "ati\r"
#define DEVICE_HARD_RESET "atz\r"
#define PID_SPEED "010D1\r" /* add 1 on the end to the set maximum */
                            /* expected number of responses        */
                            /* and so reduce the answer time       */
#define PID_RPM "010C\r"
#define PID_MAP "010B\r"    //Intake Manifold Pressure sensor
#define PID_INTAKE_MANIFOLD_PRESSURE "0x0b0\r"

#endif // OBD_PIDS_H
