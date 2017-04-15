package com.eytireyup.gol;

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.List;
import java.util.StringTokenizer;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.DialogInterface;
import android.graphics.Color;
import android.media.MediaPlayer;
import android.os.Bundle;
import android.os.Handler;
import android.text.Editable;
import android.view.Menu;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.LinearLayout.LayoutParams;
import android.widget.ScrollView;
import android.widget.TextView;
import android.widget.Toast;

public class MainGame  extends Activity {
	
	// set sizes of table and data-keeping file.
	private final int HEIGHT = 22;
	private final int WIDTH = 22;
	private final String scoreFileName = "scores";
	
	// initialize default language
	private AllStrings strings = new AllStrings();
	
	// that handler and runnable objects for simulate the game via Play>> button.
	Handler timerHandler = new Handler();
	Runnable timerRunnable;
	
	// streams that will be used to keep data
	private FileOutputStream  scoreOutStream;
	private FileInputStream scoreInStream;
	
	// the table.
	private Cell[][] table = new Cell[WIDTH][HEIGHT];
	
	private boolean language = true;		// true means English, false means Turkish.
	private boolean cont = false;			// the Play>> button clicked.
	private boolean scored = false;			// user saved a score before.
	private int generation = 0;				// num of generation in a game.
	private int previousNumOfAlives = -1;	// to check game is over or not.
	private int repeating = 0;				// to check game is over or not.
	private List<String> scoreNameList = new ArrayList<String>();	// keep scorers name.
	private List<Integer> scoreNumList = new ArrayList<Integer>();	// keep scores.
		
	private LinearLayout lay;	// main layout that keeps everything.
	private LinearLayout btnLay;// keeps button inside it vertical.
	private GameView game;		// the drawable game.
	private Button playX;		// plays a step.
	private Button playXX;		// plays simultaneously.
	private Button stop;		// stops the simulate.
	private Button save;		// saves current game.
	private Button load;		// loads a game from file.
	private Button reset;		// resets table.
	
	private MediaPlayer mplayer;
	
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        
        mplayer = MediaPlayer.create(getApplicationContext(), R.raw.msc);
        mplayer.start();
        mplayer.setScreenOnWhilePlaying(true);
        mplayer.setLooping(true);
        
        //selectLanguage();
        
        readScores();	// read score if it exist.  	
        
        // initialize game and set its table to this table.
        game = new GameView(this);
        for(int i = 0; i < WIDTH; ++i)
        	for(int j = 0; j < HEIGHT; ++j)
        		table[i][j] = new Cell();
        	        
        game.setTable(table);
        
