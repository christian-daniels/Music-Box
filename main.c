

#include <stdio.h>
#include <math.h>


/*
 * Project3.c
 *
 * Created: 2/17/2021 3:17:22 PM
 */ 

#include <avr/io.h>
#include <math.h>
#include "avr.h"
#include "lcd.h"

/*		Note n   	2^(n/12) * 220		Get period : P = 1/F -> Get Ton(sec) ->	Ton = Toff = P/2	Get Ton(timer-arg) -> Ton/timer-arg	*/
#define A1 0		/* 2^(0/12) * 220hz = 220hz		P = 1/220 = 0.004545  Ton(sec) = 0.004545/2 = 0.002273  Ton(timer-arg) = 0.002273/0.00005 = 45*/
#define A1s 1		/* 2^(1/12) * 220hz = 233.16hz  P = 1/233.1 = 0.004290*/
#define B1 2
#define C1 3
#define C1s 4
#define D1 5
#define D1s 6
#define E1 7
#define F1 8
#define F1s 9
#define G1 10
#define G1s 11
#define A2 12	/* 2^(12/12) * 220hz = 440hz */
#define A2s 13
#define B2 14
#define C2 15
#define C2s 16
#define D2 17
#define D2s 18
#define E2 19
#define F2 20
#define F2s 21
#define G2 22
#define G2s 23

#define W 1
#define H 2
#define Q 3
#define E 4
#define WH 5
#define DH 6
#define ROOT_A 220

typedef struct{
	int frequency; // These values are CODE values - switches
	int duration;
	} Note;




const Note song[] = {
					{C1, Q},{E1, H},{B2, WH},
					{D1, Q},{F1, H},{C2, WH},
					{E1, Q}, {G1, H},{D2, WH},
					{F1, Q},{A2, H},{F2, H},{E2, W},
					{C2, Q},{B2, Q},{G1, Q},{E1, Q},
					{C2, Q},{B2, Q},{G1, Q},{E1, Q},
					{C2, Q},{B2, Q},{G1, Q},{E1, Q},
					{C2, Q},{B2, Q},{G1, Q},{E1, Q},
					{F1, Q},{E1, Q},{F1, Q},{C1, Q},
					{F1, Q},{E1, Q},{F1, E},{E1, E},{C1, Q},
					{F1, Q},{E1, Q},{F1, Q},{C1, Q},
					{F1, Q},{E1, Q},{G1, Q},{B2, Q},	
					{C2, Q},{B2, Q},{G1, Q},{E1, Q},
					{C2, Q},{B2, Q},{G1, Q},{E1, Q},
					{C2, Q},{B2, Q},{G1, Q},{E1, Q},
					{C2, Q},{B2, Q},{G1, Q},{E1, Q},
					{F1, Q},{E1, Q},{F1, Q},{G1, Q},
					{A2, Q},{G1, Q},{A2, Q},{B2, Q},
					{C2, Q},{B2, Q},{C2, Q},{D2, Q},
					{E2, DH},{D2, Q},{E2, H},{G2, Q}, {G2, Q}, {G2, W},
					{E2, Q}, {D2, Q},{B2, W},
					{A1, DH},{B1, Q},{C1, H},{D1, H},
					{E1, DH},{F1,Q},{E1,W},
					{C2, Q},{B2, Q},{G1, Q},{E1, Q},
					{C2, Q},{B2, Q},{G1, Q},{E1, Q},
					{C2, Q},{B2, Q},{G1, Q},{E1, Q},
					{C2, Q},{B2, DH}
						
};

int BPM = 60;
double timer_arg = 0.00001;
double getActualFrequency(int frequencyCode){
    
	return pow(2.0, (double)frequencyCode/12) * ROOT_A;
}

double getPeriod(int frequency){
	return 1/getActualFrequency(frequency);
}
double getActualDuration(int duration){
	// avatars love is 78bpm -> 1min/78beats in a minute -> 60/78bpm = 0.769s for each quarter note
	switch (duration)
	{
		// Quarter note
		case Q: 
			return (double)60/BPM /4;
		// Half note
		case H:
			return ((double)60/BPM) /2;
		// Dotted half note
		case DH:
			return ((double)60/BPM /4) + (((double)60/BPM) /2);
		// Whole note
		case W:
			return ((double)60/BPM) * 1;
		// Eighth note
		case E:
			return ((double)60/BPM ) /8 ;
		case WH:
			return ((double)60/BPM) * 1 + ((double)60/BPM) /2;
		default:
			return (double)60/BPM;
	}
	
}


