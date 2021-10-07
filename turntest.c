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
void turn();

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
	
	if ( sensor_is_plugged ( SENSOR_3, SENSOR_TYPE__NONE_  )) {
		printf("sensor detekterad i port 3\n");
	} else {
		printf("Anslut sensor i port 3");
		brick_uninit();
        return ( 0 );  /* Stänger av sig om sensor ej är inkopplad */
    }
	
	if ( sensor_is_plugged ( SENSOR_2, SENSOR_TYPE__NONE_  )) {
		printf("sensor detekterad i port 2\n");
	} else {
		printf("Anslut sensor i port 2");
		brick_uninit();
        return ( 0 );  /* Stänger av sig om sensor ej är inkopplad */
    }
    int orientation = sensor_get_value(0, SENSOR_3, 0); /* Vad värdet är just nu */
    int gyrovalue0 = orientation -1;

    printf("%d orientation \n", orientation);
    tacho_set_speed_sp ( MOTOR_BOTH, max_hastighet * 0.1 );
  
    Sleep( 2000);


    turn(1);
    Sleep(3000);
    turn(0);

    /*
	while (gyrovalue0 < orientation + 360) {
		tacho_run_forever(MOTOR_LEFT);
		gyrovalue0 = sensor_get_value(0, SENSOR_3, 0); // värdet från gyro
		printf("%d gyro,\n", gyrovalue0);
	}
    */
  
	tacho_stop(MOTOR_BOTH);
  
    
    brick_uninit();
	printf( "dying...\n" );
    return ( 0 );
}

void turn(int value){
	int orientation = sensor_get_value(0, SENSOR_3,0);
	int gyrovalue0 = orientation -1;
        if(value == 1){
            while (gyrovalue0 < orientation + 90) {
		        tacho_run_forever(MOTOR_RIGHT);
		        gyrovalue0 = sensor_get_value(0, SENSOR_3, 0); // värdet från gyro
		        printf("%d gyro,\n", gyrovalue0);
	        } 
        }
        if(value == 0){
           while (gyrovalue0 < orientation - 90) {
		        tacho_run_forever(MOTOR_LEFT);
		        gyrovalue0 = sensor_get_value(0, SENSOR_3, 0); // värdet från gyro
		        printf("%d gyro,\n", gyrovalue0); 
        }
        tacho_stop(MOTOR_BOTH); 
    }
  return;
}