        // create two LayoutParams. one for main layout, other for buttons.
        LinearLayout.LayoutParams dimension = new LinearLayout.LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT);
        LinearLayout.LayoutParams dimension2 = new LinearLayout.LayoutParams(LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT);
        
        // initialize main layout.
        lay = new LinearLayout(this);
        lay.setLayoutParams(dimension);        
        lay.setOrientation(LinearLayout.HORIZONTAL);
        // initialize button's layout.
        btnLay = new LinearLayout(this);
        btnLay.setLayoutParams(dimension2);
        btnLay.setOrientation(LinearLayout.VERTICAL);

        initializeButtons();
              
        lay.addView(btnLay); 	// add button's layout to main layout.      
        lay.addView(game);		// add the drawable game table to main layout.
        
        game.setOnTouchListener(new View.OnTouchListener() {			
        	@Override
			public boolean onTouch(View v, MotionEvent event) {
        		// calculate which cell has been clicked. and make it opposite of it.
        		int col = (int) Math.min(event.getX() / (game.getWidth() / WIDTH), table.length - 1);
				int row = (int) Math.min(event.getY() / (game.getHeight() / HEIGHT), table.length - 1);
				table[col][row].alive = !table[col][row].alive;
				
				game.invalidate();				
				return false;
			}
		});
        // set main layout to screen.
        setContentView(lay);
    }
    
    @Override
	protected void onDestroy() {
		super.onDestroy();
		mplayer.release();
		mplayer = null;
		
		// arrange data before save it.
		arrangeLists();
		try{// open score file to save data.
			scoreOutStream = openFileOutput(scoreFileName, MODE_PRIVATE);
			
			// write all data.
			for(int i = 0; i < scoreNumList.size(); ++i)		
				scoreOutStream.write((scoreNameList.get(i) + " " + scoreNumList.get(i) + "\n").getBytes());				
			
			scoreOutStream.close();
		}catch(Exception e){
			e.printStackTrace();
		}	
		
	}

    @Override
	protected void onPause() {
		super.onPause();
		
		// arrange data before save them to file.
		arrangeLists();
		
		try{	// open score file and write all data in it.
			scoreOutStream = openFileOutput(scoreFileName, MODE_PRIVATE);		
			for(int i = 0; i < scoreNumList.size(); ++i)		
				scoreOutStream.write((scoreNameList.get(i) + " " + scoreNumList.get(i) + "\n").getBytes());				
			
			scoreOutStream.close();
		}catch(Exception e){
			e.printStackTrace();
		}	
	}
	
    @Override	
	protected void onResume() {
		super.onResume();
		// read score file and arrange data.
		readScores();
		arrangeLists();
	}
    
    @Override
	protected void onStop() {
		super.onStop();
		// arrange data before save it.
		arrangeLists();
		try{// open score file to save data.
			scoreOutStream = openFileOutput(scoreFileName, MODE_PRIVATE);
			
			// write all data.
			for(int i = 0; i < scoreNumList.size(); ++i)		
				scoreOutStream.write((scoreNameList.get(i) + " " + scoreNumList.get(i) + "\n").getBytes());				
			
			scoreOutStream.close();
		}catch(Exception e){
			e.printStackTrace();
		}	
	}
    
    @Override
	protected void onRestart() {
		super.onRestart();
		// read score data and arrange.
		readScores();
		arrangeLists();
	}
    
    @Override
	public boolean onPrepareOptionsMenu(Menu menu) {
    	// change the language of options menu each time user presses Menu button
    	if(language){
			menu.getItem(0).setTitle("High Scores");
			menu.getItem(1).setTitle("Stop/Play Song");
			menu.getItem(2).setTitle("Select Language");
			menu.getItem(3).setTitle("Info");
    	}
    	else{
    		menu.getItem(0).setTitle("Yüksek Skorlar");
    		menu.getItem(1).setTitle("Müziði Durdur/Çal");
    		menu.getItem(2).setTitle("Dil seç");
    		menu.getItem(3).setTitle("Bilgi");
    	}
    	
		return super.onPrepareOptionsMenu(menu);
	}

	@Override   
	public boolean onCreateOptionsMenu(Menu menu)
    {  	// add items to options menu
		menu.add(Menu.NONE, Menu.FIRST, Menu.NONE, "High Scores");
    	menu.add(Menu.NONE, Menu.FIRST + 1, Menu.NONE, "Stop/Play Song");
    	menu.add(Menu.NONE, Menu.FIRST + 2, Menu.NONE, "Select Language");
    	menu.add(Menu.NONE, Menu.FIRST + 3, Menu.NONE, "Info");

        return (super.onCreateOptionsMenu(menu));
    }
    
	@Override
    public boolean onOptionsItemSelected(MenuItem item)
	{
		// create a LinearLayout and set its LayoutParams.
		LinearLayout l = new LinearLayout(getApplicationContext());			
		LinearLayout.LayoutParams dimension = new LinearLayout.LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT);		
		l.setLayoutParams(dimension);
		item.setVisible(true);
		
		switch (item.getItemId())
		{
		case Menu.FIRST:	// 'High Scores'	selected.
			// create an ArrayList to keep data together.
			List<String> values = new ArrayList<String>();	
			
			// arrange list and fill the values.
			arrangeLists();
			int k = 1;
			for(int i = 0; i < scoreNameList.size(); ++i)			
				values.add(" " + (k++) + ") " + scoreNameList.get(i) + " - " + scoreNumList.get(i) + "\n");
							
			// create a Dialog and a TextView.
			Dialog d1 = new Dialog(MainGame.this);
			
			TextView txt = new TextView(getApplicationContext());			
			txt.setTextColor(Color.BLACK);
			d1.setTitle(strings.highScores);
			d1.setCancelable(true);
			
			for(int i = 0; i < values.size(); ++i)	// add data to TextView
				txt.append(values.get(i));
			
			// add TextView to layout.
			l.addView(txt);
			// add layout to Dialog and show the Dialog
			d1.addContentView(l, dimension);
			d1.show();
			break;

        case Menu.FIRST + 1:	// sound
        	if(mplayer.isPlaying())
        		mplayer.pause();
        	
        	else
        		mplayer.start();

        	break;	
		
        case Menu.FIRST + 2:	// lang
        	selectLanguage();
        	break;
        
        case Menu.FIRST + 3:	// info
        	// create a Dialog, a scrollable view and a TextView.
        	Dialog d2 = new Dialog(MainGame.this);
        	ScrollView sV = new ScrollView(MainGame.this);
        
        	TextView aboutMe = new TextView(getApplicationContext());       	
        	aboutMe.setText(strings.about);
        	d2.setTitle("Game Of Life");
        	d2.setCancelable(true);
        	aboutMe.setTextColor(Color.BLUE);
        	
        	// add TextView to scrollable view
        	sV.addView(aboutMe);
        	
        	// add scrollable view to Dialog and show it.     	
        	d2.addContentView(sV, dimension);
        	d2.show();
        	break;    	

		}
        return (super.onOptionsItemSelected(item));
    }
    
	private int numOfAlives(){
    	int counter = 0;
    	for(int i = 0; i < table.length; ++i)
    		for(int j = 0; j < table.length; ++j){
    			if(table[i][j].alive)
    				++counter;
    		}
    	return counter;
    }
    
    private boolean notEnd(){
    	// if there is an alive cell in table, return true; otherwise return false.
    	for(int i = 0; i < table.length; ++i)
    		for(int j = 0; j < table.length; ++j)
    			if(table[i][j].alive)
    				return true;
    	
    	return false;
    }
    
    public void play() {

    	if(!notEnd()){	//if all cells are dead, remove Handler's duties and inform user.
    		cont = false;
    		Toast.makeText(getApplicationContext(), strings.cellsAreDead + generation, Toast.LENGTH_SHORT).show();
    		
    		if(generation > 10 && (!scored)){	// if this is the first time that user scores grater than 10, ask for save that.
    			askForSave();
    			scored = true;
    		}
   		
    		else if(generation > 10 && highScore())	// if this score is high, ask for save.
    			askForSave();   		    		
    	}   	
    	else if(previousNumOfAlives == numOfAlives()){	// if there is no change on alive alive cells,
    		repeating ++;	// increase the repeating flag.
    		if(repeating == 3){	// if flag is 3, assume that the game is over.
    			generation -= repeating - 1; // decrease the generation with repeated steps.
        		repeating = 0;	// set zero the flag to use other game.  			
        		cont = false;	// remove Handler's duties.
            	// inform user.
            	Toast.makeText(getApplicationContext(), strings.noMoreChanges + generation, Toast.LENGTH_SHORT).show();
            		
            	if(generation > 10 && (!scored)){// if this is the first time that user scores grater than 10, ask for save that.
            		askForSave();
            		scored = true;
            	}
           		
           		else if(generation > 10 && highScore()) // if this score is high, ask for save.
           			askForSave();
    		}    		
    		else{
    			++generation;
    			previousNumOfAlives = numOfAlives();
    			makeStep();
    		}
    			
    	}
   	
    	else{
    		++generation;
    		previousNumOfAlives = numOfAlives();
    		makeStep();
    	} 	   	
    }
     
    private boolean highScore(){
    	// if score is grater than previous minimum score, return true.
    	if(scoreNumList.get(scoreNumList.size() - 1) < generation)
    		return true;
    	
    	return true;
    }
        
    private void arrangeLists(){
    	// bubble-sort algorithm for arranging.
    	for(int i = 0; i < scoreNumList.size() - 1; ++i)
    		for(int j = 0; j < scoreNumList.size() - 1; ++j){
    			if(scoreNumList.get(j + 1) > scoreNumList.get(j)){
    				String tempS = scoreNameList.get(j);
    				scoreNameList.set(j, scoreNameList.get(j + 1));
    				scoreNameList.set(j + 1, tempS);
    				
    				Integer tempI = scoreNumList.get(j);
    				scoreNumList.set(j, scoreNumList.get(j + 1));
    				scoreNumList.set(j + 1, tempI);
    			}   				
    		}
    	// check and remove the exact same data.
    	for(int i = 0; i < scoreNumList.size() - 1; ++i){
    		if(scoreNameList.get(i).equals((scoreNameList).get(i + 1)))
    			if(scoreNumList.get(i).equals(scoreNumList.get(i + 1))){
    				scoreNameList.remove(i + 1);
    				scoreNumList.remove(i + 1);
    			} 			
    	}
    	// make sure there is only 10 entry is being saved.
    	if(scoreNumList.size() > 10){
    		for(int i = 10; i < scoreNumList.size(); ++i){
    			scoreNumList.remove(i);
    			scoreNameList.remove(i);
    		}
    	}
    }
       
    private void readScores(){
    	try {// open score file and create StreamReader with BufferedReader to read bytes from file.
    		scoreInStream = openFileInput(scoreFileName);
			InputStreamReader iSR = new InputStreamReader(scoreInStream);
			BufferedReader bR = new BufferedReader(iSR);
			
			String temp = null;						
			while((temp = bR.readLine()) != null){// while file is not end, take a line, split it and place each piece.
				StringTokenizer tokens = new StringTokenizer(temp, " ");
				scoreNameList.add(tokens.nextToken());
				scoreNumList.add(Integer.parseInt(tokens.nextToken()));
			}
			scoreInStream.close();
			
		} catch (Exception e) {
			e.printStackTrace();
		}  
    }
      
    private void askForSave(){    	
    	// create a Dialog and a EditText
		AlertDialog.Builder alert = new AlertDialog.Builder(MainGame.this);
		alert.setTitle(strings.highScore + generation);
		alert.setMessage(strings.wannaSave);
		alert.setCancelable(false);

		final EditText input = new EditText(getApplicationContext());
		input.setTextColor(Color.BLACK);
		alert.setView(input);

		// initialize the options on Dialog and make the jobs.
		alert.setPositiveButton("Ok", new DialogInterface.OnClickListener() {
			public void onClick(DialogInterface dialog, int whichButton) {
				Editable text = input.getText();
			
		        try {
		        	scoreNameList.add(text.toString());
		        	scoreNumList.add(generation);
		        	arrangeLists();
		        	
		        	Toast.makeText(getApplicationContext(), strings.scoreSaved, Toast.LENGTH_SHORT).show();
		        	generation = 0;
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		});

		alert.setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
			public void onClick(DialogInterface dialog, int whichButton) {
				dialog.cancel();
			}
		}).show();		
    }
    
    private void selectLanguage(){
    	// create an dialog to show options user
    	AlertDialog.Builder alert = new AlertDialog.Builder(MainGame.this);
		alert.setTitle("Game Of Life");
		alert.setMessage("Select Language / Dil Seçin");
		alert.setCancelable(false);	// make sure user select something

		// initialize the options on Dialog and make the jobs.
		alert.setPositiveButton("English", new DialogInterface.OnClickListener() {
			public void onClick(DialogInterface dialog, int whichButton) {
				if(!language){
					setStringsToENG();
					// buttons texts' must set again here
					playX.setText(strings.step);
					playXX.setText(strings.simulation);
					stop.setText(strings.stop);
					save.setText(strings.save);
					load.setText(strings.load);
					reset.setText(strings.reset);
					language = true;
				}			
			}
		});

		alert.setNegativeButton("Türkçe", new DialogInterface.OnClickListener() {
			public void onClick(DialogInterface dialog, int whichButton) {
				if(language){
					setStringsToTR();
					// buttons texts' must set again here
					playX.setText(strings.step);
					playXX.setText(strings.simulation);
					stop.setText(strings.stop);
					save.setText(strings.save);
					load.setText(strings.load);
					reset.setText(strings.reset);
					language = false;
				}				
			}
		}).show();		
    }
    
    private void setStringsToTR(){
    	strings.highScore = "Yüksek Skor";
    	strings.highScores = "Yüksek Skorlar";
    	strings.aboutMenu = "Bilgi";
    	strings.about = "Bu bir sýfýr-oyunculu oyun, yani oyunun gidiþatý onun baþlangýç durumuna göre belirlenir, daha fazla girdi istemez. Ana fikri bir baþlangýç ayarý çizip bunun evrimini incelemektir.\nOyunda her bir adýmda þu kurallar gerçekleþtirilir:\n1. Bir canlý hücrenin ikiden az canlý komþusu varsa yalnýzlýktan ölür.\n2. Bir canlý hücrenin iki veya üç tane canlý komþusu varsa diðer jenerasyonda da yaþar.\n3. Bir canlý hücrenin üçten fazla canlý komþusu varsa kalabalýktan dolayý ölür.\n4. Bir ölü hücrenin üç tane canlý komþusu varsa canlý hale geçer.\n\nVersiyon 1.2\n\nKodlama: Eyyüp Aydýn\n\nÞarký: Pierre Van Dormael - Undercover";
    	strings.cellsAreDead = "Tüm hücreler ölü.\nSkorun: ";
    	strings.noMoreChanges = "Oyun daha fazla ilerlemiyor.\nSkorun: ";
    	strings.wannaSave = "Skorunu kaydetmek ister misin? Ýsim veya lakap gir.";
    	strings.scoreSaved = "Skorun kaydedildi.";
    	strings.ok = "Tamam";
    	strings.cancel = "Ýptal";
    	strings.step = "Tek Adým";
    	strings.simulation = "Simulasyon";
    	strings.stop = "Dur";
    	strings.save = "Kaydet";
    	strings.saved = "Kaydedildi";
    	strings.load = "Yükle";
    	strings.nothing = "Oyun alaný boþ";
    	strings.cantSave = "Boþ bir oyunu kaydedemezsin.";
    	strings.enterFileName =  "Dosya ismi gir.";
    	strings.reset = "Sýfýrla";
    	
    	game.invalidate();
    }
    
    private void setStringsToENG(){
    	strings.highScore = "High Score";
    	strings.highScores = "High Scores";
    	strings.aboutMenu = "Info";
    	strings.about = "This is a zero-player game, meaning that its evolution is determined by its initial state, requiring no further input. One interacts with the Game of Life by creating an initial configuration and observing how it evolves or, for advanced players, by creating patterns with particular properties.\nAt each step in time, the following transitions occur:\n1. Any live cell with fewer than two live neighbours dies, as if caused by under-population.\n2. Any live cell with two or three live neighbours lives on to the next generation.\n3. Any live cell with more than three live neighbours dies, as if by overcrowding.\n4. Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.\n\nVersion 1.2\n\nImplemented by Eyyüp Aydýn\n\nSong: Pierre Van Dormael - Undercover";
    	strings.cellsAreDead = "All cells are dead.\nYour score: ";
    	strings.noMoreChanges = "No more changes in game.\nYour score: ";
    	strings.wannaSave = "Do you want to save your score? Enter your name/nick.";
    	strings.scoreSaved = "Your score saved.";
    	strings.ok = "Ok";
    	strings.cancel = "Cancel";
    	strings.step = "Step";
    	strings.simulation = "Simulate";
    	strings.stop = "Stop";
    	strings.save = "Save";
    	strings.saved = "Saved";
    	strings.load = "Load";
    	strings.nothing = "Nothing here";
    	strings.cantSave = "You can't save a blank game.";
    	strings.enterFileName =  "Enter file name";
    	strings.reset = "Reset";

    	game.invalidate();
    }
    
    private void makeStep(){
    	int neighbours = 0;
        Cell[][] temp = new Cell[table.length][table.length];
        
        // create a temporary table.
    	for(int i = 0; i < table.length; ++i)
    		for(int j = 0; j < table.length; ++j){
    			temp[i][j] = new Cell();
    		}
    			
        for(int i = 0; i < table.length ; ++i)
            for(int j = 0; j < table.length; ++j){
            	// find the num of neighbors of a cell.
               	neighbours = numOfNeighbours(i, j);
               	
               	// check rules
                if(table[i][j].alive){   	
                    if(neighbours == 2 || neighbours == 3)
                    	temp[i][j].alive = true;  	
                    }
                else{
                    if(neighbours == 3)
                    	temp[i][j].alive = true;                               	
                }
            }           	
            // set the temporary table to original table.
            table = temp;
            // make sure that GameView object is pointing the true table.
            game.setTable(temp);     	
    }
        
    private void initializeButtons(){
    	// create a LayoutParams object to set it btnLay.
    	LinearLayout.LayoutParams dimension = new LinearLayout.LayoutParams(LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT);
    	
    	// initialize the Play> Button and add it to layout
        playX = new Button(this);
        playX.setText(strings.step);
        playX.setLayoutParams(dimension);
        playX.setOnClickListener(new View.OnClickListener() {		
			@Override
			public void onClick(View v) {	// just play a single step and re-paint table.
				play();		
				game.invalidate();
			}
		});
        btnLay.addView(playX);
        
        // initialize the Play>> Button and add it to layout
        playXX = new Button(this);
        playXX.setText(strings.simulation);
        playXX.setLayoutParams(dimension);
        playXX.setOnClickListener(new View.OnClickListener() {		
			@Override
			public void onClick(View v) {
				if(!cont){	// if user clicks this button repeatedly, this listener will not create another
					cont = true;	// thread inside Runnable object by meaning of this.
									
					timerRunnable = new Runnable() {
						@Override
						public void run() {
							if(cont){	// if not clicked the Stop button yet.
								play();
							    game.invalidate();	// repaint table.
							    timerHandler.postDelayed(this, 300);	// call this again after 300 ms.
							}
							else{	// if user clicks Stop button, remove the duties of Handler's.
								timerHandler.removeCallbacks(timerRunnable);
							}
								
						 }
					 };
					 // call this Runnable object just after create it.
					 timerHandler.postDelayed(timerRunnable, 0);			 	
				}					
			}
		});
        btnLay.addView(playXX);
        
        // initialize the Stop Button and add it to layout
        stop = new Button(this);
        stop.setText(strings.stop);
        stop.setLayoutParams(dimension);
        stop.setOnClickListener(new View.OnClickListener() {
			
			@Override
			public void onClick(View v) {	// if user clicks the Stop, remove Handler's duties.
				timerHandler.removeCallbacks(timerRunnable);
				if(cont)	// if user clicked Stop after Start>>, end its mission.
					cont = false;	
			}
		});
        btnLay.addView(stop);
        
        // initialize the Save Button and add it to layout
        save = new Button(this);
        save.setText(strings.save);
        save.setLayoutParams(dimension);
        save.setOnClickListener(new View.OnClickListener() {
			
			@Override
			public void onClick(View v) {
				if(!notEnd()){	// if game is empty, don't save.
					AlertDialog.Builder alert = new AlertDialog.Builder(MainGame.this);
					alert.setTitle(strings.nothing);
					alert.setMessage(strings.cantSave);
				}
				else{
					// create a Dialog to say something to user
					AlertDialog.Builder alert = new AlertDialog.Builder(MainGame.this);
					alert.setMessage(strings.enterFileName);
					alert.setCancelable(false);

					// set an EditText view to get user input 
					final EditText input = new EditText(getApplicationContext());
					input.setTextColor(Color.BLACK);
					alert.setView(input);

					// show options and make the job
					alert.setPositiveButton(strings.ok, new DialogInterface.OnClickListener() {
						public void onClick(DialogInterface dialog, int whichButton) {
							Editable text = input.getText();
					
							try {
								writeFile(text.toString());
							} catch (IOException e) {
								e.printStackTrace();
							}
						}
					});

					alert.setNegativeButton(strings.cancel, new DialogInterface.OnClickListener() {
						public void onClick(DialogInterface dialog, int whichButton) {
							dialog.cancel();
						}
					}).show();
				}
							
			}
		});
        btnLay.addView(save);
        
        // initialize the Load Button and add it to layout
        load = new Button(this);
        load.setText(strings.load);
        load.setLayoutParams(dimension);
        load.setOnClickListener(new View.OnClickListener() {
			
			@Override
			public void onClick(View v) {
				// create a Dialog to say something to user
				AlertDialog.Builder alert = new AlertDialog.Builder(MainGame.this);
				alert.setMessage(strings.enterFileName);
				alert.setCancelable(false);

				// set an EditText view to get input from user
				final EditText input = new EditText(getApplicationContext());
				input.setTextColor(Color.BLACK);
				alert.setView(input);

				// show options and make the job
				alert.setPositiveButton(strings.ok, new DialogInterface.OnClickListener() {
					public void onClick(DialogInterface dialog, int whichButton) {
						Editable value = input.getText();
				
						readFile(value.toString());
					}
				});

				alert.setNegativeButton(strings.cancel, new DialogInterface.OnClickListener() {
					public void onClick(DialogInterface dialog, int whichButton) {
						dialog.cancel();
					}
				}).show();
			}
		});
        btnLay.addView(load);
        
        // initialize the Reset Button and add it to layout
        reset = new Button(this);
        reset.setText(strings.reset);
        reset.setLayoutParams(dimension);
        reset.setOnClickListener(new View.OnClickListener() {
			
			@Override
			public void onClick(View v) {	// reset the table and re-paint it
				resetTable();
				game.invalidate();
			}
		});
        btnLay.addView(reset);
    }
       
    public void writeFile(String name) throws IOException {      

        try {	// open the file
        	FileOutputStream outp = openFileOutput(name, MODE_PRIVATE);
			   
	        for (int i = 0; i < table.length; ++i){
	        	for (int j = 0; j < table.length; ++j){	// write alive cells' coordinates to file as byte.  
	                if (table[i][j].alive){
	                	outp.write((i + " " + j + "\n").getBytes());
	                }	                       
	            }
	        }
	        // close the file and inform the user that there is no error.  
	        outp.close();
			Toast.makeText(getApplicationContext(), strings.saved, Toast.LENGTH_SHORT).show();
	        
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		}
    }
       
    public void readFile(String name){
    	// first, reset the table to show new game.
    	resetTable();
    	
    	try {	// create file, reader and bufferedReader to get bytes from file.
			FileInputStream inp = openFileInput(name);
			InputStreamReader iSR = new InputStreamReader(inp);
			BufferedReader bR = new BufferedReader(iSR);
			
			String temp = null;
						
			while((temp = bR.readLine()) != null){	// while file is not end, take a line, split it and make that coordinate alive.
				StringTokenizer tokens = new StringTokenizer(temp, " ");
				int i  = Integer.parseInt(tokens.nextToken());
				int j  = Integer.parseInt(tokens.nextToken());
				table[i][j].alive = true;
			}
			// re-paint the table.
			game.invalidate();
			
		} catch (Exception e) {
			e.printStackTrace();
		}   		
    }
       
    private void resetTable(){
    	// make all cells dead and set generation to zero.
    	for(int i = 0; i < table.length; ++i)
    		for(int j = 0; j < table.length; ++j){
    			table[i][j].alive = false;
    		}
    	
    	generation = 0;
    }
       
    private int numOfNeighbours(int r, int c) {
        int counter = 0;

        // check out all of neighborhood of a cell to calculate number of neighbors.
        if (r < HEIGHT - 1 && table[r + 1][c].alive)
            ++counter;
        if (c < WIDTH - 1 && table[r][c + 1].alive)
            ++counter;
        if (r < HEIGHT - 1 && c < WIDTH - 1 && table[r + 1][c + 1].alive)
            ++counter;
        if (r > 0 && table[r - 1][c].alive)
            ++counter;
        if (c > 0 && table[r][c - 1].alive)
            ++counter;
        if (r > 0 && c > 0 && table[r - 1][c - 1].alive)
            ++counter;
        if (r < HEIGHT - 1 && c > 0 && table[r + 1][c - 1].alive)
            ++counter;
        if (r > 0 && c < WIDTH - 1 && table[r - 1][c + 1].alive)
            ++counter;

        return counter;
    }
	
}