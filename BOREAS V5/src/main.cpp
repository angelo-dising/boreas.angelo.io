#include<Arduino.h>
#include <Wire.h>
#include "SparkFun_SCD30_Arduino_Library.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 32 
	float euler_number = 2.71828182;	//NO CAMBIAR NI USAR #DEFINE de otra forma no funcionara
	float euler_number_negative = 0.36787944117;	//NO CAMBIAR NI USAR #DEFINE de otra forma no funcionara
	float CO2 = 0; //Particulas por millon NO CAMBIAR VALOR⚠
	float Temperature = 0;//Temperatura NO CAMBIAR VALOR⚠
	float Humedad = 0;//Humedad NO CAMBIAR VALOR ⚠
	float co2_outdoor = 400;/*Si tienes un valor mas acercado cambiar con precaucion solo es la cantidad de 
	co2 que hay afuera, normalmente solo es entre 412 y 420*/
	float co2_peack = 0;//Cantidad maxima de co2 detectada es usada para calibrar el sensor y proximamente calibracion pasiva y continua NO
	//NO CAMBIAR VALOR ⚠
	float co2_diference = 0;//Diferencia de co2 entre el co2 de el exterior y el interior NO CAMBIAR ⚠
	float Decay63_percentage = 0;//Variable usada para calibrar NO CAMBIAR ⚠
	float decay63time = 0;//Variable usada para calibrar NO CAMBIAR ⚠
	float ventilation_rate = 0;//Variable usada para estimar probabilidad NO CAMBIAR ⚠
	float Probabilidad = 0;//Variable que almacena la probabilidad NO CAMBIAR ⚠
	float quanta_inhaled_per_person = 0;//Variable usada para estimar probabilidad NO CAMBIAR ⚠
	float average_quanta_concentration = 0;//Variable usada para estimar probabilidad NO CAMBIAR ⚠
	float Breathing_rate = 0.52; //Cambiar dependiendo de las personas que hay en la habitacion, ⚠ CONSULTAR INFORMACION EN EL README!!!
	float Mask_eficiency = 0.5;//Se debe de obtener el dato de la eficiencia de la mascara que las personas usan!!
	float Inhalation_mask_eficiency = 0.3;//Se debe de obtener el dato de la eficiencia de la mascara que las personas usan!!
	float People_whit_mask = 1;//Porcentaje de personas con mascara siendo 1 el 100% 0.5 el 50% etc..
	float Net_emission_Rate = 0;//Variable usada para estimar probabilidad NO CAMBIAR ⚠
	float Total_order_loss_rate = 0;//Variable usada para estimar probabilidad NO CAMBIAR ⚠
	float volume = 6;//se debe estimar el volumen de la habitacion en la que se va a estar, ES UNA VARIABLE IMPORTANTE !!!⚠
	float Quanta_Exalation_rate_of_posible_infected = 25;
	//⬆ Cambiar dependiendo de las personas que hay en la habitacion, ⚠ CONSULTAR INFORMACION EN EL README!!!
	float Event_Duration = 1;//Duracion de los sucesos que pasan en la habiatcion, la medida es en horas⚠
	float Time_past = 0;//Usado para calibrar NO CAMBIAR⚠
	int calibration_button = 21;/*Numero del pin al que el boton va a ser conectado, esta definido en pin 21 para 
	la placa esp32 dev kit v1 */
	bool a = false;//No cambiar
	int w = 0;//No cambiar
	int e = 0;//No cambiar
	bool Sensor_ausente = false;//No cambiar
	Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);/*Se usa para cambiar el tamaño de la pantalla usando SCREEN_WIDTH para
	el largo de la pantalla  y SCREEN_HEIGHT para la altura*/
	SCD30 airSensor;//Inicia el sensor de co2

	/*Los sigueinte arrays son bitmaps que contienen una imagen que se usa pra mostrar informacion en la pantalla
	como logos y texto pre diseñado, sientete libre de cambiar el bitmap a tu gusto usando la siguiente pagina: 
	https://javl.github.io/image2cpp/   En la que puedes transformar imagenes en bitmaps usables para la pantalla ssd1306
	no olvides que dependiendo de la version de pantalla que tienes debees cambiar el alto de la pantalla ya sea 128px por 32px
	o la version de 128px por 64px */
