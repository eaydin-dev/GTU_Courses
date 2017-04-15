
import javax.swing.JFrame;
import java.awt.event.*;
import java.awt.BorderLayout;
import java.awt.Rectangle;
import javax.swing.JFileChooser;
import javax.swing.JOptionPane;
import javax.swing.JLabel;
import javax.swing.JButton;
import javax.swing.JPanel;
import java.awt.Color;
import javax.swing.JScrollBar;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileWriter;
import java.io.IOException;
import java.util.Scanner;
import java.util.Timer;
import java.util.TimerTask;
import java.awt.Toolkit;
import javax.swing.JTextPane;
import javax.swing.SwingConstants;
import java.awt.Dimension;
import java.awt.FlowLayout;

/**
 * The class GameOfLife.
 *
 * @author Eyyup Aydin
 * @since 3.1.2015
 */

public class GameOfLife implements MouseListener{

	JFrame frmGameOfLife;
	private static int generation = 0;
	private JButton btnLoad;
	private JButton btnSave;
	private JButton oneStep;
	private JButton numStep;
	private JButton infStep;
	private JButton btnStop;
	private JButton btnReset;	
	private JButton btnAbout;
	private JPanel buttonPanel;
	private JTextPane txtpnTotalCells;
	private JTextPane txtpnGeneration;
	private JLabel labs[][] = new JLabel[100][100];
    private Cell[] cells = new Cell[10000];
    private JFileChooser fileDialog;
    private boolean cont = true;
      

	/**
	 * Create the application.
	 */
	public GameOfLife() {
		initialize();
	}

	/**
	 * Initialize the contents of the frame.
	 */
	private void initialize() {	
		initFrame();		
		fileDialog = new JFileChooser();
		
		// initialize two dimensional JPanel cellsay
		for(int i = 0; i < 100; ++i){
			for(int j = 0; j < 100; ++j)
				labs[i][j] = new JLabel();
		}
		
		// main panel
		JPanel panel = new JPanel();
		panel.setBackground(new Color(204, 255, 153));
		frmGameOfLife.getContentPane().add(panel, BorderLayout.CENTER);
		panel.setLayout(null);
		
		// scroll bars
		JScrollBar hbar=new JScrollBar(JScrollBar.HORIZONTAL, 30, 20, 0, 500);
        JScrollBar vbar=new JScrollBar(JScrollBar.VERTICAL, 30, 40, 0, 100);
        frmGameOfLife.getContentPane().add(hbar, BorderLayout.SOUTH);
        frmGameOfLife.getContentPane().add(vbar, BorderLayout.EAST);
		
        // buttons panel
		buttonPanel = new JPanel();
		buttonPanel.setBackground(Color.ORANGE);
		buttonPanel.setBounds(10, 11, 68, 335);
		panel.add(buttonPanel);
		
		createButtons();
		
        
		int j = 0;
    	int k = 90;    	
    	int y = 0;
    	
		for(int z = 0; z < 100; ++z){
	    	for(int i = 0; i < 100; ++i){
	    		cells[y++] = new Cell(k,j);	
	    		k += 10;
	    	}
	    	
	    	k = 90;
	    	j += 10;
		}
	    
		int f = 0;
		y = 0;
		for(int i = 0; i < 100; ++i){
			for(int z = 0; z < 100; ++z){
				labs[f][z].setText(cells[y].toString());
		    	labs[f][z].setBounds(cells[y].getX(), cells[y].getY(), 20, 20);
		    	
		    	labs[f][z].addMouseListener(this);
		    	panel.add(labs[f][z]);	
		    	++y;
			}
			++f;  		
	    }
	    
		
	}
	
	/**
	 * Initialize the frame.
	 */
	private void initFrame(){
		frmGameOfLife = new JFrame();
		frmGameOfLife.setIconImage(Toolkit.getDefaultToolkit().getImage(GameOfLife.class.getResource("/com/sun/javafx/webkit/prism/resources/mediaPlayDisabled.png")));
		frmGameOfLife.setTitle("Game Of Life");
		frmGameOfLife.setBounds(100, 100, 627, 506);
		frmGameOfLife.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		Rectangle bounds = new Rectangle(100, 0, 1125, 695);
		frmGameOfLife.setMaximizedBounds(bounds);
	}
	
