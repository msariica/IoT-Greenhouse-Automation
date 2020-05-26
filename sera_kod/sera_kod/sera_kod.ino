#include <SoftwareSerial.h>                                   //SoftwareSerial kütüphanimizi ekliyoruz.
#include <dht11.h>                                            //DHT11 sensör kütüphanemizi ekliyoruz.


String agAdi = "Muharrem";                                  //Ağımızın adını buraya yazıyoruz.
String agSifresi = "Sarıca";                                //Ağımızın şifresini buraya yazıyoruz.

int rxPin = 0;                                               //ESP8266 RX pini
int txPin = 1;                                               //ESP8266 TX pini
int dht11Pin = 2;                                             //dht11 2 numaralı pine bağlandı
int toprak_nem_sensoru = A5;                                  // toprak nem sensörü analog 5 pinine bağlanacak
int OkunanDeger;                                              // Analog 5'ten okunacak değer (0-1023)
int topraknem;                                                // Nem isminde değişken oluşturuldu
int selenoid_valf = 3;                                        // selenoid valf dijital 3 pinine bağlanacak
int tanker_motor = 12;                                        // tanker motoru dijital 12 pinine bağlanacak
int isitici = 10;                                             // ısıtıcı dijital 10 pinine bağlanacak
int fan = 11;                                                 // fan dijital 11 pinine bağlanacak
int lamba = 13;                                               // lamba dijital 11 pinine bağlanacak
int maxHum = 50;                                              // fanın devreye gireceği nem seviyesi %75
int maxTemp = 25;                                             // lamba 20 derece ve üstündeki sıcaklıklrda devreden çıkar
int suseviye = A1;                                            //sıvı seviye sensörü A1 pinine bağlanacak
int sudeger;                                                  //su seviye sensörüne sudeger ismini atadık
int suseviye2 = A2;                                            //sıvı seviye sensörü A1 pinine bağlanacak
int sudeger2;                                                  //su seviye sensörüne sudeger ismini atadık
int isik_sensor = A4;                                         //ışık sensörünü A4 pinine bağlanacak
int isik_deger;                                               //ışık sensörüne ışık değeri ismini verdik


String ip = "184.106.153.149";                                //Thingspeak ip adresi
float sicaklik, nem;

dht11 DHT11;

SoftwareSerial esp(rxPin, txPin);                             //Seri haberleşme pin ayarlarını yapıyoruz.