const unsigned char BOREAS [] PROGMEM = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03,
0xff, 0xff, 0x7f, 0xff, 0x87, 0xff, 0xfe, 0x3f, 0xff, 0xe0, 0x7e, 0x00, 0xff,
0xff, 0xe0, 0x03, 0xff, 0xff, 0x7f, 0xff, 0xc7, 0xff, 0xfe, 0x3f, 0xff, 0xe0, 
0x7e, 0x00, 0xff, 0xff, 0xe0, 0x03, 0xff, 0xfe, 0x3f, 0xff, 0xc7, 0xff, 0xfe, 
0x3f, 0xff, 0xe0, 0x7e, 0x00, 0xff, 0xff, 0xe0, 0x03, 0xff, 0xfe, 0x3f, 0xff, 
0xc7, 0xff, 0xfe, 0x3f, 0xff, 0xe0, 0x7f, 0x00, 0xff, 0xff, 0xc0, 0x01, 0xfc, 
0x3e, 0x3f, 0x87, 0xc3, 0xf0, 0xfe, 0x3f, 0x80, 0x00, 0x7f, 0x00, 0xfe, 0x1f, 
0xc0, 0x01, 0xf8, 0x3e, 0x3f, 0x87, 0xc3, 0xf0, 0xfe, 0x3f, 0x80, 0x00, 0x3f, 
0x00, 0x7e, 0x0f, 0xc0, 0x03, 0xf8, 0x7c, 0x7f, 0x0f, 0xe7, 0xf0, 0xfe, 0x3f, 
0x80, 0x00, 0x7f, 0x00, 0xfe, 0x0e, 0x00, 0x03, 0xf8, 0xf8, 0x7f, 0x0f, 0xe7, 
0xf0, 0xfe, 0x7f, 0x80, 0x00, 0xff, 0x00, 0xfe, 0x00, 0x00, 0x03, 0xf9, 0xf0, 
0x7f, 0x0f, 0xe7, 0xf0, 0xfc, 0x7f, 0x00, 0x00, 0xff, 0x01, 0xfc, 0x00, 0x00, 
0x07, 0xf3, 0xe0, 0x7f, 0x0f, 0xcf, 0xe0, 0xfc, 0x7f, 0x00, 0x01, 0xff, 0x01, 
0xfc, 0x00, 0x00, 0x07, 0xf7, 0xfc, 0xfe, 0x0f, 0xcf, 0xff, 0xfc, 0x7f, 0xfe, 
0x01, 0xff, 0x81, 0xff, 0xff, 0x80, 0x07, 0xff, 0xfc, 0xfe, 0x1f, 0xcf, 0xff,
0xfc, 0x7f, 0xfe, 0x03, 0xff, 0x81, 0xff, 0xff, 0x80, 0x07, 0xff, 0xfc, 0xfe, 
0x1f, 0xcf, 0xff, 0xf8, 0xff, 0xfe, 0x07, 0xff, 0x81, 0xff, 0xff, 0x00, 0x0f, 
0xff, 0xfc, 0xfe, 0x1f, 0x9f, 0xff, 0xf8, 0xff, 0xfc, 0x07, 0xff, 0x83, 0xff, 
0xff, 0x00, 0x0f, 0xfd, 0xfd, 0xfe, 0x1f, 0x9f, 0xff, 0xf8, 0xff, 0xfc, 0x0f, 
0xff, 0x80, 0x00, 0x7f, 0x00, 0x0f, 0xf9, 0xfd, 0xfc, 0x3f, 0x9f, 0xc7, 0xe0, 
0xfe, 0x00, 0x0f, 0xdf, 0xc0, 0x00, 0x7f, 0x00, 0x0f, 0xf1, 0xf9, 0xfc, 0x3f, 
0x9f, 0xc7, 0xe1, 0xfc, 0x00, 0x1f, 0xcf, 0xc0, 0x00, 0xfe, 0x00, 0x0f, 0xe1, 
0xf9, 0xfc, 0x3f, 0x9f, 0xc7, 0xe1, 0xfc, 0x00, 0x1f, 0x8f, 0xc0, 0xe0, 0xfe, 
0x00, 0x1f, 0xc3, 0xf9, 0xfc, 0x3f, 0x3f, 0x87, 0xf1, 0xfc, 0x00, 0x3f, 0x8f, 
0xc7, 0xe0, 0xfe, 0x00, 0x1f, 0xc3, 0xfb, 0xf8, 0x3f, 0x3f, 0x83, 0xf1, 0xfc, 
0x00, 0x3f, 0x0f, 0xc7, 0xf0, 0xfe, 0x00, 0x1f, 0xff, 0xf9, 0xff, 0xff, 0x3f, 
0x83, 0xf1, 0xff, 0xff, 0x7f, 0xff, 0xc7, 0xff, 0xfc, 0x00, 0x1f, 0xff, 0xf1, 
0xff, 0xff, 0x3f, 0x83, 0xf1, 0xff, 0xff, 0x7f, 0xff, 0xe7, 0xff, 0xfc, 0x00, 
0x3f, 0xff, 0xf1, 0xff, 0xff, 0x7f, 0x03, 0xf1, 0xff, 0xff, 0xff, 0xff, 0xef, 
0xff, 0xf8, 0x00, 0x3f, 0xff, 0xf1, 0xff, 0xfe, 0x7f, 0x03, 0xf1, 0xff, 0xff, 
0xff, 0xff, 0xef, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00};