	/**
	 * Create all buttons in frame.
	 */
	private void createButtons(){
		
		oneStep = new JButton(">");
		Dimension d = new Dimension();
		d.setSize(50, 25);
		oneStep.setSize(new Dimension(50, 10));
		oneStep.addMouseListener(new MouseAdapter() {
			@Override
			public void mouseClicked(MouseEvent e) {
				play();
				update_table();
			}
		});
		
		btnReset = new JButton("Reset");
		btnReset.addMouseListener(new MouseAdapter() {
			@Override
			public void mouseClicked(MouseEvent e) {
				for(int i = 0; i < 10000; ++i)
					cells[i].toggle = false;
				
				update_table();
			}
		});
		
		btnStop = new JButton("Stop");
		btnStop.addMouseListener(new MouseAdapter() {
			@Override
			public void mouseClicked(MouseEvent e) {
				cont = false;
			}
		});
		
		infStep = new JButton(">>>");
		infStep.addMouseListener(new MouseAdapter() {
			@Override
			public void mouseClicked(MouseEvent e) {
				Timer timer = new Timer();
				timer.scheduleAtFixedRate(new TimerTask (){
				    public void run() {
				    	if(!cont)
							this.cancel();
				    	play();
						update_table();					
				    }
				}, 0, 250); 
				cont = true;
			}
			
		});
		
		btnSave = new JButton("Save");
		btnSave.addMouseListener(new MouseAdapter() {
			@Override
			public void mouseClicked(MouseEvent e) {
				try {
					writeFile();
				} catch (IOException e1) {
					// TODO Auto-generated catch block
					e1.printStackTrace();
				}
				JOptionPane.showMessageDialog(null, "Done.");
			}
		});
		btnLoad = new JButton("Load");
		btnLoad.addMouseListener(new MouseAdapter() {
			@Override
			public void mouseClicked(MouseEvent e) {
				
				try {
					readFile();
				} catch (FileNotFoundException e1) {
					// TODO Auto-generated catch block
					e1.printStackTrace();
				}
				update_table();
			}
		});
		
		txtpnTotalCells = new JTextPane();
		txtpnTotalCells.setForeground(Color.WHITE);
		txtpnTotalCells.setEditable(false);
		txtpnTotalCells.setBackground(Color.MAGENTA);
		txtpnTotalCells.setText("Total Cells:\n        " + numOfAlives());
		
		txtpnGeneration = new JTextPane();
		txtpnGeneration.setForeground(Color.WHITE);
		txtpnGeneration.setBackground(Color.MAGENTA);
		txtpnGeneration.setText("Generation:\n        " + generation);
		
		btnAbout = new JButton("About");
		btnAbout.addMouseListener(new MouseAdapter() {
			@Override
			public void mouseClicked(MouseEvent arg0) {
				JOptionPane.showMessageDialog(null, "Game Of Life v1.0\nImplemented by Eyyüp Aydın\nSince January 2015");
			}
		});
		btnAbout.setVerticalAlignment(SwingConstants.BOTTOM);
		buttonPanel.setLayout(new FlowLayout(FlowLayout.CENTER, 5, 5));
		
		
		
		numStep = new JButton(">>");
		numStep.addMouseListener(new MouseAdapter() {
			@Override
			public void mouseClicked(MouseEvent e) {
				int num = Integer.parseInt(JOptionPane.showInputDialog("How many steps?"));
				
				Timer timer = new Timer();
				timer.scheduleAtFixedRate(new TimerTask (){
					int i = 1;
				    public void run() {
				    	if(i == num)
				    		this.cancel();
				    	play();
						update_table();
						++i;
				    }
				}, 0, 250);
			}
		});
		
		buttonPanel.add(oneStep);
		buttonPanel.add(numStep);
		buttonPanel.add(infStep);
		buttonPanel.add(btnStop);
		buttonPanel.add(btnReset);
		buttonPanel.add(btnLoad);
		buttonPanel.add(btnSave);
		buttonPanel.add(txtpnTotalCells);
		buttonPanel.add(txtpnGeneration);
		buttonPanel.add(btnAbout);
	}
	
	/**
	 * Reads the initial state of game from file.
	 * @throws FileNotFoundException
	 */
    public void readFile() throws FileNotFoundException {
    	fileDialog.showOpenDialog(frmGameOfLife);
        File f = fileDialog.getSelectedFile();
        Scanner inp = new Scanner(f);

        if(f.exists() | f.canRead()){
        	int num = inp.nextInt();
        	for(int i = 0; i < num; ++i){
        		cells[inp.nextInt()].toggle = true;
        		inp.nextInt(); inp.nextInt();
        	}

        }
        inp.close();

    }	
    
