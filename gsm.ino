void ceksim() { ///CONNECT AT
  for (indeksA = 0; indeksA < 20; indeksA++) {
    indeksB = ConnectAT(F("AT"), 200);
    if (indeksB == 20) {
      Serial.println(F("GSM MODULE OK!!"));
      Serial.flush();
      GSMerror = 0;
      break;
    }
    if (indeksB < 20) {
      Serial.print(indeksA);
      Serial.println(F(" GSM MODULE ERROR"));
      Serial.flush();
      GSMerror = 1;
    }
    delay(500);
  }
}

void regsim() {
  indeksA = 0;
  result = "";
  //Functionality
  Serial.println(F("Set Functionality"));
  gsm.println(F("AT+CFUN=1"));
  readSerial(200);
  delay(1500);

  //Functionality
  Serial.println(F("Set Network"));
  gsm.println(F("AT+CNMP=51;+CMNB=3;+CLTS=1;+CBAND=\"ALL_MODE\"")); // AT+CNMP=51;+CMNB=3;+CLTS=1;+CBAND="ALL_MODE"
  readSerial(1500);

  gsm.print(F("AT+CGDCONT=1,\"IP\",\""));
  gsm.print(apn);
  gsm.println("\"");
  readSerial(500);
  gsm.print(F("AT+CGDCONT=13,\"IP\",\""));
  gsm.print(apn);
  gsm.println("\"");
  readSerial(500);

  gsm.println(F("AT+CGDCONT?;+CGREG?;+CPSI?"));
  readSerial(1000);

  //opsi jika tidak terhubung ke network
  indeksA = 0;
  while (indeksA <= 10) {
    sim();
    indeksA++;
    if (json.length() > 0)  {
      sinyal();
      GSMerror = 0;
      break;
    }
    if (indeksA == 10) {
      //REGISTASI NETWORK
      Serial.println(F("\r\n- TUTUP TCP IP -"));
      gprsShut(200);

      Serial.println(F("TIDAK ADA OPERATOR"));
      Serial.flush();
      GSMerror = 1;
    }
  }
}

void sim() {
  result = "";
  json = "";
  //REGISTASI NETWORK
  Serial.println(F("Registrasi Network"));
  gsm.println(F("AT+CREG=1"));
  readSerial(500);
  delay(1500);

  if (gsmTidur==1) sinyal();

  //CARI OPERATOR KARTU
  Serial.println(F("Cari operator"));
  gsm.println(F("AT+COPS?"));
  Serial.flush();
  gsm.flush();
  delay(200);
  while (gsm.available() > 0) {
    if (gsm.find("+COPS:")) {
      while (gsm.available() > 0) {
        karakter = gsm.read();
        result += karakter;
      }
    }
  }
  Serial.flush();
  gsm.flush();

  indeksA = result.indexOf('"');
  indeksB = result.indexOf('"', indeksA + 1);
  json = result.substring(indeksA + 1, indeksB);

  //Nama operator
  Serial.println(json);
  Serial.flush();
  if (json.length() == 0) GSMerror = 1;
  if (json.length() > 0) GSMerror = 0;
  delay(1500);

  gsm.println("AT+COPS=1,2,\"51010\""); // AT+COPS=1,2,"51010"
  delay(100);
  cekSerial(10000);

}

void sinyal() {
  kalimat = "";
  Serial.print("Kulitas Sinyal ");
  gsm.println(F("AT+CSQ"));
  Serial1.flush();
  delay(100);
  while (gsm.available() > 0) {
    if (gsm.find("+CSQ: ")) {
      while (gsm.available() > 0) {
        karakter = Serial1.read();
        if (gsmTidur == 1)   Serial.print(karakter);
        kalimat += karakter;
      }
    }
  }
  if (gsmTidur == 1) {
    Serial.println(" ");
    Serial.println(kalimat);
    Serial.flush();
    Serial1.flush();
  }

  a = (kalimat.substring(0, kalimat.indexOf(','))).toInt();
  Serial.print("("); Serial.print(a);
  Serial.print(") ");
  Serial.flush();
  if (a < 10)             Serial.println("JELEK");
  if (a > 9 && a < 15)    Serial.println("CUKUP");
  if (a > 14 && a < 20)   Serial.println("BAGUS");
  if (a > 19 && a <= 31)  Serial.println("SANGAT BAGUS");
  if (a == 99)            Serial.println("Tidak Diketahui");
  delay(100);

}

void waktu() {
  //WAKTU LOKAL BERDASARKAN NETWORK GSM
  Serial.println(F("Minta waktu lokal ke network GSM"));
  gsm.println(F("AT+CLTS=1"));
  readSerial(500);
  delay(500);
  gsm.println(F("AT+CCLK?"));
  kalimat = "";
  start = millis();
  while ((start + 200) > millis()) {
    while (gsm.available() > 0) {
      karakter = gsm.read();
      kalimat = kalimat + karakter;
    }
  }
  gsm.flush();

  if (gsmTidur == 1) {
    Serial.println(kalimat);
    Serial.flush();
  }

  delay(500);
  tmElements_t tm;
  //parse string
  indeksB = 0;

  //tahun
  indeksA = kalimat.indexOf("\"", indeksB + 1);
  indeksB = kalimat.indexOf("/", indeksA + 1);
  tahun = 2000 + kalimat.substring(indeksA + 1, indeksB).toInt();

  //bulan
  indeksA = kalimat.indexOf("/", indeksB + 1);
  bulan = kalimat.substring(indeksB + 1, indeksA).toInt();

  //hari
  indeksB = kalimat.indexOf(",", indeksA + 1);
  hari = kalimat.substring(indeksA + 1, indeksB).toInt();

  //jam
  indeksA = kalimat.indexOf(":", indeksB + 1);
  jam = kalimat.substring(indeksB + 1, indeksA).toInt();

  //menit
  indeksB = kalimat.indexOf(":", indeksA + 1);
  menit = kalimat.substring(indeksA + 1, indeksB).toInt();

  //detik
  indeksA = kalimat.indexOf("+", indeksB + 1);
  detik = kalimat.substring(indeksB + 1, indeksA).toInt();

  if (tahun >= 2019 && tahun < 2080) {
    tm.Year = CalendarYrToTm(tahun);
    tm.Month = bulan;
    tm.Day = hari;
    tm.Hour = jam;
    tm.Minute = menit;
    tm.Second = detik;
    Waktu = makeTime(tm);
    RTC.set(Waktu);
    setTime(Waktu);
  }
  else ambilWaktu();

  printDateTime(Waktu);
  Serial.flush();

}

byte ConnectAT(String cmd, int d) {
  i = 0;

  while (1) {
    gsm.println(cmd);
    while (gsm.available()) {
      if (gsm.find("OK"))
        i = 20;
    }
    delay(d);
    if (i > 5) {
      break;
    }
    i++;
  }
  return i;
}

void readSerial(unsigned int wait) {
  char karakter = -2;
  unsigned long mulai = millis();
  while (millis() - mulai <= wait)  {
    karakter = gsm.read();
    if (gsmTidur == 1) {
      if (isAscii(karakter)) Serial.write(karakter);
    }
  }
}

void GSMreset() { //WAKE UP GSM
  Serial.println(F(" "));
  gsm.println(F("AT+CSCLK=0"));
  Serial.flush();
  gsm.flush();
  delay(200);
  gsm.println(F("AT+CSCLK=0"));
  gsm.flush();
  readSerial(200);
  Serial.flush();
  gsm.flush();
  LowPower.powerDown(SLEEP_2S, ADC_OFF, BOD_OFF);
}
