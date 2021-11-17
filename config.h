//#define debug
String email="pdambogor@gmail.com";
String emailPass="bogor12345";
//VARIABLES
String ID; // unit ID
byte interval ;// menit
float offset = 0.0;
float Lat=-6.569142;
float Long=106.786768;
String apn="internet";
String username="";
String password="";
char sTime[]="2021-07-13T15:15:55.000Z";
String host="13.229.88.241";

#define burst 3 // second
byte indeksB = 10; // waktu tunggu GSM dalam detik

#define pres            A1 //channel
#define baud 9600
#define rtc_addr    0x68
const byte SSpin = 53 ;    // pin Slave Select
char NamaFile[14]="LOG1.txt";


boolean gsmTidur = 0; // indikator GSM sedang tidur atau aktif
boolean GSMerror = 0; //indikator GSM error atau tidak
int a,b, i, kode, tahun, nomor=1;
char karakter, A='m';
byte indeksA, hari1=0;
byte bulan, hari, jam, menit, detik;
unsigned long reads = 0; //pressure
unsigned int reads1 = 0; //voltage
unsigned long awal, akhir, start;
float tekanan, volt;
String kalimat;
String result, json;
