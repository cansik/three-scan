class ScreenPrinter
{
  private int maxLines;
  private int currentIndex = 0;

  // style
  private color textColor = color(0, 255, 255);
  private float textSize = 12;
  private int lineSpacing = 13;

  private String[] lines;

  public ScreenPrinter(int maxLines) {
    this.maxLines = maxLines;
    lines = new String[maxLines];

    clear();
  }

  void clear() {
    for (int i = 0; i < maxLines; i++) {
      lines[i] = "";
    }
    currentIndex = 0;
  }

  void add(String text) {
    lines[currentIndex] = text;
    currentIndex = (currentIndex + 1) % maxLines;
  }

  public void render(PGraphics g) {
    g.fill(textColor);
    g.noStroke();
    g.textAlign(LEFT, BOTTOM);
    g.textSize(textSize);

    for (int i = 0; i < maxLines; i++) {
      int index = (i + currentIndex) % maxLines;
      int y = (i * lineSpacing);

      text(lines[index], 0, y);
    }
  }
}
