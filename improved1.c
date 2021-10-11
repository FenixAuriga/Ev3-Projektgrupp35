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
//int meter = 2080; //2080 counts motsvarar en meters färd
void turn();
void Search();
void approach_wall();
void drive(float,float);
void Release();
void Correction();
void retreat_wall();

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
	
	if (tacho_is_plugged(MOTOR_A, TACHO_TYPE__NONE_))
	{													 /* TACHO_TYPE__NONE_ = Alla typer av motorer */
		tacho_reset(MOTOR_A);
	}
	else
	{
		printf("Anslut platforms motor i port A. \n");
		brick_uninit();
		return (0); /* Stänger av sig om motorer ej är inkopplade */
	}

    int orientation = sensor_get_value(0, SENSOR_3, 0); /* Vad värdet är just nu */
    int gyrovalue0 = orientation -1;

    tacho_set_speed_sp ( MOTOR_BOTH, max_hastighet * 0.1 );
  
    Sleep( 1000);
  
	Search();
	approach_wall();
	tacho_run_forever(MOTOR_BOTH);
	Sleep(2000);
	tacho_stop(MOTOR_BOTH);
	Sleep(1000);
	retreat_wall();
	turn(1);
	drive(1,2.65);
	Sleep(12000);
	Release();
  
  Sleep( 6000);
 
  
    brick_uninit();
	printf( "dying...\n" );
    return ( 0 );
}

void turn(int value){
	tacho_set_speed_sp ( MOTOR_BOTH, max_hastighet * 0.1 );
	int orientation = sensor_get_value(0, SENSOR_3,0);
	int gyrovalue0 = orientation -1;
	printf("turning %d \n", value);
	Sleep( 1000 );
        if(value == 1){
            while (gyrovalue0 < orientation + 90) {
		        tacho_run_forever(MOTOR_LEFT);
		        gyrovalue0 = sensor_get_value(0, SENSOR_3, 0); // värdet från gyro
		        printf("%d gyro,\n", gyrovalue0);
	        } 
        }
        if(value != 1){
			gyrovalue0 = orientation + 1;
            while (gyrovalue0 > orientation - 90) {
		        tacho_run_forever(MOTOR_RIGHT);
		        gyrovalue0 = sensor_get_value(0, SENSOR_3, 0); // värdet från gyro
		        printf("%d gyro,\n", gyrovalue0);
			}
		}
        tacho_stop(MOTOR_BOTH); 
    
  return;
}

void Search(){

int orientation = sensor_get_value(0, SENSOR_3, 0); // nuvarande gyro värde
int gyrovalue0 = orientation -1;
int distance;
int mindistance = 5000; // initieras med stort avstånd så if satsen stämmer till att börja med
int gyromindistance;

printf("%d orientation \n", orientation);
tacho_set_speed_sp ( MOTOR_BOTH, max_hastighet * 0.1 );
  
Sleep( 1000);

  // Letar efter närmaste vägg
	while (gyrovalue0 < orientation + 360) {
		tacho_run_forever(MOTOR_LEFT);
		gyrovalue0 = sensor_get_value(0, SENSOR_3, 0); // värdet från gyro
		distance = sensor_get_value(0, SENSOR_2,0); // värdet från UL
		if(distance < mindistance){
			mindistance = distance;
			gyromindistance = gyrovalue0;
		}
		printf("%d gyro,\n", gyrovalue0);
	}
  
	tacho_stop(MOTOR_BOTH);
	
	// Vänd roboten mot minimumdistance
	
	printf("Vänd mot min_avstånd");
	printf("%d minimumdistance, %d gyro \n", mindistance, gyromindistance);
	Sleep( 1000);
	
	tacho_set_speed_sp ( MOTOR_BOTH, max_hastighet * 0.1 );
	
	if(gyrovalue0 - 9 < gyromindistance) {
		while(gyrovalue0 < gyromindistance); {
			tacho_run_forever( MOTOR_LEFT );
			gyrovalue0 = sensor_get_value(0, SENSOR_3, 0);
		}
	}
	else if( gyrovalue0 + 9 > gyromindistance) {
		while(gyrovalue0 > gyromindistance) {
			tacho_run_forever( MOTOR_RIGHT );
			gyrovalue0 = sensor_get_value(0, SENSOR_3, 0);
		}
	}
	
	
	tacho_stop(MOTOR_BOTH);
	
	printf("%d nuvarande orienation, %d mingyro \n", gyrovalue0, gyromindistance);
    
    return;
}

