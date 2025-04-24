import javax.swing.*;
import java.awt.*;
import java.util.ArrayList;
import java.util.List;
import java.io.File;
import java.util.Scanner;
import java.io.IOException;

public class GraphDrawer extends JPanel
{
    String matrix_file;
    String groups_file;
    int graph_width;
    int graph_height;
    int cellSize;

    public GraphDrawer(String matrix_file) {
        this.matrix_file = matrix_file;
        this.groups_file = null;
    }

    public GraphDrawer(String matrix_file, String groups_file) {
        this.matrix_file = matrix_file;
        this.groups_file = groups_file;
    }

    @Override
    public Dimension getPreferredSize() {
        return new Dimension(graph_width * cellSize + 40, graph_height * cellSize + 40);
    }    

    @Override
    protected void paintComponent(Graphics g) {
        super.paintComponent(g);
        setBackground(Color.WHITE);
        g.setColor(Color.BLACK);
        g.setFont(new Font("Arial", Font.BOLD, 18));

        cellSize = 60;
        int circleSize = 40;
        int margin = (cellSize - circleSize) / 2;

        List<Point> node_position = new ArrayList<>();

        try (Scanner matrix_scanner = new Scanner(new File(matrix_file))) {
            String t = "";
            int row = 0;
            int col = 0;
            int bracket_depth = 0;
            boolean still_in_matrix = true;

            while (still_in_matrix && matrix_scanner.hasNext()) {
                t = matrix_scanner.next();
                switch (t) {
                    case "[":
                        graph_height = ++row;
                        graph_width = col + 1;
                        col = 0;
                        bracket_depth++;
                        break;

                    case "]":
                        bracket_depth--;
                        break;
                        
                    case "0":
                    case "1":
                        if (bracket_depth == 0) {
                            still_in_matrix = false;
                            break;
                        }
                        if (t.equals("1")) {
                            int x = col * cellSize + margin + 20;
                            int y = (row-1) * cellSize + margin + 20;
                            node_position.add(new Point(x + circleSize/2, y + circleSize/2)); //wspł. środka koła
                        }
                        col++;
                        break;
                
                    default:
                        break;
                }
            } //w tym momencie t = pierwszy token nowej linii po końcu macierzy

            while (matrix_scanner.hasNext()) {
                int node1 = (t.equals("-")) ? matrix_scanner.nextInt() : Integer.parseInt(t); // przy 1szym obrocie pętli, node1 trzeba odzyskać z t
                t = matrix_scanner.next(); // wczytujemy - z "0 - 4"
                int node2 = matrix_scanner.nextInt();
                g.drawLine(node_position.get(node1).x, node_position.get(node1).y, node_position.get(node2).x, node_position.get(node2).y);
            }
            
            int node_number = 0;
            for (Point node : node_position) {
                String number = String.valueOf(node_number);
                if (groups_file == null)
                    g.setColor(Color.green);
                else
                    SetColorByGroup(g, GetGroup(node_number, groups_file));

                g.fillOval(node.x - circleSize/2, node.y - circleSize/2, circleSize, circleSize); //wspł. lewego górnego "rogu" koła
                g.setColor(Color.black);
                FontMetrics fm = g.getFontMetrics();
                int textX = node.x - fm.stringWidth(number)/2;
                int textY = node.y + fm.getAscent()/2;
                g.drawString(number, textX, textY);
                node_number++;
            }
        } 
        catch (IOException e) {
            System.out.println("Błąd podczas odczytu pliku: " + e.getMessage());
            return;
        }
    }

    public void SetColorByGroup(Graphics g, int group) {
        if (group == 1)         g.setColor(Color.green);
        else if (group == 2)    g.setColor(Color.cyan);
        else if (group == 3)    g.setColor(Color.magenta);
        else if (group == 4)    g.setColor(Color.orange);
        else if (group == 5)    g.setColor(Color.pink);
        else if (group == 6)    g.setColor(Color.yellow);
        else if (group == 7)    g.setColor(Color.blue);
        else                    g.setColor(Color.gray);
    }

    private int GetGroup(int node, String groups_file) {
        try (Scanner groups_scanner = new Scanner(new File(groups_file))) {
            int group = 1;
            int next = groups_scanner.nextInt();
            while (node != next && groups_scanner.hasNextInt()) {
                int before = next;
                next = groups_scanner.nextInt();
                if (next < before) group++; // węzły są uporządkowane rosnąco, więc zmniejszenie numeru oznacza następną linię/grupę
            }
            return group;
        } 
        catch (IOException e) {
            System.out.println("Błąd podczas odczytu pliku: " + e.getMessage());
            return -1;
        }
    }

    public static void main(String[] args) {
        JFrame frame = new JFrame("Graph Drawer");
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.setSize(1300, 800);
        // utworzenie obiektu rysującego macierz
        GraphDrawer drawGraph;
        if (args.length == 1) {
            drawGraph = new GraphDrawer(args[0]); //w javie nazwa programu nie zaśmieca args[]
        }
        else if (args.length == 2) {
            drawGraph = new GraphDrawer(args[0], args[1]);
        }
        else {
            System.out.println("Niewłaściwa ilość argumentów wejściowych. Podaj nazwę jednego lub dwóch plików.");
            return;
        }
        // umieszczenie go w JScrollPane
        JScrollPane scrollPane = new JScrollPane(drawGraph);
        scrollPane.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);
        scrollPane.setHorizontalScrollBarPolicy(JScrollPane.HORIZONTAL_SCROLLBAR_ALWAYS);
        frame.add(scrollPane);
        frame.setVisible(true);
    }
}