const unsigned char NO_SENSOR_LOGO [] PROGMEM = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xc3, 0xdf, 0xfe, 
0x00, 0x7f, 0xff, 0xff, 0xff, 0xf0, 0xef, 0xff, 0xdf, 0xfc, 0x7f, 0xfc, 0x0f, 
0xe3, 0xdf, 0xfe, 0x00, 0x7f, 0xff, 0xff, 0xf7, 0xf0, 0xef, 0xff, 0xdf, 0xfe, 
0x7f, 0xfc, 0x0f, 0xe3, 0xdf, 0xfe, 0x00, 0x7f, 0xfe, 0xff, 0xf7, 0xf1, 0xef, 
0xff, 0xdf, 0xfe, 0x7f, 0xfe, 0x0f, 0xe3, 0xdf, 0xfe, 0x00, 0x7f, 0xfe, 0xff, 
0xf3, 0xf1, 0xef, 0xff, 0xdf, 0xfe, 0x7f, 0xfe, 0x07, 0xe7, 0x9f, 0xfe, 0x00, 
0x7f, 0xfc, 0xff, 0xf3, 0xf1, 0xe7, 0xff, 0xdf, 0xfe, 0x3f, 0xfe, 0x07, 0xe7, 
0x9f, 0x1e, 0x00, 0x3c, 0x3c, 0xf8, 0x03, 0xf1, 0xe7, 0xc7, 0x9f, 0x1e, 0x3c, 
0x3c, 0x0f, 0xe7, 0x9f, 0x1e, 0x00, 0x7c, 0x3c, 0xf8, 0x07, 0xf9, 0xe7, 0xc7, 
0x9f, 0x1e, 0x7c, 0x7c, 0x0f, 0xe7, 0x9f, 0x1e, 0x00, 0x7c, 0x21, 0xf8, 0x07, 
0xfb, 0xcf, 0x84, 0x1f, 0x1e, 0x7c, 0x7c, 0x0f, 0xe7, 0xbe, 0x1e, 0x00, 0x7c, 
0x01, 0xf0, 0x07, 0xfb, 0xcf, 0x80, 0x1f, 0x1f, 0x7c, 0x7c, 0x0f, 0xe7, 0xbe, 
0x1e, 0x00, 0x78, 0x01, 0xf0, 0x07, 0xfb, 0xcf, 0x80, 0x3f, 0x3e, 0x7c, 0x7c, 
0x1f, 0xf7, 0xbe, 0x3e, 0x00, 0x78, 0x01, 0xf0, 0x07, 0xfb, 0xce, 0x00, 0x3e, 
0x3e, 0xfc, 0x7c, 0x1f, 0xff, 0x3e, 0x3e, 0x00, 0x7f, 0xf9, 0xff, 0x87, 0xfb, 
0xdf, 0xff, 0xbe, 0x3e, 0xff, 0xfc, 0x1f, 0xff, 0x3e, 0x3e, 0x00, 0xff, 0xf9, 
0xff, 0x87, 0xfb, 0xdf, 0xff, 0x3e, 0x3e, 0xff, 0xf8, 0x1f, 0xff, 0x3e, 0x3e, 
0x00, 0xff, 0xf9, 0xff, 0x87, 0xfb, 0x9f, 0xff, 0x3e, 0x3e, 0xff, 0xf8, 0x1f, 
0xff, 0x3e, 0x3e, 0x00, 0xff, 0xfb, 0xff, 0x87, 0xfb, 0x9f, 0xff, 0x3e, 0x3e, 
0xff, 0xf8, 0x1f, 0xff, 0x7e, 0x3c, 0x00, 0xff, 0xfb, 0xff, 0x8f, 0xff, 0x9f, 
0xff, 0x3e, 0x7c, 0xff, 0xf8, 0x1f, 0xff, 0x7c, 0x3c, 0x00, 0x00, 0xfb, 0xff, 
0x8f, 0xff, 0x80, 0x1f, 0x7c, 0x7c, 0xff, 0xf0, 0x1f, 0xff, 0x7c, 0x3c, 0x00, 
0x00, 0xfb, 0xe0, 0x0f, 0xff, 0x80, 0x1e, 0x7c, 0x7c, 0xf9, 0xe0, 0x3e, 0xff, 
0x7c, 0x7c, 0x00, 0x00, 0xf3, 0xe0, 0x0f, 0xff, 0x80, 0x3e, 0x7c, 0x7c, 0xf1, 
0xe0, 0x3e, 0xff, 0x7c, 0x7c, 0x00, 0x71, 0xf3, 0xe0, 0x1f, 0xff, 0x8e, 0x3e, 
0x7c, 0x7d, 0xf1, 0xe0, 0x3e, 0xfe, 0x7c, 0x7c, 0x01, 0xf1, 0xf7, 0xe0, 0x1f, 
0xbf, 0xbe, 0x3e, 0x7c, 0x7d, 0xf1, 0xe0, 0x3e, 0xfe, 0x78, 0x7c, 0x01, 0xf1, 
0xf7, 0xc0, 0x1f, 0x3f, 0x3e, 0x3e, 0x78, 0x79, 0xf1, 0xf0, 0x3e, 0x7c, 0x7f, 
0xfc, 0x01, 0xff, 0xf7, 0xff, 0xdf, 0x3f, 0x3f, 0xfe, 0x7f, 0xf9, 0xf1, 0xf0, 
0x3e, 0x7c, 0x7f, 0xf8, 0x01, 0xff, 0xe7, 0xff, 0xdf, 0x3f, 0x3f, 0xfc, 0x7f, 
0xf9, 0xf1, 0xf0, 0x3e, 0x7c, 0x7f, 0xf8, 0x01, 0xff, 0xe7, 0xff, 0xdf, 0x3f, 
0x3f, 0xfc, 0x7f, 0xfb, 0xe0, 0xf0, 0x7e, 0x7c, 0x7f, 0xf8, 0x03, 0xff, 0xe3, 
0xff, 0xdf, 0x3f, 0x7f, 0xfc, 0x7f, 0xfb, 0xe0, 0xf0, 0x7c, 0x7c, 0x7f, 0xf8, 
0x03, 0xff, 0xc3, 0xff, 0xdf, 0x3f, 0x7f, 0xfc, 0x7f, 0xfb, 0xe0, 0xf0, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00};

				const unsigned char ALERT_LOGO [] PROGMEM = {
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x3c, 0x20, 0x01, 
0xff, 0x80, 0x00, 0x00, 0x00, 0x0f, 0x10, 0x00, 0x20, 0x03, 0x80, 0x03, 0xf0, 
0x1c, 0x20, 0x01, 0xff, 0x80, 0x00, 0x00, 0x08, 0x0f, 0x10, 0x00, 0x20, 0x01, 
0x80, 0x03, 0xf0, 0x1c, 0x20, 0x01, 0xff, 0x80, 0x01, 0x00, 0x08, 0x0e, 0x10, 
0x00, 0x20, 0x01, 0x80, 0x01, 0xf0, 0x1c, 0x20, 0x01, 0xff, 0x80, 0x01, 0x00, 
0x0c, 0x0e, 0x10, 0x00, 0x20, 0x01, 0x80, 0x01, 0xf8, 0x18, 0x60, 0x01, 0xff, 
0x80, 0x03, 0x00, 0x0c, 0x0e, 0x18, 0x00, 0x20, 0x01, 0xc0, 0x01, 0xf8, 0x18, 
0x60, 0xe1, 0xff, 0xc3, 0xc3, 0x07, 0xfc, 0x0e, 0x18, 0x38, 0x60, 0xe1, 0xc3, 
0xc3, 0xf0, 0x18, 0x60, 0xe1, 0xff, 0x83, 0xc3, 0x07, 0xf8, 0x06, 0x18, 0x38, 
0x60, 0xe1, 0x83, 0x83, 0xf0, 0x18, 0x60, 0xe1, 0xff, 0x83, 0xde, 0x07, 0xf8, 
0x04, 0x30, 0x7b, 0xe0, 0xe1, 0x83, 0x83, 0xf0, 0x18, 0x41, 0xe1, 0xff, 0x83, 
0xfe, 0x0f, 0xf8, 0x04, 0x30, 0x7f, 0xe0, 0xe0, 0x83, 0x83, 0xf0, 0x18, 0x41, 
0xe1, 0xff, 0x87, 0xfe, 0x0f, 0xf8, 0x04, 0x30, 0x7f, 0xc0, 0xc1, 0x83, 0x83, 
0xe0, 0x08, 0x41, 0xc1, 0xff, 0x87, 0xfe, 0x0f, 0xf8, 0x04, 0x31, 0xff, 0xc1, 
0xc1, 0x03, 0x83, 0xe0, 0x00, 0xc1, 0xc1, 0xff, 0x80, 0x06, 0x00, 0x78, 0x04, 
0x20, 0x00, 0x41, 0xc1, 0x00, 0x03, 0xe0, 0x00, 0xc1, 0xc1, 0xff, 0x00, 0x06, 
0x00, 0x78, 0x04, 0x20, 0x00, 0xc1, 0xc1, 0x00, 0x07, 0xe0, 0x00, 0xc1, 0xc1, 
0xff, 0x00, 0x06, 0x00, 0x78, 0x04, 0x60, 0x00, 0xc1, 0xc1, 0x00, 0x07, 0xe0, 
0x00, 0xc1, 0xc1, 0xff, 0x00, 0x04, 0x00, 0x78, 0x04, 0x60, 0x00, 0xc1, 0xc1, 
0x00, 0x07, 0xe0, 0x00, 0x81, 0xc3, 0xff, 0x00, 0x04, 0x00, 0x70, 0x00, 0x60, 
0x00, 0xc1, 0x83, 0x00, 0x07, 0xe0, 0x00, 0x83, 0xc3, 0xff, 0xff, 0x04, 0x00, 
0x70, 0x00, 0x7f, 0xe0, 0x83, 0x83, 0x00, 0x0f, 0xe0, 0x00, 0x83, 0xc3, 0xff, 
0xff, 0x04, 0x1f, 0xf0, 0x00, 0x7f, 0xe1, 0x83, 0x83, 0x06, 0x1f, 0xc1, 0x00, 
0x83, 0x83, 0xff, 0xff, 0x0c, 0x1f, 0xf0, 0x00, 0x7f, 0xc1, 0x83, 0x83, 0x0e, 
0x1f, 0xc1, 0x00, 0x83, 0x83, 0xff, 0x8e, 0x0c, 0x1f, 0xe0, 0x00, 0x71, 0xc1, 
0x83, 0x82, 0x0e, 0x1f, 0xc1, 0x01, 0x83, 0x83, 0xfe, 0x0e, 0x08, 0x1f, 0xe0, 
0x40, 0x41, 0xc1, 0x83, 0x82, 0x0e, 0x1f, 0xc1, 0x01, 0x87, 0x83, 0xfe, 0x0e, 
0x08, 0x3f, 0xe0, 0xc0, 0xc1, 0xc1, 0x87, 0x86, 0x0e, 0x0f, 0xc1, 0x83, 0x80, 
0x03, 0xfe, 0x00, 0x08, 0x00, 0x20, 0xc0, 0xc0, 0x01, 0x80, 0x06, 0x0e, 0x0f, 
0xc1, 0x83, 0x80, 0x07, 0xfe, 0x00, 0x18, 0x00, 0x20, 0xc0, 0xc0, 0x03, 0x80, 
0x06, 0x0e, 0x0f, 0xc1, 0x83, 0x80, 0x07, 0xfe, 0x00, 0x18, 0x00, 0x20, 0xc0, 
0xc0, 0x03, 0x80, 0x04, 0x1f, 0x0f, 0x81, 0x83, 0x80, 0x07, 0xfc, 0x00, 0x1c, 
0x00, 0x20, 0xc0, 0x80, 0x03, 0x80, 0x04, 0x1f, 0x0f, 0x83, 0x83, 0x80, 0x07, 
0xfc, 0x00, 0x3c, 0x00, 0x20, 0xc0, 0x80, 0x03, 0x80, 0x04, 0x1f, 0x0f, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff};

				const unsigned char NO_SENSOR_ATACHT [] PROGMEM = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x80, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x80, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x07, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x07, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x70, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1c, 
