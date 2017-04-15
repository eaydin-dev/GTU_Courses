/**
 * The class Cell.
 * Describes a cell's coordinates.
 *
 * @author Eyyup Aydin
 * @since 2.1.2015
 */


public class Cell{
    /**
     * Height value of cell.
     */
    private int x;

    /**
     * Width value of cell.
     */
    private int y;

    /**
     * Shows that the cell is alive or dead.
     */
    public boolean toggle = false;  
    
    /**
     * No parameter constructor. Generates a (0,0) Cell.
     */
    public Cell() {
        this(0, 0);
    }

    /**
     * Two parameter constructor.
     * @param v_x: Cell's height value.
     * @param v_y: Cell's width value.
     */
    public Cell(int v_x, int v_y) {
        x = v_x;
        y = v_y;
    }

    /**
     * @return Cell's height value.
     */
    public int getX(){return x;}

    /**
     * @return Cell's width value.
     */
    public int getY(){return y;}

    /**
     * Set height of cell.
     * @param v_x: value of height.
     */
    public void setX(int v_x){x = v_x;}

    /**
     * Set width of cell.
     * @param v_y: value of width.
     */
    public void setY(int v_y){y = v_y;}

    /**
     * Set both height and width of cell.
     * @param v_x: value of height.
     * @param v_y: value of width.
     */
    public void setXY(int v_x, int v_y) {
        x = v_x;
        y = v_y;
    }
    
    /**
     * If a cell is alive, its string "*"; if not "-".
     */
    public String toString(){
    	if(toggle)
    		return "*";
    	
    	return "-";
    }
}
