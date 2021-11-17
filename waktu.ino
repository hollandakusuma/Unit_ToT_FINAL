void cekRTC() {
  if (gsmTidur == 1) {
    Serial.println(F("Cek RTC"));
    Serial.flush();
  }
  Wire.begin();
  Wire.beginTransmission(rtc_addr);
  indeksA = Wire.endTransmission();
  if (indeksA == 0) {
    Serial.println(F("RTC ada"));
  }
  if (indeksA != 0) {
    Serial.println(F("RTC Error"));
    digitalWrite(13, HIGH);
    while (1);
  }
  Wire.end();
  setSyncProvider(RTC.get);
  Serial.flush();
}

void ambilWaktu() {
  Waktu = RTC.get();
  tahun = year(Waktu);
  bulan = month(Waktu);
  hari = day(Waktu);
  jam = hour(Waktu);
  menit = minute(Waktu);
  detik = second(Waktu);
  if (tahun >= 2080 || tahun < 2021) {
    waktu();
    tahun = year(Waktu);
    bulan = month(Waktu);
    hari = day(Waktu);
    jam = hour(Waktu);
    menit = minute(Waktu);
    detik = second(Waktu);
  }

  //set waktu alarm
  tmElements_t tm;
  tm.Year = CalendarYrToTm(tahun);
  tm.Month = bulan;
  tm.Day = hari;
  tm.Hour = jam;
  tm.Minute = menit;
  tm.Second = 0;
  sprintf(sTime,"%d-%02d-%02dT%02d:%02d:%02d.000Z",tahun,bulan,hari,jam,menit,0);
  alarmTime = makeTime(tm) + ALARM_INTERVAL;
}

void cekwaktu() { // tidur hingga waktu pengambilan data yang disuruh
  Waktu = RTC.get();
  printDateTime(Waktu) ;
  menit = minute(Waktu);
  kode = menit;
  Serial.print(F("menit sekarang = "));
  Serial.println(menit);
//  Serial.print("Tidur hingga ");
  for (byte i = 0; i < pembagiWaktu; i++) {
    byte batasBawah=i*interval;
    byte batasAtas= (i + 1) * interval;
    delay(5);
    if (menit >= 60 - interval) {
//      Serial.print(60 - menit);
      menit = 60 - menit;
      break;
    }
    else if (menit >= batasBawah && menit <batasAtas)  {
//      Serial.print(batasAtas);
      menit = batasAtas - menit;
      break;
    }
  }
//  Serial.println(" menit");
  Serial.flush();

  menit = kode + menit;
  if (menit > 60) menit = menit - 60;
  Serial.print("Bangun di menit ");
  Serial.println(menit);

  detik = 1;
  while (1) {
    Waktu = RTC.get();
    kode = minute(Waktu);
    detik = second(Waktu);
    printDateTime(Waktu) ;
    if (detik == 0 && kode == menit) break;
    else if (kode == menit) break;
    digitalWrite(13, 1);
    delay(500);
    digitalWrite(13, 0);
    delay(500);
  }
}

void printDateTime(time_t t) {
  Serial.print(day(t));
  Serial.print("/");
  Serial.print(month(t));
  Serial.print("/");
  Serial.print(year(t));
  Serial.print(" ");
  Serial.print(hour(t));
  Serial.print(":");
  Serial.print(minute(t));
  Serial.print(":");
  Serial.println(second(t));
  Serial.flush();
}

void inisialisasi() {
  awal = millis();
  while (millis() - awal < 5000) {
    while (Serial.available() > 0) {
      karakter = Serial.read();
      if (karakter == A) break;
    }
    if (karakter == A) break;
  }
  if (karakter == A) gsmTidur = 1;

}

void tidur() {
  if (kode != 200) digitalWrite(13, 1);
  a = 0;
  
  printDateTime(alarmTime);
  while (1) {
    Waktu=RTC.get();
    if (gsmTidur == 1) printDateTime(Waktu) ;
    else    {
      Serial.print('-');
      a++;
      if (a > 30) {
        a = 0;
        Serial.println("-");
      }
    }

    if (Waktu >= alarmTime) {
      Serial.println('-');
      break;
    }
    if (kode == 200) {
      b = a % 2;
      digitalWrite(13, b);
    }
    delay(2000);
  }
}
