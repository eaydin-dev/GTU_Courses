package com.eytireyup.gol;
// this is the class that draws everything on screen with Canvas.

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.view.View;

public class GameView extends View {
	
	private Cell[][] cellTable; // the table.

	public void setTable(Cell[][] newTable){
		cellTable = newTable;
	}
	
	public GameView(Context context) {
		super(context);
	}
			
	@Override
	protected void onDraw(Canvas canvas) {
		super.onDraw(canvas);
		// create Paints to use wile drawing.
		Paint pBox = new Paint(Paint.ANTI_ALIAS_FLAG);
		pBox.setColor(Color.BLUE);
		
		Paint pLine = new Paint(Paint.ANTI_ALIAS_FLAG);
		pLine.setColor(Color.GRAY);
		
		// get each box's width and height according to device via View.
		int boxW = getWidth() / cellTable.length;		
		int boxH = getHeight() / cellTable.length;
		
		for(int i = 0; i < cellTable.length; ++i)
			for(int j = 0; j < cellTable.length; ++j){
				if(cellTable[i][j].alive){	// fill the box if cell is alive.
					canvas.drawRect(i * boxW, j * boxH, i * boxW + boxW, j * boxH + boxH, pBox);
				}
			}
		
		// draw lines to draw boxes.
		for (int i = 0; i < cellTable.length + 1; i++) {
			canvas.drawLine(i * boxW, 0, i * boxW, getHeight() , pLine);
			canvas.drawLine(0, i * boxH, getWidth(), i * boxH, pLine);
		}
	}
}
