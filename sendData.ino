boolean sendServer() {
  //HTTP initilization
  Serial.println(F("\r\n- MULAI KIRIM DATA -"));
  boolean nilai = TCPstart(5000, 3);
  if (nilai == 0) {
    for (byte z = 0; z < 10; z++) {
      gsm.println("AT");
      readSerial(200);
    }

    return 0;
  }

  //SET HTTP PARAMETERS VALUE
  Serial.println(F("\r\n- KIRIM DATA -"));

  nilai = TCPsend();
  if (nilai == 0) {
    if (gsmTidur == 1) Serial.println("TCP SEND ERROR");
  }
  while (Serial.available()) Serial.read();

  if (kode != 200) return 0;
  else return 1;
}


// LOW CODE FOR TCP IP
// --------------------------------------------------
// FUNCTION FOR TCP IP
boolean TCPstart(unsigned long jeda, byte ulangan) {
  char g;
  boolean hasilTCP = 0;
  for (byte angka = 1; angka <= ulangan; angka++) {
    if (gsmTidur == 1) {
      Serial.print(F(" - TCP ke - "));
      Serial.println(angka);
    }

    gsm.print(F("AT+CIPSTART=\"TCP\",\""));

    //ALAMAT HOST SERVER
    gsm.print(host);//54.255.55.184
    gsm.println(F("\",\"4000\""));
    readSerial(200); // harus ada untuk menunggu CONNECT OK

    unsigned long waktu = millis();
    g = '\0';
    while (millis() - waktu < jeda) {
      g = gsm.read();
      if (isAscii(g)) {
        if (gsmTidur == 1)Serial.print(g);
      }
      if (g == 'S' || g == 'C' || g == 'A' || g == 'E') break;
    }

    readSerial(15);
    if (gsmTidur == 1) Serial.println(g);
    if (g == 'C' || g == 'A') {
      hasilTCP = 1;
      break;
    }
    if (g == 'E' || g == 'S') {
      hasilTCP = 0;
    }
  }
  if (gsmTidur == 1) {
    Serial.print(F("\r\nHasil TCP Start="));
    Serial.println(hasilTCP);
  }

  if (hasilTCP == 0) readSerial(2000);
  return hasilTCP;
}

boolean TCPsend() {
  //siapkan variabel kode HTTP
  kode = 0;
  char karakter = '.';

  gsm.println(F("AT+CIPSEND"));
  //  gsm.println(output.length()+output1.length() - 1);
  unsigned long waktu = millis();
  while (millis() - waktu < 500 && karakter != '>') {
    while (gsm.available() > 0 && karakter != '>') {
      karakter = gsm.read();
      if (gsmTidur == 1)Serial.write(karakter);
    }
  }
  if (karakter != '>') return 0;

  json = "";
  delay(10);

  json = "\"s1\": " + String (tekanan, 2) + ",\r\n\"s2\": 0,\r\n\"s3\": 0,\r\n";
  json.concat("\"b\": 0,\r\n\"signature\": \"\",\r\n");
  json.concat("\"device_code\": \"");
  json.concat(String(ID));
  json.concat("\",\r\n");
  json.concat("\"sTime\": \"");
  json.concat(String(sTime));
  json.concat("\",\r\n");
  json.concat("\"lat\": \"");
  json.concat(String(Lat, 6));
  json.concat("\",\r\n");
  json.concat("\"lon\": \"");
  json.concat(String(Long, 6));

  if (gsmTidur == 1) {
    Serial.print("Data JSON=");
    Serial.println(json);
    Serial.println(" ");
    Serial.flush();
  }

  delay(100);

  gsm.print("POST /graphql HTTP/1.1\r\n");
  gsm.print("Host: ");
  gsm.print(host);
  gsm.print(":4000\r\n");
  readSerial(100);
  gsm.print("Content-Type: application/json\r\n");
  readSerial(100);
  gsm.print("Content-Length: ");
  gsm.print(119 + json.length() + 105);
  gsm.print("\r\n\r\n");
  readSerial(100);
  gsm.print( "{\r\n\"operationName\": \"insert_data\",\r\n\"variables\": {\r\n\"data\": {\r\n\"email\": \"");
  //  gsm.print(email);
  gsm.print("admin@gmail.com");
  readSerial(100);
  gsm.print("\",\r\n\"password\": \"");
  //  gsm.print(emailPass);
  gsm.print("admin!2357");
  readSerial(100);
  gsm.print("\",\r\n");
  //  gsm.print(json);

  gsm.print("\"s1\": ");
  gsm.print(tekanan, 2);
  gsm.print(",\r\n\"s2\": 0,\r\n\"s3\": 0,\r\n");
  gsm.print("\"b\": 0,\r\n\"signature\": \"\",\r\n");
  gsm.print("\"device_code\": \"");
  gsm.print(ID);
  gsm.print("\",\r\n");
  readSerial(100);
  gsm.print("\"sTime\": \"");
  gsm.print(sTime);
  gsm.print("\",\r\n");
  readSerial(100);
  gsm.print("\"lat\": \"");
  gsm.print(Lat, 6);
  gsm.print("\",\r\n\"lon\": \"");
  gsm.print(Long, 6);
  gsm.print("\"\r\n}\r\n},\r\n");
  readSerial(100);
  gsm.print("\"query\": \"mutation insert_data($data: DataCreateInput!) {insert_one_data(data: $data) {id}}\"\r\n}");
  gsm.flush();
  readSerial(1000);


  char character = '\0';
  unsigned long waktuAwal = millis();
  // SEND OK
  waktuAwal = millis();
  boolean nilai = 0;
  Serial.write(13);
  Serial.write(10);
  gsm.write(26);
  while (millis() - waktuAwal < 10000 && nilai == 0) {
    nilai = gsm.find("SEND OK");
  }
  //  if (nilai == 0) return 0;


  Serial.println(F("\r\n- TUNGGU KODE HTTP -"));

  //HTTP 1.1
  waktuAwal = millis();
  kode = 0;
  while (millis() - waktuAwal < 10000 && character != ' ') { // && gsm.available()>0
    character = gsm.read();
    if (gsmTidur == 1) {
      if (isAscii(character)) Serial.write(character);
    }
  }

  if (character != ' ') return 0;

  //HTTP RESPONSE
  character == '0';
  kode = 0;
  waktuAwal = millis();
  while (millis() - waktuAwal < 10000 && character != 'O') {
    character = gsm.read();

    if (gsmTidur == 1) {
      if (isAscii(character)) Serial.write(character);
    }
    if (kode < 100) {
      switch (character) {
        case '0' ... '9':
          kode *= 10;
          kode += character - '0';
          break;
      }
    }

  } // while serial available
  //  if (character != 'O') return 0;
  readSerial(500);
  Serial.print(" OK");



  Serial.print(F("\r\nKODE="));
  Serial.println(kode);
  return 1;
}

void TCPclose(unsigned int waktu) {
  gsm.println(F("AT+CIPCLOSE"));
  readSerial(waktu);
}
