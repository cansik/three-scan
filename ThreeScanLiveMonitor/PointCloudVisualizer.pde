class PointCloudVisualizer
{
  PApplet parent;
  PShader pointShader;

  // settings
  float colorMix = 0.0f;
  color backgroundColor = color(12);

  public PointCloudVisualizer(PApplet parent)
  {
    this.parent = parent;
    pointShader = parent.loadShader("shader/pointColor.glsl", "shader/pointVertex.glsl");
  }

  public void render(PGraphics g, PointCloud cloud)
  {
    beginVisualizer(g, cloud);

    pointShader.set("pointScale", cloud.pointScale);
    pointShader.set("pointColor", 
      red(cloud.pointColor) / 255f, 
      green(cloud.pointColor) / 255f, 
      blue(cloud.pointColor) / 255f, 
      alpha(cloud.pointColor) / 255f);
    pointShader.set("colorMix", colorMix);


    // draw cloud
    g.shape(cloud.vertices);

    endVisualizer(g);
  }

  public void beginVisualizer(PGraphics g, PointCloud cloud)
  {
    g.background(backgroundColor);
    g.shader(pointShader);
    g.push();
    g.translate(cloud.translation.x, cloud.translation.y, cloud.translation.z);
    g.scale(cloud.scale);
  }

  public void endVisualizer(PGraphics g)
  {
    g.pop();
    g.resetShader();
  }
}
