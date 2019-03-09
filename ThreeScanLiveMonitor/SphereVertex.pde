class SphereVertex
{
  float azimuthalAngle;
  float polarAngle;
  int r;
  int signalStrength;

  public SphereVertex(float azimuthalAngle, float polarAngle, int r, int signalStrength)
  {
    this.azimuthalAngle = azimuthalAngle;
    this.polarAngle = polarAngle;
    this.r = r;
    this.signalStrength = signalStrength;
  }
}
