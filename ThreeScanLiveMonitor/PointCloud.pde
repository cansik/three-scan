class PointCloud
{
  PVector dimensions = new PVector();
  PVector center = new PVector();

  PShape vertices = new PShape();

  float scale = 1.0f;
  PVector translation = new PVector();

  float pointScale = 1f;
  color pointColor = color(255, 255, 255);

  int currentIndex = 0;

  public PointCloud()
  {
    initVertexArray(50 * 1000);
  }

  public void setNextVertex(Vertex v)
  {
    if (currentIndex < vertices.getVertexCount())
    {
      vertices.setVertex(currentIndex, v.position.x, v.position.y, v.position.z);
      currentIndex++;
    }
  }

  public void initVertexArray(int size)
  {
    vertices = createShape();
    vertices.beginShape(POINTS);

    for (int i = 0; i < size; i++)
    {
      vertices.strokeWeight(1.0f);
      vertices.stroke(255);
      vertices.vertex(0, 0, 0);
    }

    vertices.endShape();
  }

  public void createCloud(ArrayList<Vertex> data)
  {
    vertices = createShape();
    vertices.beginShape(POINTS);

    for (int i = 0; i < data.size(); i++)
    {
      Vertex v = data.get(i);
      vertices.strokeWeight(1.0f);

      vertices.fill(color(v.signalStrength, 100, 255));
      vertices.stroke(color(v.signalStrength, 100, 255));

      vertices.vertex(v.position.x, v.position.y, v.position.z);
    }

    vertices.endShape();
  }
}
