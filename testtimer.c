<<<<<<< HEAD
=======
/***************************************************************\
 *
 *              Copyright (c) 2007 SCFI Automation, Inc.
 * Code taken over by georges@sancosme.net after the author passed away and
 * published under GNU GPLv3
 *
 * Original Author      : (Deceased)
 * Current Maintainer   : gsancosme (georges@sancosme.net)
 * Maintained Since     : 13.01.2025
 * Created On           : 04.06.2007
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 *
 *******************************************************************/

>>>>>>> 6e6eccb (Update headers of c files to include GPLv3 and new maintainer)
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

#define MAXLONGVAL 2147383600

long glTimerMsec = 0;
long glTimerMday = 0;
long glTarMsec;
long glTarMday;
long glExpireCount = 0;

void timer_handler(int signum)
{
    if(++glTimerMsec >= MAXLONGVAL)
    {
	++glTimerMday;
	glTimerMsec = 0;
    }
}

int expiredTimer(void)
{
    if(++glExpireCount <= 125) return 0;
    glExpireCount = 0;

    if(glTimerMday < glTarMday) return 0;
    if(glTimerMsec < glTarMsec) return 0;
    return 1;
}    
void delay(long msec)
{

    glTarMsec = glTimerMsec + (msec/2);
    glTarMday = glTimerMday;

    if (glTarMsec >= MAXLONGVAL)
    {
	++glTarMday;
    	glTarMsec -= MAXLONGVAL;
    }

    printf("beg timer msec=%ld  target=%ld\n",glTimerMsec, glTarMsec);
    while(!expiredTimer());
    printf("end timer msec=%ld  target=%ld\n",glTimerMsec, glTarMsec);
}
    
int main()
{
    struct sigaction sa;
    struct itimerval timer;

    /* intall timer handler as the signal handler for SIGVTALRM. */
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = &timer_handler;
//    sigaction(SIGPROF, &sa, NULL);
//    sigaction(SIGVTALRM, &sa, NULL);
    sigaction(SIGALRM, &sa, NULL);

    // configure the timer to expire after 1 msec
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = 1000;

    // and at every 1 msec after that
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 1000;

    // start a REAL timer
    setitimer(ITIMER_REAL, &timer, NULL);
//    setitimer(ITIMER_VIRTUAL, &timer, NULL);
//    setitimer(ITIMER_PROF, &timer, NULL);

    int i=0;
    while(1)
    {
	delay(10000);
    }
}