void approach_wall() {
	int distance = 0;
	int mode = 0;
	
	// mode = sensor_get_mode( SENSOR_2); // Sensor mode 76 verkar vara avstånd i mm läget
	// printf("%d mode active \n", mode);
	tacho_set_stop_action_hold( MOTOR_BOTH );
	tacho_set_speed_sp ( MOTOR_BOTH, max_hastighet * 0.5 );
	
    distance = sensor_get_value(0, SENSOR_2, 0);
    printf("%d distance to object \n", distance);

    while(distance > 260) {
		tacho_run_forever( MOTOR_BOTH );
		distance = sensor_get_value(0, SENSOR_2, 0);
		//printf("%d distance to object \n", distance);
    }
	
	tacho_stop( MOTOR_BOTH );
	tacho_set_stop_action_brake( MOTOR_BOTH );
  return;
}

void retreat_wall() {
	int distance = 0;
	int mode = 0;
	
	// mode = sensor_get_mode( SENSOR_2); // Sensor mode 76 verkar vara avstånd i mm läget
	// printf("%d mode active \n", mode);
	tacho_set_stop_action_hold( MOTOR_BOTH );
	tacho_set_speed_sp ( MOTOR_BOTH, max_hastighet * (-0.5) );
	
    distance = sensor_get_value(0, SENSOR_2, 0);
    printf("%d distance to object \n", distance);

    while(distance < 240) {
		tacho_run_forever( MOTOR_BOTH );
		distance = sensor_get_value(0, SENSOR_2, 0);
		//printf("%d distance to object \n", distance);
    }
	
	tacho_stop( MOTOR_BOTH );
	tacho_set_stop_action_brake( MOTOR_BOTH );
  return;
}

void drive(float value, float distance ) {
	int meter = 2080;
	int travel = distance * meter;
	tacho_set_stop_action_brake( MOTOR_BOTH );
	tacho_set_speed_sp ( MOTOR_BOTH, max_hastighet * 0.5 );
	printf("%f travel n",travel);
	
	if (value == 1) {
	printf("framåt \n");
	}
	if (value != 1) {
		travel = -1*travel;
		printf("Bakåt \n");
	}
	// Sätter hastigheten på båda motorerna till 50% av maxhastigheten
	/* Om man vill köra bakåt anger man negativ hastighet, till exempel max_hastighet * (-0.5) */
	
	
	tacho_set_position(MOTOR_BOTH, 0);
  	tacho_set_position_sp( MOTOR_BOTH, travel); /* en rotation av motorerna motsvarar ca 20cm färd eller ca 20 counts per centimeter */
	
	tacho_run_to_rel_pos(  MOTOR_BOTH ); /* kör från position till position+position_sp */
	
    return ;
}

void Release(){
	tacho_set_speed_sp(MOTOR_A, max_hastighet * 0.5); // Sätter hastigheten på båda motorerna till 50% av maxhastigheten
	/* Om man vill köra bakåt anger man negativ hastighet, till exempel max_hastighet * (-0.5) */

	tacho_set_position(MOTOR_A, 0);
	tacho_set_position_sp(MOTOR_A, -2500); /* en rotation av motorerna motsvarar ca 20cm färd eller ca 20 counts per centimeter.
	Om man vill rotera åt andra hållet så anger man negativ grader, till exempel tacho_set_position_sp(MOTOR_A, -1000); */
	/* printf("%d countperrot \n", rotation);  count_per_rot = 360 för bägge motorer */

	tacho_run_to_rel_pos(MOTOR_A); /* kör från position till position+position_sp */

	Sleep(6000);

	tacho_set_position_sp(MOTOR_A, 2500);
	tacho_run_to_rel_pos(MOTOR_A);
	Sleep(6000);
	return;
}

void Correction(){
	int distance = 0;
	tacho_set_speed_sp(MOTOR_A, max_hastighet * 0.3); // kör in i väggen
	printf("HJDFSKLJFHSDLKJFHSDLFKJHSDLKFSDLFKJH");
	tacho_run_forever(MOTOR_BOTH);
	printf("HJDFSKLJFHSDLKJFHSDLFKJHSDLKFSDLFKJH");
	Sleep(3000);
	tacho_stop(MOTOR_BOTH);
	printf("HJDFSKLJFHSDLKJFHSDLFKJHSDLKFSDLFKJH");
	sleep(4000);
	printf("HJDFSKLJFHSDLKJFHSDLFKJHSDLKFSDLFKJH");
	
	
	// mode = sensor_get_mode( SENSOR_2); // Sensor mode 76 verkar vara avstånd i mm läget
	// printf("%d mode active \n", mode);
	tacho_set_stop_action_hold( MOTOR_BOTH );
	tacho_set_speed_sp ( MOTOR_BOTH, max_hastighet * (-0.5) );
	
    distance = sensor_get_value(0, SENSOR_2, 0);
    printf("%d herjewhrjwherklwj \n", distance);

    while(distance < 240) {
		tacho_run_forever( MOTOR_BOTH );
		distance = sensor_get_value(0, SENSOR_2, 0);
		printf("%d distance to object \n", distance);
    }
	printf("test!");
	tacho_stop( MOTOR_BOTH );
	tacho_set_stop_action_brake( MOTOR_BOTH );
	
	return;
	
}