	/**
     * Saves the game to a file.
	 * @throws IOException 
     */
    public void writeFile() throws IOException {
    	fileDialog.showSaveDialog(frmGameOfLife);       

        try {
           File outp = null;
			outp = fileDialog.getSelectedFile();
			BufferedWriter output = new BufferedWriter(new FileWriter(outp));
			
			output.write(numOfAlives() + "\n\n");
	           
	        for (int j = minCol(); j <= maxCol(); j += 10){
	        	for (int i = minRow(); i <= maxRow(); i += 10){     
	        		int where = search(i, j);
	                if (where != -99999){
	                   if(cells[where].toggle)
	                	   output.write(where + " " + cells[where].getX() + " " + cells[where].getY() + "\n");
	                }
	                       
	            }
	        }
	            
	        output.close();
				
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
    }
	
    /**
     * Calculates the alive cells in game.
     * @return Number of alive cells.
     */
    public int numOfAlives(){
    	int c = 0;
    	if(generation > 0){   		
        	for(int i = 0; i < 10000; ++i)
        		if(cells[i].toggle)
        			++c;      	
    	} 	
    	return c;
    }
    
    /**
     * Searches a cell in game.
     * @param r: The x value of cell.
     * @param c: The y value of cell.
     * @return The index of cell in cellsay. If there is no cell, returns -99999.
     */
    public int search(int r, int c){
    	for(int i = 0; i < 10000; ++i)
    		if(cells[i].getX() == r & cells[i].getY() == c)
    			return i;
    	
    	return -99999;
    }
    
    /**
     * Updates the state of game.
     */
    private void update_table(){
    	for(int i = 0; i < 100; ++i)
    		for(int j = 0; j < 100; ++j)
    			labs[i][j].setText(cells[search(labs[i][j].getX(), labs[i][j].getY())].toString());
    	
    	txtpnTotalCells.setText("Total Cells:\n         " + numOfAlives());
    	txtpnGeneration.setText("Generation:\n        " + generation);
    }
    
    /**
     * Plays the game for a single step.
     */
    public void play() {
    	++generation;
        int neighbours = 0;
        Cell[] temp = new Cell[10000];
        
		for(int z = 0; z < 10000; ++z){
			temp[z] = new Cell(cells[z].getX(), cells[z].getY());
			temp[z].toggle = cells[z].toggle;
		}
			
        for(int i = minRow() - 10; i <= maxRow() + 10; i += 10)
            for(int j = minCol() - 10; j <= maxCol() + 10; j += 10){
            	int where = search(i, j);
            	if(where != -99999){
                    if(!cells[where].toggle){
                        neighbours = numOfNeighbours(i, j);
                        if(neighbours == 3)
                            temp[where].toggle = true;
                    }

                    else{
                        neighbours = numOfNeighbours(i, j);
                        if(neighbours < 2 || neighbours > 3)
                        	temp[where].toggle = false;
                        
                    }
            	}
            	
            }
        
        for(int z = 0; z < 10000; ++z){
	    	cells[z].setXY(temp[z].getX(), temp[z].getY());
        	cells[z].toggle = temp[z].toggle;
        }
    }
    
    /**
     * Checks whether a cell is alive or not.
     * @param x: The x value of cell.
     * @param y: The y value of cell.
     * @return True if alive; false if not.
     */
    private boolean isAlive(int x, int y){
    	int w = search(x, y);
    	if(w != -99999)
    		return cells[w].toggle;
    	
    	return false;
    }
    
    /**
     * Calculates a cell's number of neighbours.
     * @param r: The x value of cell.
     * @param c: The y value of cell.
     * @return Number of neighbours of cell.
     */
    private int numOfNeighbours(int r, int c) {
        int counter = 0;

        if (isAlive(r + 10, c))
            ++counter;
        if (isAlive(r, c + 10))
            ++counter;
        if (isAlive(r + 10, c + 10))
            ++counter;
        if (isAlive(r - 10, c))
            ++counter;
        if (isAlive(r, c - 10))
            ++counter;
        if (isAlive(r - 10, c - 10))
            ++counter;
        if (isAlive(r + 10, c - 10))
            ++counter;
        if (isAlive(r - 10, c + 10))
            ++counter;

        return counter;
    }
    
    /**
     * Finds the lower bound of height.
     * @return Lowest of height.
     */
    private int minRow() {
        int temp = 99000;

        for(int i = 0; i < 10000; ++i)
        	if(cells[i].toggle)
	            if(cells[i].getX() < temp)
	                temp = cells[i].getX();

        return temp;
    }

    /**
     * Finds the lower bound of width.
     * @return Lowest of width.
     */
    private int minCol() {
        int temp = 99000;

        for(int i = 0; i < 10000; ++i)
        	if(cells[i].toggle)
        		if(cells[i].getY() < temp)
        			temp = cells[i].getY();

        return temp;
    }

    /**
     * Finds the higher bound of height.
     * @return Highest of height.
     */
    private int maxRow() {
        int temp = -99000;

        for(int i = 0; i < 10000; ++i)
        	if(cells[i].toggle)
	            if(cells[i].getX() > temp)
	                temp = cells[i].getX();

        return temp;
    }

    /**
     * Finds the higher bound of width.
     * @return Highest of width.
     */
    private int maxCol() {
        int temp = -99000;

        for(int i = 0; i < 10000; ++i)
        	if(cells[i].toggle)
	            if(cells[i].getY() > temp)
	                temp = cells[i].getY();

        return temp;
    }
    
	@Override
	public void mouseClicked(MouseEvent e) {
		// TODO Auto-generated method stub
		if(e.getComponent() instanceof JLabel){
			JLabel tmp = new JLabel();
			tmp = (JLabel)e.getComponent();
			
			int where = search(tmp.getX()+10, tmp.getY());
			if(where != -99999){
				if(cells[where].toggle)
					cells[where].toggle = false;
				else
					cells[where].toggle = true;
				
			update_table();
			}
		}		
	}
	@Override
	public void mouseEntered(MouseEvent e) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void mouseExited(MouseEvent e) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void mousePressed(MouseEvent e) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void mouseReleased(MouseEvent e) {
		// TODO Auto-generated method stub
		
	}
}
