
#include <stdio.h>
#include "brick.h"
#include <unistd.h>
#include <ev3_sensor.h>

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

int max_hastighet;

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
	
	if ( sensor_is_plugged ( SENSOR_2, SENSOR_TYPE__NONE_  )) {
		printf("sensor detekterad i port 2 \n");
	} else {
		printf("Anslut sensor i port 2");
		brick_uninit();
        return ( 0 );  /* Stänger av sig om sensor ej är inkopplad */
    }
	
	//size_t set_sensor_mode(SENSOR_ULTRASONIC,?)
	int distance = 0;
	int mode = 0;
	
	// mode = sensor_get_mode( SENSOR_2); // Sensor mode 76 verkar vara avstånd i cm läget
	// printf("%d mode active \n", mode);
	
	tacho_set_speed_sp ( MOTOR_BOTH, max_hastighet * 0.5 );
	
    distance = sensor_get_value(0, SENSOR_2, 0);
    printf("%d distance to object \n", distance);

    while(distance > 200) {
		tacho_run_forever( MOTOR_BOTH );
		distance = sensor_get_value(0, SENSOR_2, 0);
		printf("%d distance to object \n", distance);
    }
	
	tacho_stop( MOTOR_BOTH );

    /*if (distance <= 50){

    }
    else{

    }*/
    
    brick_uninit();
	printf( "dying...\n" );
    return ( 0 );

}