int calcCycles(double duration, double period){
	return duration/period;
}
int pause;
void playNote(int frequency, int duration){
	int i;
	int k; /* number of cycles */
	double p = getPeriod(frequency);
	k = calcCycles(getActualDuration(duration), p);
	//k = getActualFrequency(frequency);
	//k = BPM;
	//double Ton = (p/2) / 0.00001 ;
	//double Ton = getActualFrequency(frequency);
	double Ton = round((p/2) / 0.00001);
	double Toff = Ton;
	
	//printf("FrequencyCode: %d actual frequency was %f k was %d, p was %f, Ton/Toff was %f\n", frequency, getActualFrequency(frequency),k, p, Toff);
	for (i = 0; i < k; i++){
		SET_BIT(PORTA, 0);
 		avr_wait(Ton);
 		CLR_BIT(PORTA, 0);
 		avr_wait(Toff);
	
	}
}

int is_pressed(int r, int c)
{
	// Set DDRC to 0 - input mode
	DDRC = 0;
	// Set all pins to N/C
	PORTC = 0;
	CLR_BIT(DDRC, r);
	SET_BIT(PORTC, r);
	SET_BIT(DDRC, c + 4);
	CLR_BIT(PORTC, c + 4);
	
	// Check if PINC is 1 or not at c
	if (!GET_BIT(PINC, r))
	{
		return 1;
	}
	
	return 0;
}
int get_key(){
	int r,c;
	for(r = 0; r < 4; r++){
		for(c = 0; c < 4; c++){
			if (is_pressed(r, c))
			{
				return 1 +  (r*4 + c);
			}
		}
	}
	return 0;
}

void musicDisplaySongPause(){
	lcd_clr();
	char songName1[17];
	sprintf(songName1, "|| Avatar's Love");
	char songName2[17];
	sprintf(songName2, "by J. Zuckerman");
	
	lcd_pos(0,0);
	lcd_puts(songName1);
	lcd_pos(1,0);
	lcd_puts(songName2);
}

void playSong(const Note *song, int n){
	int i, a;
	for (i = 0; i < n; i++){
		a = get_key();
		if (a == 8)
		{
			pause = 1;
			// Run pause/unpause code
			while (pause)
			{
				// display paused
				musicDisplaySongPause();
				avr_wait(500);
				a = get_key();
				if (a == 12)
				{
					musicDisplaySongDetails();
					pause = 0;
				}
			}
		}
		playNote(song[i].frequency, song[i].duration);
		
	}
	
}

void musicDisplayDash(){
	char songName1[17];
	sprintf(songName1, "Music Box Dash");
	char songName2[17];
	sprintf(songName2, "A to Play");
	
	lcd_pos(0,0);
	lcd_puts(songName1);
	lcd_pos(1,0);
	lcd_puts(songName2);
}

void musicDisplaySong(){
	lcd_clr();
	char songName1[17];
	sprintf(songName1, "Now Playing:");
	char songName2[17];
	sprintf(songName2, "Avatar's Love");
	
	lcd_pos(0,0);
	lcd_puts(songName1);
	lcd_pos(1,0);
	lcd_puts(songName2);
}

void musicDisplaySongDetails(){
	lcd_clr();
	char songName1[17];
	sprintf(songName1, "|> Avatar's Love");
	char songName2[17];
	sprintf(songName2, "by J. Zuckerman");
	
	lcd_pos(0,0);
	lcd_puts(songName1);
	lcd_pos(1,0);
	lcd_puts(songName2);
}

int main(void)
{
	
	SET_BIT(DDRA, 0);
	lcd_init();
	int k;
	while(1){
		musicDisplayDash();
		k = get_key();
		
		if (k == 4)
		{
			musicDisplaySong();
			avr_wait(1000000000);
			musicDisplaySongDetails();
			playSong(song, sizeof(song)/sizeof(song[0]));
			lcd_clr();
		}
		
		
	}
	//playSong(song, sizeof(song)/sizeof(song[0]));
	//playSong(song, 12);
	// Set Speaker to input mode in pin0
	//turnOn();
	return 0;
}



