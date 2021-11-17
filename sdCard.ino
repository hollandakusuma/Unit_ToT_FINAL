void sdInit() {
  pinMode(SSpin, OUTPUT);
  digitalWrite(SSpin, HIGH);
  delay(10);

  if (!SD.begin(SSpin)) { //SD ERROR
    Serial.println(F("> MICRO SD CARD TIDAK TERDETEKSI!!!!"));
    digitalWrite(13, 1);
    while (1);
  }
  Serial.println(F("> MICRO SD CARD TERDETEKSI!"));
  Serial.flush();
}

void configs() {
  i = 0;
  char sdcard[300];
  File file = SD.open(F("config.txt"));
  if (file) {
    while (file.available()) {
      sdcard[i++] = file.read();
    }
  }
  else  { //error reading
    Serial.println(F("CONFIG.TXT Tidak Ditemukan!!"));
    digitalWrite(13, 1);
    while (1);
  }
  file.close();

  String filename = String(sdcard);
  for ( a = 0; a < sizeof(sdcard); a++) { //clear variable
    sdcard[a] = (char)0;
  }

//  Serial.println(filename);
  Serial.println(F("GET CONFIG SUCCESS!!!"));
  Serial.flush();
  b = 0;

  //ID
  a = filename.indexOf("=", b + 1);
  b = filename.indexOf("\r", a + 1);
  ID = filename.substring(a + 2, b);

  //interval data
  a = filename.indexOf("=", b + 1);
  b = filename.indexOf("\r", a + 1);
  interval = filename.substring(a + 1, b).toInt();

  //offset
  a = filename.indexOf("=", b + 1);
  b = filename.indexOf("\r", a + 1);
  offset = filename.substring(a + 1, b).toFloat();

  //LATITUDE
  a = filename.indexOf("=", b + 1);
  b = filename.indexOf("\r", a + 1);
  String y = filename.substring(a + 2, b);
  Lat = y.toFloat();


  //LONGITUDE
  a = filename.indexOf("=", b + 1);
  b = filename.indexOf("\r", a + 1);
  y = filename.substring(a + 2, b);
  Long = y.toFloat();


  // APN
  a = filename.indexOf("=", b + 1);
  b = filename.indexOf("\r", a + 1);
  apn = filename.substring(a + 2, b);

  //USERNAME
  a = filename.indexOf("=", b + 1);
  b = filename.indexOf("\r", a + 1);
  username = filename.substring(a + 2, b);

  //pwd
  a = filename.indexOf("=", b + 1);
  b = filename.indexOf("\r", a + 1);
  password = filename.substring(a + 2, b);

  //email
  a = filename.indexOf("=", b + 1);
  b = filename.indexOf("\r", a + 1);
  email = filename.substring(a + 2, b);

  //email Password
  a = filename.indexOf("=", b + 1);
  b = filename.indexOf("\r", a + 1);
  emailPass = filename.substring(a + 2, b);

  filename = '0';

  Serial.print(F("Station ID = "));
  Serial.println(ID);
  Serial.print(F("Interval data = "));
  Serial.print(interval);
  Serial.println(F(" menit"));
  Serial.print(F("offset = "));
  Serial.println(offset);
  Serial.print(F("Lintang = "));
  Serial.println(Lat, 6);
  Serial.print(F("Bujur = "));
  Serial.println(Long, 6);
  Serial.print(F("APN = "));
  Serial.println(apn);
  Serial.print(F("USERNAME = "));
  Serial.println(username);
  Serial.print(F("PASSWORD = "));
  Serial.println(password);
  if (gsmTidur == 1) {
    Serial.print(F("EMAIL = "));
    Serial.println(email);
    Serial.print(F("EMAIL PASSWORD = "));
    Serial.println(emailPass);
  }
  Serial.println(F("\r\n\r\n"));
  Serial.flush();

}

void cekfile() {
  //LOCAL VARIABLE
  byte indeks = 1;
  nomor = 0;

  while (indeks) {
    nomor = nomor + 1;
    sprintf(NamaFile, "LOG%03d.txt", nomor);
    indeks = SD.exists(NamaFile);
    if (indeks == 0) {
      Serial.print(NamaFile);
      Serial.println(F(" tidak ada"));
      File file = SD.open(NamaFile, FILE_WRITE);
      file.println(F("Tanggal(YYYY/MM/DD) , Waktu(HH:MM:SS) , Tekanan (Bar) , Kode HTTP"));
      file.flush();
      file.close();
    }
  }
  Serial.flush();
}

void simpanData() {
  if (gsmTidur == 1) {
    Serial.print(F("Nomor File "));
    Serial.println(nomor);
    Serial.print(F("Nama File = "));
    Serial.println(NamaFile);
  }

  SdFile::dateTimeCallback(dateTime);
  File file = SD.open(NamaFile, FILE_WRITE);
  file.print(tahun);
  file.print("/");
  file.print(bulan);
  file.print("/");
  file.print(hari);
  file.print(" ");
  file.print(jam);
  file.print(":");
  file.print(menit);
  file.print(":");
  file.print(detik);
  file.print(",");
  file.print(tekanan);
  file.print(",");
  file.println(kode);
  file.close();
  Serial.println("- SIMPAN DATA SELESAI -");
}

void dateTime(uint16_t* date, uint16_t* time) {
  // return date using FAT_DATE macro to format fields
  *date = FAT_DATE(tahun, bulan, hari);

  // return time using FAT_TIME macro to format fields
  *time = FAT_TIME(jam, menit, detik);
}