0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x18, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x71, 0x1c, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x73, 0x8c, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0xe3, 0x8e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0xe3, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xc3, 0x87, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x83, 0x83, 0x80, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 
0x83, 0x83, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x03, 0x03, 0x81, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x03, 0x81, 0xc0, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x03, 0x80, 0xe0, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x03, 
0x80, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x1c, 0x03, 0x80, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x1c, 0x01, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x38, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 
0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x70, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x60, 0x01, 0x80, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x03, 0x80, 0x0e, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xc0, 0x01, 0x00, 0x07, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xc0, 
0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x03, 0xe0, 0x00, 0x00, 0x07, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 0xff, 0xc0, 
0x00, 0x00, 0x00, 0x00, 0x00};
				const unsigned char CALIBRAR [] PROGMEM = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x07, 0xfb, 0xfe, 0x0f, 0xdf, 0xff,
0xfe, 0xff, 0xff, 0xcf, 0xff, 0xfc, 0x00, 0x3f, 0x80, 0x00, 0x07, 0xff, 0xfe, 
0x1f, 0xdf, 0xff, 0xfe, 0xff, 0xff, 0xcf, 0xff, 0xfc, 0x00, 0x1f, 0x80, 0x00, 
0x07, 0xfb, 0xfe, 0x1f, 0xdf, 0xff, 0xfe, 0xff, 0xff, 0xcf, 0xff, 0xfc, 0x00, 
0x1f, 0x80, 0x00, 0x07, 0xfb, 0xfe, 0x1f, 0xdf, 0xff, 0xfc, 0xff, 0xff, 0xef, 
0xff, 0xfc, 0x00, 0x1f, 0x80, 0x00, 0x07, 0xf9, 0xfc, 0x1f, 0x9f, 0xff, 0xfc, 
0xff, 0xff, 0xef, 0xff, 0xfc, 0x00, 0x1f, 0xc0, 0x00, 0x07, 0xf9, 0xfc, 0x3f, 
0x9f, 0xff, 0xfc, 0xff, 0xff, 0xef, 0xff, 0xf8, 0x00, 0x1f, 0xc0, 0x00, 0x07, 
0xf1, 0xfc, 0x3f, 0x8f, 0xe1, 0xfc, 0x03, 0xf0, 0x0f, 0xe0, 0x00, 0x00, 0x1f, 
0xc0, 0x00, 0x07, 0xf3, 0xfc, 0x3f, 0x9f, 0xc1, 0xf8, 0x03, 0xf8, 0x0f, 0xe0, 
0x00, 0x00, 0x2f, 0xc0, 0x00, 0x07, 0xf3, 0xf8, 0x3f, 0x9f, 0xc1, 0xc0, 0x07, 
0xf8, 0x0f, 0xe0, 0x00, 0x00, 0x3f, 0xc0, 0x00, 0x07, 0xf3, 0xf8, 0x3f, 0x1f, 
0xc0, 0x00, 0x07, 0xf8, 0x1f, 0xe0, 0x00, 0x00, 0x7f, 0xc0, 0x00, 0x0f, 0xf3, 
0xf8, 0x7f, 0x3f, 0x80, 0x00, 0x07, 0xf0, 0x1f, 0xe0, 0x00, 0x00, 0x7f, 0xe0, 
0x00, 0x0f, 0xe3, 0xf8, 0x7f, 0x3f, 0x80, 0x00, 0x07, 0xf0, 0x1f, 0xc0, 0x00, 
0x00, 0xff, 0xe0, 0x00, 0x0f, 0xe7, 0xf8, 0x7f, 0x3f, 0xff, 0xf8, 0x07, 0xf0, 
0x1f, 0xff, 0x80, 0x00, 0xff, 0xe0, 0x00, 0x0f, 0xe7, 0xf0, 0x7f, 0x3f, 0xff, 
0xf0, 0x0f, 0xf0, 0x1f, 0xff, 0xc0, 0x01, 0xff, 0xe0, 0x00, 0x0f, 0xe7, 0xf0, 
0x7e, 0x3f, 0xff, 0xf0, 0x0f, 0xf0, 0x3f, 0xff, 0x80, 0x01, 0xff, 0xe0, 0x00, 
0x1f, 0xe7, 0xf0, 0xfe, 0x7f, 0xff, 0xf0, 0x0f, 0xe0, 0x3f, 0xff, 0x80, 0x03, 
0xff, 0xe0, 0x00, 0x1f, 0xcf, 0xf0, 0xfe, 0x7f, 0xff, 0xf0, 0x0f, 0xe0, 0x3f, 
0xff, 0x80, 0x03, 0xff, 0xe0, 0xf8, 0x1f, 0xcf, 0xf0, 0xfe, 0x00, 0x07, 0xf0, 
0x0f, 0xe0, 0x3f, 0xff, 0x80, 0x07, 0xf7, 0xf1, 0xfc, 0x1f, 0xcf, 0xe0, 0xfe, 
0x00, 0x0f, 0xe0, 0x1f, 0xe0, 0x3f, 0x80, 0x00, 0x07, 0xf7, 0xf1, 0xfc, 0x3f, 
0xcf, 0xe0, 0xfc, 0x00, 0x0f, 0xe0, 0x1f, 0xc0, 0x7f, 0x80, 0x00, 0x0f, 0xe7, 
0xf1, 0xfc, 0x3f, 0xcf, 0xe1, 0xfc, 0x0c, 0x0f, 0xe0, 0x1f, 0xc0, 0x7f, 0x80, 
0x00, 0x0f, 0xe3, 0xf1, 0xfc, 0x3f, 0x9f, 0xe1, 0xfc, 0xfc, 0x1f, 0xe0, 0x1f, 
0xc0, 0x7f, 0x00, 0x00, 0x1f, 0x83, 0xf1, 0xf8, 0x3f, 0x9f, 0xc1, 0xfc, 0xfe, 
0x1f, 0xe0, 0x1f, 0xc0, 0x7e, 0x00, 0x00, 0x1f, 0xff, 0xf3, 0xff, 0xff, 0x9f, 
0xff, 0xfc, 0xff, 0xff, 0xc0, 0x3f, 0xc0, 0x7f, 0xff, 0xe0, 0x3f, 0xff, 0xfb, 
0xff, 0xff, 0x1f, 0xff, 0xf8, 0xff, 0xff, 0xc0, 0x3f, 0x80, 0x7f, 0xff, 0xe0, 
0x3f, 0xff, 0xfb, 0xff, 0xff, 0x1f, 0xff, 0xf9, 0xff, 0xff, 0x80, 0x3f, 0x80, 
0x7f, 0xff, 0xe0, 0x3f, 0xff, 0xfb, 0xff, 0xfe, 0x3f, 0xff, 0xf1, 0xff, 0xff, 
0x80, 0x3f, 0x80, 0x7f, 0xff, 0xe0, 0x7f, 0xff, 0xfb, 0xff, 0xfe, 0x3f, 0xff, 
0xf1, 0xff, 0xff, 0x00, 0x3f, 0x80, 0x7f, 0xff, 0xc0, 0x7f, 0xff, 0xfb, 0xff, 
0xfc, 0x3f, 0xff, 0xe1, 0xff, 0xff, 0x00, 0x3f, 0x00, 0x3f, 0xff, 0xc0, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00};
				const unsigned char CLOCK [] PROGMEM = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 
