#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

#include <TFMPlus.h>
TFMPlus tfmP;


// trigger range in cm
unsigned int triggerLowerLimit = 0;
unsigned int triggerUpperLimit = 80;



byte localMac[] = {
  0x0, 0x00, 0x00, 0x00, 0x00, 0x00
};

IPAddress localIp(192, 168, 1, 180);

IPAddress remoteIp(192, 168, 1, 131);
unsigned int remotePort = 4567;



char packetBuffer[UDP_TX_PACKET_MAX_SIZE];
char ReplyBuffer[] = "acknoledged";


EthernetUDP Udp;


void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  // udp setup
  Ethernet.begin(localMac, localIp);

  // check if Ethernet hardware is available, if not, halt
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    while (true) {
      delay(1);
    }
  }

  // check wether there is an Ethernet link, if not, wait until there is
  while(Ethernet.linkStatus() == LinkOFF)
  {
    delay(1);
  }

  // sensor setup
  Serial2.begin(115200);
  delay(20);
  tfmP.begin(&Serial2);

  tfmP.sendCommand( SOFT_RESET, 0);

  delay(500);

  tfmP.sendCommand(SET_FRAME_RATE, FRAME_1000);
  delay(500);
}

int16_t tfDist = 0;  // Distance to object in centimeters
int16_t tfFlux = 0;  // Strength or quality of return signal
int16_t tfTemp = 0;  // Internal temperature of Lidar sensor chip

bool lastSample = false;
bool currentSample = false;

void loop() {

  if(tfmP.getData(tfDist, tfFlux, tfTemp))  // Get data from the device.
  {
    currentSample = (tfDist > triggerLowerLimit && tfDist < triggerUpperLimit);

    if(currentSample != lastSample)
    {
      if(currentSample == true)
      {
        digitalWrite(LED_BUILTIN, HIGH);

        Udp.beginPacket(remoteIp, remotePort);
        Udp.write("rein");
        Udp.endPacket();
      }
      else
      {
        digitalWrite(LED_BUILTIN, LOW);

        Udp.beginPacket(remoteIp, remotePort);
        Udp.write("raus");
        Udp.endPacket();
      }
    }

    lastSample = currentSample;
  }
}