/* I-GAUGE Unit ToT 2021
    Kegiatan IUWASH - SECO
    BEKERJA SAMA DENGAN PDAM DEPOK
    2560 WiFi

    - LED NYALA - RTC & SD CARD ERROR
    - LED BLINKING TIAP 1 DETIK > WAKTU TUNGGU

    - LED NYALA > DATA TIDAK TERKIRIM KE SERVER
    - LED BLINKING 2 DETIK SEKALI > DATA TERKIRIM KE SERVER
*/

//LIBRARY
#include <avr/power.h>
#include <avr/sleep.h>
#include <DS3232RTC.h>
#include <LowPower.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>

//custom library
#include "config.h"

#define gsm Serial1
//parameter waktu
time_t Waktu, alarmTime;
byte pembagiWaktu;
time_t ALARM_INTERVAL;  // alarm interval in seconds

void setup() {
  //initialisation
  Serial.begin(baud);
  gsm.begin(baud);  // SIM900A
  pinMode(13, OUTPUT);

  //WELCOME SCREEN
  Serial.println(F("\r\nGSM IGAUGE unit Training 5V 2021"));
  //  Serial.print(F("Unit = "));
  //  Serial.println(ID);
  Serial.flush();
  inisialisasi();

  //CEK RTC
  cekRTC();

  //cek micro SD Card
  sdInit();
  configs();
  pembagiWaktu = 60 / interval;
  ALARM_INTERVAL = (interval * 60);
  cekfile();

  //waiting 15 second for GSM warm up
  for (indeksA = 1; indeksA <= indeksB; indeksA++) {
    Serial.print(indeksA);
    Serial.println(F(" detik"));
    Serial.flush();
    delay(1000);
  }

  Serial.println(F("Cek AT Command"));
  ceksim(); // Cek GSM modul apakah terhubung
  Serial.println(F("Registrasi SIM"));
  regsim(); // registrasi GSM modul ke network

  //atur waktu interupsi
  Wire.setClock(400000) ;
  waktu(); // ambil data waktu dari RTC network

  kerja(0);
  Serial.print(F("waktu = "));
  cekwaktu();
}

void loop() {
  kerja(1);
  tidur();
}

void kerja(boolean momen) {
  Serial.println("Saatnya bekerja");

  // turn on GSM & Pressure sensor
  ambilWaktu();
  Serial.println(F("Mulai Ambil Data"));

  if (momen == 1) {
    tmElements_t tm;
    tm.Year = CalendarYrToTm(tahun);
    tm.Month = bulan;
    tm.Day = hari;
    tm.Hour = jam;
    tm.Minute = menit;
    tm.Second = detik;
    time_t t = makeTime(tm);
    RTC.set(t);
    Waktu = RTC.get();
  }
  printDateTime(Waktu);


  gprsComm();

  bersihdata();

  //AMBIL DATA SENSOR TEKANAN
  Serial.println(F("ambil data sensor tekanan"));
  Serial.flush();

  //burst data 5 seconds
  for (i = 0; i < burst; i++) {
    reads1 = analogRead(pres); //pressure
    reads = reads + reads1;
    delay(1000);
  }


  //KONVERSI TEKANAN
  volt = ((float)reads / (float)burst) / 1024.00 * 5.0; // nilai voltase dari nilai DN

  Serial.print(F("TEGANGAN SENSOR = "));
  Serial.println(volt);
  tekanan = (3.00 * volt - 1.5) + float(offset);
  //formula hasil regresi
  //  tekanan = 1.1739*tekanan - 0.1;

  Serial.print(F("TEKANAN SENSOR = "));
  Serial.println(tekanan);



  if (tekanan < 0) tekanan = 0;

  //debug value
  Serial.print(F("TEKANAN = "));
  Serial.println(tekanan, 2);
  Serial.flush();

  if (GSMerror == 0) {
    sendServer();
    //HTTP TERMINATE
    Serial.println(F("\r\n- SELESAI KIRIM -"));
    TCPclose(200);
    gprsShut(300);
  }
  else kode = 999;

  simpanData();
}

void bersihdata() {
  reads = 0; reads1 = 0; tekanan = 0; volt = 0;
}