0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x0e, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x04, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x41, 0x82, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x41, 0x82, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0xc1, 0x83, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x81, 0x81, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x81, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0xc0, 0x43, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x40, 0x23, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x06, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 
0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x0e, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00};
				const unsigned char DONE [] PROGMEM = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0x87, 0xff, 0xff, 0x87, 0xff, 0x03, 0xfb, 
0xff, 0xff, 0xf3, 0xff, 0x8f, 0xfc, 0x07, 0xff, 0xff, 0x87, 0xff, 0xff, 0x87, 
0xff, 0x03, 0xfb, 0xff, 0xff, 0xf3, 0xff, 0x0f, 0xfc, 0x07, 0xff, 0xff, 0x87, 
0xff, 0xff, 0x83, 0xff, 0x03, 0xfb, 0xff, 0xff, 0xe3, 0xff, 0x0f, 0xfc, 0x03, 
0xff, 0xff, 0xc3, 0xff, 0xff, 0xc3, 0xff, 0x83, 0xfb, 0xff, 0xff, 0xe1, 0xff, 
0x0f, 0xf8, 0x03, 0xff, 0xff, 0xc3, 0xff, 0xff, 0xc3, 0xff, 0x83, 0xf3, 0xff, 
0xff, 0xe1, 0xfe, 0x0f, 0xf8, 0x03, 0xff, 0xff, 0xc3, 0xff, 0xff, 0xc3, 0xff, 
0x87, 0xf1, 0xff, 0xff, 0xc1, 0xfe, 0x0f, 0xf8, 0x03, 0xfc, 0x1f, 0xc3, 0xfc, 
0x1f, 0xc3, 0xff, 0x87, 0xf1, 0xfe, 0x00, 0x01, 0xfe, 0x07, 0xf0, 0x07, 0xfc, 
0x1f, 0xc3, 0xfc, 0x1f, 0xc3, 0xff, 0x87, 0xf3, 0xfe, 0x00, 0x01, 0xfe, 0x07, 
0xf0, 0x07, 0xfc, 0x1f, 0xc7, 0xfc, 0x1f, 0xe7, 0xff, 0xc7, 0xe3, 0xfc, 0x00, 
0x01, 0xfc, 0x0f, 0xf0, 0x07, 0xfc, 0x1f, 0xe7, 0xfc, 0x1f, 0xe7, 0xff, 0xcf, 
0xe3, 0xfc, 0x00, 0x03, 0xfc, 0x0f, 0xf0, 0x07, 0xf8, 0x3f, 0xc7, 0xf8, 0x3f, 
0xe7, 0xff, 0xcf, 0xe7, 0xfc, 0x00, 0x03, 0xfc, 0x0f, 0xe0, 0x0f, 0xf8, 0x3f, 
0xc7, 0xf8, 0x3f, 0xc7, 0xff, 0xcf, 0xe7, 0xfc, 0x00, 0x03, 0xf8, 0x0f, 0xe0, 
0x0f, 0xf8, 0x3f, 0xcf, 0xf8, 0x3f, 0xcf, 0xff, 0xcf, 0xc7, 0xfc, 0x00, 0x03, 
0xf8, 0x1f, 0xe0, 0x0f, 0xf8, 0x3f, 0xcf, 0xf8, 0x3f, 0xcf, 0xff, 0xdf, 0xc7, 
0xff, 0xfc, 0x03, 0xf8, 0x1f, 0xc0, 0x0f, 0xf0, 0x7f, 0xcf, 0xf0, 0x3f, 0xcf, 
0xff, 0xff, 0xcf, 0xff, 0xfc, 0x07, 0xf8, 0x1f, 0xc0, 0x0f, 0xf0, 0x7f, 0x8f, 
0xf0, 0x7f, 0x8f, 0xff, 0xff, 0xcf, 0xff, 0xfc, 0x07, 0xf0, 0x1f, 0xc0, 0x1f, 
0xf0, 0x7f, 0x9f, 0xf0, 0x7f, 0x8f, 0xff, 0xff, 0x8f, 0xff, 0xfc, 0x07, 0xf0, 
0x1f, 0x80, 0x1f, 0xf0, 0x7f, 0x9f, 0xf0, 0x7f, 0x9f, 0xef, 0xff, 0x8f, 0xff, 
0xf8, 0x07, 0xf0, 0x3f, 0x80, 0x1f, 0xe0, 0xff, 0x9f, 0xf0, 0x7f, 0x9f, 0xef, 
0xff, 0x9f, 0xf0, 0x00, 0x07, 0xe0, 0x3f, 0x80, 0x1f, 0xe0, 0xff, 0x1f, 0xe0, 
0xff, 0x1f, 0xef, 0xff, 0x9f, 0xf0, 0x00, 0x07, 0xe0, 0x3f, 0x80, 0x3f, 0xe0, 
0xff, 0x3f, 0xe0, 0xff, 0x1f, 0xef, 0xff, 0x1f, 0xf0, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x3f, 0xe0, 0xff, 0x3f, 0xe0, 0xff, 0x3f, 0xef, 0xff, 0x1f, 0xe0, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x3f, 0xc0, 0xff, 0x3f, 0xc0, 0xff, 0x3f, 0xef, 0xff, 
0x1f, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x81, 0xff, 0x3f, 0x81, 0xff, 
0x3f, 0xc7, 0xff, 0x3f, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xfe, 
0x3f, 0xff, 0xfe, 0x3f, 0xc7, 0xff, 0x3f, 0xff, 0xff, 0x3f, 0xe0, 0xff, 0x80, 
0x7f, 0xff, 0xfe, 0x3f, 0xff, 0xfe, 0x7f, 0xc7, 0xfe, 0x1f, 0xff, 0xff, 0x3f, 
0xe0, 0xff, 0x00, 0x7f, 0xff, 0xfe, 0x3f, 0xff, 0xfe, 0x7f, 0xc7, 0xfe, 0x1f,
0xff, 0xfe, 0x3f, 0xe1, 0xff, 0x00, 0x7f, 0xff, 0xfe, 0x3f, 0xff, 0xfe, 0x7f, 
0xc7, 0xfe, 0x1f, 0xff, 0xfe, 0x3f, 0xc1, 0xff, 0x00, 0x7f, 0xff, 0xfc, 0x1f, 
0xff, 0xfc, 0x7f, 0x83, 0xfe, 0x1f, 0xff, 0xfe, 0x7f, 0xc1, 0xff, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00};
				const unsigned char EXIT [] PROGMEM = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 0xe3, 0xfe, 0x0f, 0xf1, 0xff, 0xf3, 
