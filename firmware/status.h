#ifndef STATUS_H
#define STATUS_H

#define GOOD    1
#define ERROR   0

/* Starts Status Thread */
void begin_status(void);

/* Set Global Status */
void set_status(bool stat);

#endif