void setup() {

  
  pinMode(toprak_nem_sensoru, INPUT);                         // A5 pini toprak nem sensörü için giriş yapıldı
  pinMode(suseviye, INPUT);                                   //A1 pini input olarak tanımlandı
  pinMode(suseviye2, INPUT);                                   //A2 pini input olarak tanımlandı
  pinMode(dht11Pin, INPUT);                                   //2. pin dht11 input olarak tanımlandı
  pinMode(isik_sensor, INPUT);                                //A4 pini ışık sensörü input olarak tanımlandı
  pinMode(selenoid_valf, OUTPUT);                             // 3 pini selenoid valf için çıkış yapıldı
  pinMode(tanker_motor, OUTPUT);                             // 12 pini tanker su motoru için çıkış yapıldı
  pinMode(isitici, OUTPUT);                                 // 10 pini ısıtıcı için çıkış yapıldı
  pinMode(lamba, OUTPUT);                                     // 13 nolu pin lamba için çıkış yapıldı
  pinMode(fan, OUTPUT);                                       // 11 nolu pin fan için çıkış yapıldı


  Serial.begin(9600);                                         //Seri port ile haberleşmemizi başlatıyoruz.
  Serial.println("Started");
  esp.begin(115200);                                          //ESP8266 ile seri haberleşmeyi başlatıyoruz.
  esp.println("AT");                                          //AT komutu ile modül kontrolünü yapıyoruz.
  Serial.println("AT Yollandi");
  while (!esp.find("OK")) {                                   //Modül hazır olana kadar bekliyoruz.
    esp.println("AT");
    Serial.println("ESP8266 Bulunamadi");
  }
  Serial.println("OK Komutu Alindi");
  esp.println("AT+CWMODE=1");                                 //ESP8266 modülünü client olarak ayarlıyoruz.
  while (!esp.find("OK")) {                                   //Ayar yapılana kadar bekliyoruz.
    esp.println("AT+CWMODE=1");
    Serial.println("Ayar Yapiliyor....");
  }
  Serial.println("Client olarak ayarlandı");
  Serial.println("Aga Baglaniliyor...");
  esp.println("AT+CWJAP=\"" + agAdi + "\",\"" + agSifresi + "\""); //Ağımıza bağlanıyoruz.
  while (!esp.find("OK"));                                    //Ağa bağlanana kadar bekliyoruz.
  Serial.println("Aga Baglandi.");
  delay(1000);
}
void loop() {
  esp.println("AT+CIPSTART=\"TCP\",\"" + ip + "\",80");       //Thingspeak'e bağlanıyoruz.
  if (esp.find("Error")) {                                    //Bağlantı hatası kontrolü yapıyoruz.
    Serial.println("AT+CIPSTART Error");
  }
  DHT11.read(dht11Pin);
  sicaklik = (float)DHT11.temperature;
  nem = (float)DHT11.humidity;
  delay(1000);  // 1 saniye bekle
  OkunanDeger = analogRead(toprak_nem_sensoru);                                   // Toprak nem sensöründen okunan değer, OkunanDeger adında bir değişken olarak atandı
  topraknem = map (OkunanDeger, 1023, 0, 0, 160);                                 // Toprak nemi sensöründen 5 volt ile 0 volt arasındaki okunan değer yüzde olarak ifade edildi
  delay(500);  // yarım saniye bekle


  // ısı belirlenen seviyeyi geçerse şunları yap burada başladı
  if ( sicaklik > maxTemp)
  {
    digitalWrite(fan, HIGH);                                                         //fanı çalıştır
    digitalWrite(isitici, LOW);                                                      //ısıtıcıyı durdur
    delay(1000);                                                                    // 1 saniye bekle
  }
  // ısı belirlenen seviyeyi geçerse şunları yap burada bitti

  // ısı belirlenen seviyeyinin altında ise şunları yap burada başladı
  else
  {
    digitalWrite(fan, LOW); //fanı kapat
    digitalWrite(isitici, HIGH);  //ısıtıcıyı çalıştır
    delay(10000); // ısıtıcının çalışma süresi
    digitalWrite(isitici, LOW);   // ısıtıcıyı kapat
  }
  // ısı belirlenen seviyeyinin altında ise şunları yap burada bitti
  
  // nem belirlenen seviyeyi geçerse şunları yap burada başladı
  if (nem > maxHum)
  {
    digitalWrite(fan, HIGH);   // havalandırma fanını çalıştır
    delay(60000);  // fanın devrede kalacağı süre 1 dakika olarak belirlendi
    digitalWrite(fan, LOW);   // fanı kapat
  }
  // nem belirlenen seviyeyi geçerse şunları yap burada bitti

  // nem belirlenen seviyeyinin altındaysa şunları yap burada başladı
  else
  {
    digitalWrite(fan, LOW); // havalandırma fanı kapatıldı
    delay(2000); // 2 sn bekle
  }
  // nem belirlenen seviyeyi altındaysa şunları yap bitti



  toprak_nem_sensoru = analogRead(A5);                                                // Toprak nem sensöründen gelen bilgi okundu
  if (toprak_nem_sensoru > 543)                                                       // 543 değeri %85'e tekabul etmektedir
  {

    digitalWrite(selenoid_valf, LOW);                                                  // selenoid valf kapalı
    delay(1000);
  }
  else
  {

    digitalWrite(selenoid_valf, HIGH);                                                   // selenoid açıldı
    delay(1000);                                                                        // 1 saniye bekle
  }


  //suseviye yani A1 pinine bağlı sıvı seviye sensörünü okuyan kısım
  sudeger = analogRead(suseviye);
  if (sudeger <=300)
  {
    Serial.println("Su seviyesi cok az");
    digitalWrite(tanker_motor, HIGH);  
    delay(1000);                                                            
  } 
  //suseviye2 yani A2 pinine bağlı sıvı seviye sensörünü okuyan kısım
  sudeger2 = analogRead(suseviye2);
 if (sudeger2 > 700)
  {
    Serial.println("Maksimum su seviyesi");
    digitalWrite(tanker_motor, LOW);                                                             
  }

  // ışık sensörünü okuyan kısım 
  isik_deger = analogRead(isik_sensor);
  if(isik_deger > 900){                         //Okunan ışık değeri 900'den büyük ise yani hava aydınlık ise
    digitalWrite(lamba,LOW);                    //LED yanmasın
  }
  else
  {
    digitalWrite(lamba , HIGH);
  }
  


  String veri = "GET https://api.thingspeak.com/update?api_key=76IFPUPYCFGGLFWZ";   //Thingspeak komutu. Key kısmına kendi api keyimizi yazıyoruz.
  veri += "&field1=";
  veri += String(sicaklik);                                                         //Göndereceğimiz sıcaklık değişkeni
  veri += "&field2=";
  veri += String(nem);                                                              //Göndereceğimiz nem değişkeni
  veri += "&field3=";
  veri += String(OkunanDeger);                                    //toprak nem bilgisini gönderiyoruz.
  veri += "&field4=";
  veri += String(suseviye);                                      //suyun azaldığı veriyi gönderiyoruz.
  veri += "&field=";
  veri += String(isik_deger);                                   //ışık değerini gönderiyoruz.
  veri += "\r\n\r\n\r\n\r\n\r\n";
  esp.print("AT+CIPSEND=");                                   //ESP'ye göndereceğimiz veri uzunluğunu veriyoruz.
  esp.println(veri.length() + 5);
  delay(2000);
  if (esp.find(">")) {                                        //ESP8266 hazır olduğunda içindeki komutlar çalışıyor.
    esp.print(veri);                                          //Veriyi gönderiyoruz.
    Serial.println(veri);
    Serial.println("Veri gonderildi.");
    delay(1000);
  }
  Serial.println("Baglantı Kapatildi.");
  esp.println("AT+CIPCLOSE");                                //Bağlantıyı kapatıyoruz
  delay(1000);                                               //Yeni veri gönderimi için 1 dakika bekliyoruz.
}
