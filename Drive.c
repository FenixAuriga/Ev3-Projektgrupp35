#include <stdio.h>
#include "brick.h"
#include <unistd.h>

#define Sleep( msec ) usleep(( msec ) * 1000 ) /* Definerar sleep, Sleep(1000)= 1 sekund */

#define MOTOR_RIGHT    	OUTB
#define MOTOR_LEFT    	OUTC
#define MOTOR_A    		OUTA
#define MOTOR_D    		OUTD
#define SENSOR_TOUCH	IN1
#define SENSOR_2		IN2
#define SENSOR_3		IN3
#define SENSOR_4		IN4

#define MOTOR_BOTH     	( MOTOR_LEFT | MOTOR_RIGHT ) /* Bitvis ELLER ger att båda motorerna styrs samtidigt */

int max_hastighet;         /* variabel för max hastighet på motorn */
int meter = 2080; /* tachometerns counts motsvarar ca 20 per cm */
int traveled;

int main( void )
{  
	
	if ( !brick_init()) return ( 1 ); /* Initialiserar EV3-klossen */
	printf( "*** ( EV3 ) Hello! ***\n" );
	Sleep( 2000 );
	
	if ( tacho_is_plugged( MOTOR_BOTH, TACHO_TYPE__NONE_ )) {  /* TACHO_TYPE__NONE_ = Alla typer av motorer */
        max_hastighet = tacho_get_max_speed( MOTOR_LEFT, 0 );	/* Kollar maxhastigheten som motorn kan ha */
        tacho_reset( MOTOR_BOTH );
    } else {
        printf( "Anslut vänster motor i port C,\n"
        "Anslut höger motor i port B.\n"
        );
      	brick_uninit();
        return ( 0 );  /* Stänger av sig om motorer ej är inkopplade */
    }
	
	tacho_set_stop_action_brake( MOTOR_BOTH );
	tacho_set_speed_sp( MOTOR_BOTH, max_hastighet * 0.5 );  // Sätter hastigheten på båda motorerna till 50% av maxhastigheten
	/* Om man vill köra bakåt anger man negativ hastighet, till exempel max_hastighet * (-0.5) */
	
	tacho_set_position(MOTOR_BOTH, 0);
  	tacho_set_position_sp( MOTOR_BOTH, 1 * meter); /* en rotation av motorerna motsvarar ca 20cm färd eller ca 20 counts per centimeter */
	/* printf("%d countperrot \n", rotation);  count_per_rot = 360 för bägge motorer */
	Sleep( 3000 );
	
	
	tacho_run_to_rel_pos(  MOTOR_BOTH); /* kör från position till position+position_sp */
	traveled = tacho_get_position_sp(MOTOR_RIGHT, 0);
	printf("%d counts traveled \n", traveled);
	Sleep( 4000 );
	
	
	
	brick_uninit();
	printf( "dying...\n" );
    return ( 0 );

}
