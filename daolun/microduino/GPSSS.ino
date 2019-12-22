#include <Wire.h> 
#include <ESP8266.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#define SSID        F("wheat-226") //WIFI名称
#define PASSWORD    F("19980828") //WIFI密码
#define HOST_NAME   "172.81.254.250"
#define HOST_PORT   (3005)
#define id    1
String mCottenData;
String postString;                                //用于存储发送数据的字符串
TinyGPSPlus gps;                         // The TinyGPS++ object 
SoftwareSerial mySerial(10, 11);  
ESP8266 wifi(&mySerial);
long double lat_d,lon_d;
char  lat_c[15], lon_c[15], isempty_c[2] ;      //定义gps，人体红外数据转换成数组
int isempty;                                  //定义人体红外变量，有人为1，无人为0
int inPin = 12;                               //人体红外引脚接口
void uqur();
void setup(){             //初始化函数
  Serial.begin(9600);     //串口初始化
  pinMode(inPin,INPUT);  
  Serial.print(F("setup begin\r\n"));
  delay(100);
  WifiInit(mySerial,9600);
  Serial.print(F("FW Version:"));
  Serial.println(wifi.getVersion().c_str());
  if (wifi.setOprToStationSoftAP()) {
    Serial.print(F("to station + softap ok\r\n"));
  } else {
    Serial.print(F("to station + softap err\r\n"));
  }
  if (wifi.joinAP(SSID, PASSWORD)) {               //加入无线网
    Serial.print(F("Join AP success\r\n"));
    Serial.print(F("IP:"));
    Serial.println( wifi.getLocalIP().c_str());
  } else {
    Serial.print(F("Join AP failure\r\n"));
  }
 if (wifi.disableMUX()) {
    Serial.print(F("single ok\r\n"));
  } else {
    Serial.print(F("single err\r\n"));
  }
  Serial.print(F("setup end\r\n"));
}
void loop(){
   uqur();        //调用函数
   delay(100);   
}
void uqur(){
 isempty=digitalRead(inPin);
 while ( Serial.available() > 0){
  gps.encode( Serial.read());
  if (gps.location.isUpdated()){
     lat_d=gps.location.lat();
     lon_d=gps.location.lng();
     dtostrf(lat_d, 3, 10, lat_c);
     dtostrf(lon_d, 3, 10, lon_c);
     dtostrf(isempty, 1, 0, isempty_c);
     delay(1000);
  if (wifi.createTCP(HOST_NAME, HOST_PORT)) { //建立TCP连接，如果失败，不能发送该数据
    Serial.print("create tcp ok\r\n");
    //postString将存储传输请求，格式很重要,发送内容：/process?id=1&lat=纬度&lon=经度&isempty=有人为1无人为0
    postString = "POST ";               //post发送方式，后要有空格
    postString += "/process?id=";    //接口process
    postString += id;                 //要发送的数据
    postString += "&lat=";
    postString += lat_c;
    postString += "&lon=";
    postString += lon_c;
    postString += "&isempty=";
    postString += isempty;
    postString += " HTTP/1.1";                 //空格+传输协议
    postString += "\r\n";
    postString += "Host: ";                         //Host：+空格
    postString += HOST_NAME;
    postString += "\r\n";
    postString += "Content-Type: application/x-www-form-urlencoded\r\n";  //编码类型
    postString += "Connection: close\r\n";
    postString += "\r\n";                                             //不可删除
    const char *postArray = postString.c_str();                 //将str转化为char数组
    Serial.println(postArray);
    wifi.send((const uint8_t*)postArray, strlen(postArray));    //send发送命令，参数必须是这两种格式，尤其是(const uint8_t*)
    Serial.println("send success");   
    if (wifi.releaseTCP()) {                                 //释放TCP连接
        Serial.print("release tcp ok\r\n");
        } 
     else {
        Serial.print("release tcp err\r\n");
        }
      postArray = NULL;                                       //清空数组，等待下次传输数据
  }  else {
    Serial.print("create tcp err\r\n");
    }
   } 
  }
 }

 
 
