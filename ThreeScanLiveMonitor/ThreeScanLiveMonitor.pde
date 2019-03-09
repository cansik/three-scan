import peasy.PeasyCam;

import oscP5.*;
import netP5.*;

OscP5 oscP5;
NetAddress mcuAddress;

String mcuPath = "/dev/tty.SLAB_USBtoUART";
boolean isMcuAvailable = false;

PointCloudMCU mcu = new PointCloudMCU(this, mcuPath);

PeasyCam cam;

boolean debug = true;

PointCloudVisualizer visusalizer;
PointCloud cloud;

void setup()
{
  frameRate(60);
  size(800, 600, P3D);
  surface.setTitle("Three Scan Live Monitor");

  cam = new PeasyCam(this, -20, 200, 0, 800);
  cam.rotateX(radians(-75));

  randomSeed(7148);

  // setup renderer
  visusalizer = new PointCloudVisualizer(this);

  // load pointcloud
  cloud = new PointCloud();

  // try to attach
  checkDevices();
  if (isMcuAvailable)
    mcu.attach();

  mcu.start();
}

void draw()
{
  if (oscP5 == null)
  {
    // osc
    oscP5 = new OscP5(this, 9000);
    mcuAddress = new NetAddress("192.168.4.1", 8000);
  }

  checkDevices();

  background(0);

  updateCloudPoints();
  visusalizer.render(this.g, cloud);

  if (mcu.ended) {
    mcu.ended = false;

    // recreate cloud
    cloud.createCloud(mcu.vbo);

    println("cloud recreated!");
  }

  showInfo();
}

void updateCloudPoints()
{
  if (cloud.currentIndex < mcu.syncedPosition - 1) {
    for (int i = cloud.currentIndex; i < mcu.syncedPosition; i++)
    {
      cloud.setNextVertex(mcu.vbo.get(i));
    }
  }
}

void showDebug(String text)
{
  cam.beginHUD();
  fill(0, 255, 0);
  textSize(14);
  text("Debug \"" + text + "\"", 20, height - 50);
  cam.endHUD();
}

void showInfo()
{
  cam.beginHUD();
  fill(255);
  textSize(14);
  text("MCU detected: " + isMcuAvailable + "\nattached: " + (mcu.isAttached()) + "\nFPS: " + round(frameRate), 20, 20);
  cam.endHUD();
}

void keyPressed()
{
  if (key == ' ')
  {
    // try to connect
    if (isMcuAvailable)
      mcu.attach();
    else
      println("No MCU detected!");
  }

  if (key == 'i')
  {
    int seed = (int)random(0, 10000);
    randomSeed(seed);

    println("Seed: " + seed);
  }

  if (key == 's')
  {
    OscMessage msg = new OscMessage("/threescan/start");
    msg.add(1.0f);
    oscP5.send(msg, mcuAddress);
  }

  if (key == 't')
  {
    OscMessage msg = new OscMessage("/threescan/serialtransfer");
    msg.add(1.0f);
    oscP5.send(msg, mcuAddress);
  }
}

void checkDevices()
{
  isMcuAvailable = new File(mcuPath).exists();
}

void oscEvent(OscMessage theOscMessage) {
}