0xff, 0xff, 0xff, 0xc3, 0xff, 0xf0, 0x03, 0xff, 0xff, 0xff, 0xe7, 0xff, 0x3f, 
0xfb, 0xff, 0xf3, 0xff, 0xff, 0xff, 0xe3, 0xff, 0xf0, 0x03, 0xff, 0xff, 0xff, 
0xe3, 0xff, 0x3f, 0xf1, 0xff, 0xf3, 0xff, 0xff, 0xff, 0xe3, 0xff, 0xf0, 0x01, 
0xff, 0xff, 0xff, 0xe3, 0xff, 0x7f, 0xe1, 0xff, 0xf3, 0xff, 0xff, 0xff, 0xe1, 
0xff, 0xe0, 0x01, 0xff, 0xff, 0xff, 0xc3, 0xff, 0xff, 0xc1, 0xff, 0xe1, 0xff, 
0xff, 0xff, 0xf1, 0xff, 0xe0, 0x01, 0xff, 0xff, 0xff, 0xc3, 0xff, 0xff, 0xc1, 
0xff, 0xe1, 0xff, 0xff, 0xff, 0xf1, 0xff, 0xc0, 0x01, 0xff, 0xc0, 0x00, 0x01, 
0xff, 0xff, 0x80, 0xff, 0xe0, 0x01, 0xff, 0x80, 0x01, 0xff, 0xc0, 0x03, 0xff, 
0xc0, 0x00, 0x01, 0xff, 0xff, 0x01, 0xff, 0xc0, 0x01, 0xff, 0xe0, 0x00, 0xff, 
0xc0, 0x03, 0xff, 0xc0, 0x00, 0x01, 0xff, 0xfe, 0x03, 0xff, 0xc0, 0x01, 0xff, 
0xc0, 0x01, 0xff, 0x80, 0x03, 0xff, 0x80, 0x00, 0x01, 0xff, 0xfe, 0x03, 0xff, 
0xc0, 0x03, 0xff, 0xc0, 0x01, 0xff, 0x80, 0x07, 0xff, 0x80, 0x00, 0x00, 0xff, 
0xfc, 0x03, 0xff, 0x80, 0x03, 0xff, 0xc0, 0x03, 0xff, 0x00, 0x07, 0xff, 0x80, 
0x00, 0x00, 0xff, 0xf8, 0x07, 0xff, 0x80, 0x03, 0xff, 0x80, 0x03, 0xff, 0x00, 
0x07, 0xff, 0xff, 0xe0, 0x00, 0xff, 0xf0, 0x07, 0xff, 0x80, 0x03, 0xff, 0x80, 
0x03, 0xfe, 0x00, 0x0f, 0xff, 0xff, 0xf0, 0x00, 0x7f, 0xe0, 0x07, 0xff, 0x00, 
0x07, 0xff, 0x80, 0x07, 0xfe, 0x00, 0x0f, 0xff, 0xff, 0xf0, 0x00, 0xff, 0xe0, 
0x07, 0xff, 0x00, 0x07, 0xff, 0x00, 0x07, 0xfe, 0x00, 0x0f, 0xff, 0xff, 0xe0, 
0x00, 0xff, 0xc0, 0x0f, 0xff, 0x00, 0x07, 0xff, 0x00, 0x07, 0xfc, 0x00, 0x0f, 
0xff, 0xff, 0xe0, 0x01, 0xff, 0xe0, 0x0f, 0xfe, 0x00, 0x0f, 0xff, 0x00, 0x07, 
0xfc, 0x00, 0x1f, 0xff, 0xff, 0xe0, 0x03, 0xff, 0xe0, 0x0f, 0xfe, 0x00, 0x0f, 
0xfe, 0x00, 0x0f, 0xf8, 0x00, 0x1f, 0xfc, 0x00, 0x00, 0x07, 0xff, 0xf0, 0x1f, 
0xfe, 0x00, 0x0f, 0xfe, 0x00, 0x0f, 0xf8, 0x00, 0x1f, 0xfc, 0x00, 0x00, 0x07, 
0xff, 0xf0, 0x1f, 0xfc, 0x00, 0x1f, 0xfe, 0x00, 0x0f, 0xf8, 0x00, 0x3f, 0xfc, 
0x00, 0x00, 0x0f, 0xff, 0xf0, 0x1f, 0xfc, 0x00, 0x1f, 0xfc, 0x00, 0x00, 0x00, 
0x00, 0x3f, 0xf8, 0x00, 0x00, 0x1f, 0xff, 0xf0, 0x3f, 0xfc, 0x00, 0x1f, 0xfc, 
0x00, 0x00, 0x00, 0x00, 0x3f, 0xf0, 0x00, 0x00, 0x3f, 0xff, 0xf8, 0x3f, 0xf8, 
0x00, 0x3f, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xfc, 0x3f, 0xff, 
0xf8, 0x3f, 0xf8, 0x00, 0x3f, 0xf8, 0x00, 0x3f, 0xf8, 0x00, 0x3f, 0xff, 0xff, 
0xfc, 0x7f, 0xff, 0xf8, 0x7f, 0xf8, 0x00, 0x3f, 0xf8, 0x00, 0x7f, 0xf8, 0x00, 
0x3f, 0xff, 0xff, 0xfc, 0xff, 0xdf, 0xf8, 0x7f, 0xf0, 0x00, 0x7f, 0xf8, 0x00, 
0x7f, 0xf0, 0x00, 0x3f, 0xff, 0xff, 0xf9, 0xff, 0xcf, 0xfc, 0x7f, 0xf0, 0x00, 
0x7f, 0xf8, 0x00, 0xff, 0xf0, 0x00, 0x3f, 0xff, 0xff, 0xf9, 0xff, 0x8f, 0xfc, 
0xff, 0xf0, 0x00, 0x7f, 0xf0, 0x00, 0xff, 0xf0, 0x00, 0x1f, 0xff, 0xff, 0xf9, 
0xff, 0x0f, 0xfc, 0x7f, 0xe0, 0x00, 0x7f, 0xf0, 0x00, 0xff, 0xe0, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00};
//Rutina que se encarga de mostrar los bitmaps que inican si el sensor no esta conectado a la placa
void No_Sensor_ (){
	Serial.println("No se reconoce un sensor conectado......");	
		display.stopscroll();
		display.clearDisplay();
		display.setTextSize(2);
		display.setTextColor(WHITE);
		display.setCursor(0, 0);
		display.drawBitmap(0,0,NO_SENSOR_LOGO,128,32,1);
		display.display();
			delay(1000);
		display.clearDisplay();
		display.setTextSize(2);
		display.setTextColor(WHITE);
		display.setCursor(0, 0);
		display.drawBitmap(0,0,ALERT_LOGO,128,32,1);
		display.display(); 
			delay(1000);
		display.startscrollleft(0x00, 0x0F);
		display.clearDisplay();
		display.setTextSize(2);
		display.setTextColor(WHITE);
		display.setCursor(0, 0);
		display.drawBitmap(0,0,NO_SENSOR_ATACHT,128,32,1);
		display.display(); 
			delay(10000);
	Sensor_ausente = true;
 		
		 if(Sensor_ausente == true)esp_restart();
}
//Rutina que estima de forma definitiva la proobabilidad de contagio
void Count_probability_(){
	if (airSensor.dataAvailable()){
		CO2 = airSensor.getCO2();
				Total_order_loss_rate = ventilation_rate + 0.62 + 0.3;
				//⬆⬆ calcula que tan rapido el aire cambia en la habitacion, es uno de los parametros mas importantes!
				Net_emission_Rate = Quanta_Exalation_rate_of_posible_infected *(1-(Mask_eficiency*People_whit_mask))*1;
				//⬆⬆calcula la emision de aerosoles  tomando en cuenta cuantas personas hay en la habitacion y la eficiencia de la mascara entre otros
				average_quanta_concentration = (Net_emission_Rate/Total_order_loss_rate/volume)*(1-(1/Total_order_loss_rate/Event_Duration)*(1-(pow(euler_number,-Total_order_loss_rate*Event_Duration))));
				//⬆⬆ alcula el promedio de las emisiones de las personas en la habiatacion
				quanta_inhaled_per_person = average_quanta_concentration * Breathing_rate * Event_Duration * (1 - (Inhalation_mask_eficiency * People_whit_mask));
				/*⬆⬆ Calcula cuantos aerosoles cada persona ha respirado en el intervalo y tomando en cuenta la eficiiencia de la mascara, la respiracion
				de las personas, la duraccion del evento etc..*/
				Probabilidad = (1-(pow(euler_number,(-quanta_inhaled_per_person))));
				/*⬆⬆A partir de resultado anterior determina la probabilidad*/
					display.clearDisplay();
					display.setTextSize(3);
					display.setTextColor(WHITE);
					display.setCursor(0, 0);
						display.print(Probabilidad);
						display.print("%");
					display.setCursor(0, 24);
					display.setTextSize(1);
					display.setTextColor(WHITE);
						display.print(CO2);
						display.print(" PPM");
					display.display();
	delay(2000);
}
}
//Rutina que se encarga de calibrar el sensor que usa el co2 para estimar la velocidad que la habitacion cambia de aire (ACH)
void Calibration (){
	display.clearDisplay();
	display.setTextSize(2);
	display.setTextColor(WHITE);
	display.setCursor(0, 8);
	display.drawBitmap(0,0,CALIBRAR,128,32,1);
	display.display(); 
		delay(2000);
			while(a == false){
				display.clearDisplay();
				display.setTextSize(2);
				display.setTextColor(WHITE);
				display.setCursor(0, 0);
				display.printf("COMENZAR?");
				display.display(); 
			if (digitalRead(21) == HIGH){
				display.clearDisplay();
				display.setTextSize(4);
				display.setTextColor(WHITE);
				display.setCursor(0, 0);
				display.drawBitmap(0,0,CLOCK,128,32,1);
				display.display();
			for(int Q = 0;Q < 1;){
	            display.clearDisplay();
	            display.setTextSize(2);
	            display.setTextColor(WHITE);
	            display.setCursor(0, 0);
	            display.drawBitmap(0,0,CLOCK,128,32,1);
	            display.setCursor(32, 8);
				display.print(Q);
				display.setCursor(40, 8);
				display.print(";");
				display.setCursor(48, 8);
				display.print(w);
				display.setCursor(57, 8);
				display.print(e);
				display.display();
			for (; w < 6;){
				display.clearDisplay();
				display.setTextSize(2);
				display.setTextColor(WHITE);
				display.setCursor(0, 0);
				display.drawBitmap(0,0,CLOCK,128,32,1);
				display.setCursor(32, 8);
				display.print(Q);
				display.setCursor(40, 8);
				display.print(";");
				display.setCursor(48, 8);
				display.print(w);
				display.setCursor(57, 8);
				display.print(e);
				display.display();
			for (; e < 9;){
				display.clearDisplay();
				display.setTextSize(2);
				display.setTextColor(WHITE);
				display.setCursor(0, 0);
				display.drawBitmap(0,0,CLOCK,128,32,1);
				display.setCursor(32, 8);
				display.print(Q);
				display.setCursor(40, 8);
				display.print(";");
				display.setCursor(48, 8);
				display.print(w);
				display.setCursor(57, 8);
				display.print(e);
				display.display();
					e++;
					CO2 = airSensor.getCO2();
					if(CO2 > co2_peack)co2_peack = CO2;
			 		delay(1000);}
					e = 0;
					w++;}
					w = 0;
					Q++;}
					
	display.clearDisplay();
	display.setTextSize(2);
	display.setTextColor(WHITE);
	display.setCursor(0, 8);
	display.drawBitmap(0,0,EXIT,128,32,1);
	display.display(); 
	delay(10000);
	decay63time = 610;
	co2_diference = co2_peack - co2_outdoor;
	Decay63_percentage = co2_outdoor + (euler_number_negative * co2_diference);
	display.clearDisplay();
	display.setTextSize(2);
	display.setTextColor(WHITE);
	display.setCursor(0, 8);
	display.println(Decay63_percentage);
	display.display(); 
	delay(10000);
	while(CO2 > Decay63_percentage){
				decay63time++;
				CO2 = airSensor.getCO2();
				display.clearDisplay();
	display.setTextSize(1);
	display.setTextColor(WHITE);
	display.setCursor(0, 0);
	display.println(CO2);
	display.setCursor(0, 8);
	display.println(Decay63_percentage);
	display.setCursor(0, 16);
	display.println(decay63time);
	display.display(); 
				delay(1000);
	}
delay(1000);
Serial.println(decay63time);
	decay63time = decay63time/60;
	delay(1000);
	Serial.println(decay63time);
	decay63time = decay63time/60;
	ventilation_rate =1 / decay63time;
	display.clearDisplay();
	display.setTextSize(2);
	display.setTextColor(WHITE);
	display.setCursor(0, 8);
	display.drawBitmap(0,0,DONE,128,32,1);
	display.display(); 
	delay(2000);
	display.clearDisplay();
	display.setTextSize(4);
	display.setTextColor(WHITE);
	display.setCursor(0, 8);
	display.println(ventilation_rate);
	display.display(); 
	delay(2000);
	a = true;
	}
				}
					
					}
//SETUP
void setup() {
	//inicia el pin 21 como entrada
	pinMode(calibration_button,INPUT);
	Wire.begin(23,22);//Configura el puerto I2C donde pin 23 es SDA Y PIN 22 SCL
	Serial.begin(115200);//⚠ VELOCIDAD DE PUERTO SERIAL ⚠
	delay(100);
		if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
			Serial.println("SSD1306 allocation failed");
				for(;;);}
					display.clearDisplay();
					display.setTextSize(2);
					display.setTextColor(WHITE);
					display.setCursor(0, 0);
					display.drawBitmap(0,0,BOREAS,128,32,1);
					display.display(); 
delay(1000);
		if(airSensor.begin() == false)No_Sensor_();
}
void loop() {
	//Si el boton esta presionado la calibracion se activa si no se procede a iniciar las mediciones!!!
	if (digitalRead(calibration_button) == 1)
	{
		Calibration();
	}else{
		Count_probability_();
	}
}