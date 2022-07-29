#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

#define WIFI_SSID "wifi_ID"
#define WIFI_PASSWORD "wifi_Password"
#define BOT_TOKEN "private_Token"

X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);
unsigned long bot_lasttime;

const unsigned long BOT_MTBS = 1000;

const int motion_pin = 4;
int motion = 0;
const int ledPin = LED_BUILTIN;
int ledStatus = 0;
const int control_led = 0;

void handleNewMessages(int numNewMessages)
{
  Serial.print("handleNewMessages ");
  Serial.println(numNewMessages);
  
  if(motion ==1)
    {
    String chat_id ="216600224";
    digitalWrite(ledPin, LOW); // turn the LED on (HIGH is the voltage level)
      ledStatus = 1;
      bot.sendMessage(chat_id, "There is someone in your house!", "");
      motion ==0;}
      
  for (int i = 0; i < numNewMessages; i++)
  { String chat_id = bot.messages[i].chat_id;
    String text = bot.messages[i].text;
    String from_name = bot.messages[i].from_name;
   
    if (from_name == "")
      from_name = "User_Name";

    if (text == "/turn_off_alarm" || text =="/Turn off alarm")
    { ledStatus = 0;
      digitalWrite(ledPin, HIGH); // turn the LED off (LOW is the voltage level)
      bot.sendMessage(chat_id, "Alarm devre dışı bırakıldı", "");
      Serial.print("chat_id");
      Serial.println(chat_id);}
    
    if (text == "/call_police" || text =="/Call police")
    { bot.sendMessage(chat_id, "Police calling", "");}

    if (text == "/status" || text =="/status")
    {if (ledStatus)
      {bot.sendMessage(chat_id, "There is someone in your house", "");}
     else
      {bot.sendMessage(chat_id, "Your home is safe", "");
      }}

      
    if (text == "/help" || text =="/Help" )
    {
      String welcome = "Hi " + from_name + " welcome to telegram bot. " + "\n";
      welcome += "/turn_off_alarm : To disable the alarm \n";
      welcome += "/call_police :to contact the police\n";
      welcome += "/status : to know the status of your instant home\n";
      bot.sendMessage(chat_id, welcome, "Markdown");
    }}}


void bot_setup()
{
  const String commands = F("["
                            "{\"command\":\"help\",  \"description\":\"Allows you to see commands in detail\"},"
                            "{\"command\":\"turn_off_alarm\", \"description\":\"Lets you turn off the alarm\"},"
                            "{\"command\":\"status\",\"description\":\"Shows the safety status of your home\"}" // no comma on last command
                            "]");
  bot.setMyCommands(commands);
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  pinMode(motion_pin, INPUT);
  pinMode(control_led, OUTPUT);
  digitalWrite(ledPin, HIGH);
 
  configTime(0, 0, "pool.ntp.org");
  secured_client.setTrustAnchors(&cert);

  Serial.print("Connecting to Wifi SSID ");
  Serial.print(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.print("\nWiFi connected. IP address: ");
  Serial.println(WiFi.localIP());
  digitalWrite(control_led,HIGH);
  Serial.print("Retrieving time: ");
  time_t now = time(nullptr);
  while (now < 24 * 3600)
  {
    Serial.print(".");
    delay(100);
    now = time(nullptr);
  }
  Serial.println(now);
  bot_setup();
}


void loop() {
  // put your main code here, to run repeatedly:
  motion = digitalRead(motion_pin);
  Serial.println("+++++++++");
  
  if (millis() - bot_lasttime > BOT_MTBS  || motion ==1)
  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    if (numNewMessages || motion ==1)
    {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
      Serial.print("motion  =");
      Serial.println(motion);
    }

    bot_lasttime = millis();
  }

}
