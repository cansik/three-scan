class PointCloudMCU extends MicroControllerUnit implements Runnable
{
  volatile boolean running = false;

  Thread thread;

  ArrayList<Vertex> vbo;

  volatile int syncedPosition = 0;
  volatile boolean ended = false;

  public PointCloudMCU(PApplet parent, String portName)
  {
    super(parent, portName);
  }

  void start()
  {
    running = true;
    ended = false;
    vbo = new ArrayList<Vertex>();

    thread = new Thread(this);
    thread.setDaemon(true);
    thread.start();
  }

  void stop()
  {
    running = false;

    try
    {
      thread.join(5000);
    }
    catch(Exception ex) 
    {
      ex.printStackTrace();
    }
  }

  void run()
  {
    while (running)
    {
      try
      {
        Thread.sleep(1);
        readScanData();
      }
      catch(Exception ex) 
      {
        ex.printStackTrace();
      }
    }
  }

  void readScanData()
  {
    if (!isAttached())
      return;

    String raw = readData();

    if (debug && raw != null && !raw.startsWith("TST") && !raw.trim().equals(""))
    {
      println("MCU: " + raw.trim());
    }

    if (raw == null || !raw.startsWith("TST"))
      return;

    // relevant data
    String[] data = raw.split(":");
    try
    {
      String header = data[0].trim();
      String cmd = data[1].trim();

      if (cmd.equals("SYN")) {
        // sync
        println("sync received!");

        syncedPosition = vbo.size();
      }

      if (cmd.equals("DAT")) {
        // data point
        float x = parseFloat(data[2].trim());
        float y = parseFloat(data[3].trim());
        float z = parseFloat(data[4].trim());
        int signalStrength = parseInt(data[5].trim());

        vbo.add(new Vertex(new PVector(x, y, z), signalStrength));
        println("datapoint: " + x + " / " + y + " / " + z + " | " + signalStrength);
      }

      if (cmd.equals("END")) {
        ended = true;
      }
    }
    catch(Exception ex)
    {
    }
  }
}
