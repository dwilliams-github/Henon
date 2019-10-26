/* | ++ | | Plot 'Henon Attractor' chaos drawing | | (c) Copyright 1992 David C. Williams | | --*/#include <stdlib.h>#include <string.h>#include <stdio.h>#include <math.h>#include <MacHeaders>#include "hendef.h"#define		MAIN_WIND	128#define		ABOUT_WIND	129#define		INFO_STRS	128#define		APPLE_MENU	128#define		APPLE_ABOUT	1#define		FILE_MENU	129#define		FILE_QUIT	1#define		EDIT_MENU	130#define		OPTION_MENU		131#define	    OPTION_GO		1#define		OPTION_SCALE	2#define		OPTION_COORD	3#define		OPTION_PARAM	4#define		OPTION_NUMBER	5#define		STRING_DRAW		128#define		STRING_CONT		129#define		DEF_X_START		0.85675893452#define		DEF_Y_START		-.451231123412            MenuHandle 		menus[4];CursHandle		watchCurs,				crossCurs;WindowRecord	wStorage;				/* Storage for window record */PicHandle		saveHenon;HenonViewType	henonView;main(){		int		notEnghMem; 	/* Allocate memory for the screen save */		MaxApplZone();		notEnghMem = !(henonView.recall = NewHandle( MAX_RECALL*4 ));	henonView.points = 0;			/* Initialize all the system stuff */	InitGraf( &thePort );	InitFonts();	InitWindows();	InitMenus();	TEInit();	InitDialogs( 0L );		if (notEnghMem) NotEnoughMem();		/* Get handle to watch and cross cursors */		watchCurs = GetCursor( watchCursor );	crossCurs = GetCursor( crossCursor );			/* Make our menus */	MakeMenus();								/* Make our window */	MakeHenonWindow( &henonView );	/* Initialize henonView structure */		InitHenonView( &henonView );		/* Wait for event */	EventLoop( &henonView ); 		/* Finish up */			CloseWindow( henonView.window );	DisposHandle( henonView.recall );	ExitToShell();}NotEnoughMem(){	InitCursor();	StopAlert( NOTENOUGHMEM_ALERT, 0 );	ExitToShell();}MakeHenonWindow( henonView )	HenonViewType	*henonView;{	GrafPtr		wPort;	int			height;		/* Get screen size from window manager port */		GetWMgrPort( &wPort );		henonView->windowWidth = wPort->portRect.right - wPort->portRect.left;	height = wPort->portRect.bottom - wPort->portRect.top;		if (height<henonView->windowWidth) henonView->windowWidth = height;		henonView->windowWidth -= 46;		/* Get the main window from the resource */	henonView->window = GetNewWindow( MAIN_WIND, &wStorage, (WindowPtr)(-1) );		/* Modify its position and size according to the screen size */		SizeWindow( henonView->window, henonView->windowWidth,	            henonView->windowWidth, 0 ); 	MoveWindow( henonView->window, 4, 40, 0 );	/* Make it the active window and make it visible */		SelectWindow( henonView->window );	ShowWindow( henonView->window );		return;}	InitHenonView( henonView )	HenonViewType	*henonView;{	static ScaleType	defScale = DEFAULT_SCALE;	henonView->parm1 = DEF_PARM1;	henonView->parm2 = DEF_PARM2;	henonView->parm3 = DEF_PARM3;	henonView->xstart = DEF_X_START;	henonView->ystart = DEF_Y_START;    henonView->currScale = 0;    henonView->validNewScale = 0;	henonView->scales[0] = defScale;	return;}MakeMenus(){	int				menu;			/* Make Apple menu, add standard DA menu items */		menus[0] = GetMenu( APPLE_MENU );	AddResMenu( menus[0], 'DRVR' );		/* Add our file menu */		menus[1] = GetMenu( FILE_MENU );		/* Add the default edit menu */		menus[2] = GetMenu( EDIT_MENU );		/* Add the "Options" menu */		menus[3] = GetMenu( OPTION_MENU );		/* Insert the menus in order onto the menu list */		for (menu=0;menu<4;menu++) InsertMenu( menus[menu], 0 );		/* Draw the menus */		DrawMenuBar();		return;}void	FindNewScale( newScale, currScale, pixWidth, selRect )	ScaleType	*newScale,				*currScale;	int			pixWidth;	SelRect		*selRect;/* | Find new scale parameter from selected square region. | Parameters: |		newScale	- (out) New scale |		currScale	- (in) Current scale |		pixWidth	- (in) Width, in pixels, of main window |		selRect		- (in) Selection rectangle*/{	int		pointv = selRect->localWhere.v,			pointh = selRect->localWhere.h,			size   = selRect->size;	double	width  = (float)pixWidth;; 	newScale->bot  = (currScale->size/(2.0*width))*(width  - pointv - size) 				   + currScale->bot; 	newScale->left = (currScale->size/width)*((double) pointh) + currScale->left; 	newScale->size = currScale->size*((double)size)/width;}EventLoop( henonView )	HenonViewType	*henonView;{	EventRecord		doneEvent;	WindowPtr 		whichWindow;	int 			whereCode; 	Rect     		moveBound,					newChar 	= {35,40,60,70};	GrafPtr			wPort;	ScaleType		*currScale;	SelRect			*selRect = &henonView->selRect;	int				*selValid = &henonView->validNewScale;							FlushEvents( everyEvent, 0 );		/* Calculate the move bounds for the window */		GetWMgrPort( &wPort );	moveBound.left = wPort->portRect.left;	moveBound.top = wPort->portRect.top + 16;	moveBound.right = wPort->portRect.right - 4;	moveBound.bottom = wPort->portRect.bottom - 4; 		/* Initialize henonView */		henonView->x = henonView->xstart;	henonView->y = henonView->ystart;	henonView->points = 0;		/* Wait for "goaway" event */		for(;;) {			/* Change the mouse to a pointer */				InitCursor();				/* Wait for next update or mouse down event */				while( !GetNextEvent( everyEvent, &doneEvent )) SystemTask();				switch(doneEvent.what) {			case mouseDown:								/* Mouse was pressed - find out where */								whereCode = FindWindow( doneEvent.where, &whichWindow );				switch (whereCode) {					case inGoAway:						TrackGoAway( whichWindow, doneEvent.where );						return;				 		break;				 	case inSysWindow:				 		SystemClick( &doneEvent, whichWindow );				 		break;				 	case inDrag:				 		DragWindow( whichWindow, doneEvent.where, &moveBound );				 		break;				 	case inContent:				 					 	    /* New scale? Do we have space */				 	    				 	    currScale = henonView->scales + henonView->currScale;				 	    if (currScale >= henonView->scales+MAX_SCALES-1) {				 	    	Alert( RESCALELIMIT_ALERT, 0 );				 	    	break;				 	    }				 	    				 					 		/* Draw scale box */				 					 		NewSquare( henonView->window, &doneEvent.where, 				 		           selRect, selValid );				 						 		if (*selValid) {				 						 			/* Convert to Henon coordinates */				 						 			FindNewScale( currScale+1, currScale, 				 						  henonView->windowWidth, selRect ); 				 		}			  				 		break;				 	case inMenuBar:				 		if (!DoMenu( doneEvent.where, henonView )) return;				 		break;				 }		 		break;		 	case updateEvt:				BeginUpdate( henonView->window );				RestoreHenon( henonView );				EndUpdate( henonView->window );					break;			default:;	 	}	}}int	DoMenu( where, henonView )	Point			where;	HenonViewType	*henonView;{	union {		short		byWord[2];		long		byLong;	} menuId;		Str255			appleName;	StringHandle	menuStr;		/* Set draw menu */		menuStr = GetString( henonView->points ? STRING_CONT : STRING_DRAW );	SetItem( menus[3], OPTION_GO, *menuStr );		if (henonView->points < MAX_RECALL)		EnableItem( menus[3], OPTION_GO );	else		DisableItem( menus[3], OPTION_GO );	/* Decide whether we are allowed to draw scaled */		if (henonView->validNewScale)		EnableItem( menus[3], OPTION_SCALE );	else		DisableItem( menus[3], OPTION_SCALE );		/* Get menu selection */		menuId.byLong = MenuSelect( where );		if (menuId.byWord[1]!=0) {		 	/* Valid menu selection */	 		 	switch(menuId.byWord[0]) {	 		 		case APPLE_MENU:					/* In Apple Menu: display 'about' info */	 			switch(menuId.byWord[1]) {	 				case APPLE_ABOUT:		 			 	ShowAbout(); 			 				break;		 			default:		 				GetItem( menus[0], menuId.byWord[1], appleName );		 				OpenDeskAcc( appleName );		 		}		 		break;			case FILE_MENU:					/* In File Menu */	 			switch(menuId.byWord[1]) {	 				case FILE_QUIT:	 					HiliteMenu(0);	 					return(FALSE);	 					break;	 			}	 			break;	 		case OPTION_MENU:	 			switch(menuId.byWord[1]) {	 				case OPTION_GO:	 					 					/* Make picture */ 	 	 					MakeHenon( henonView );	 					break;	 				case OPTION_SCALE:	 					 					/* Make it current */	 						 					henonView->currScale++;	 					henonView->validNewScale = 0;	 						 					/* Redraw picture */	 						 					ResetHenon( henonView );	 					MakeHenon( henonView );	 					break;	 				case OPTION_COORD:	 					 					/* Throw up coordinate dialog */	 						 					if (CoordDialog( henonView )) {	 							 					/* Erase picture */		 							 					ResetHenon( henonView );	 					}	 					break;	 				case OPTION_PARAM:	 					 					/* Throw up parameter dialog */	 						 					if (ParamDialog( henonView ) ) {	 							 					/* Apply new parameters */		 							 					henonView->points = 0;		 					henonView->x = henonView->xstart;		 					henonView->y = henonView->ystart;		 							 					/* Reset picture */		 							 					ResetHenon( henonView );	 					}	 					break;	 				case OPTION_NUMBER:	 					 					/* Throw up Number points dialog */	 						 					NumberDialog( henonView );	 					break;	 			}	 	}	}		HiliteMenu(0);	return(TRUE);}ShowAbout(){	WindowPtr		aboutWindow,					whichWindow;	EventRecord 	aboutEvent;	int				where;		Str255			infoLine;	int				line,					lineh[5] = { 15, 35, 45, 65, 77 };	/* Make info Window */		aboutWindow = GetNewWindow( ABOUT_WIND, 0L, (WindowPtr)(-1) );		SelectWindow( aboutWindow );	SetPort( aboutWindow );		TextSize( 9 );		for(line=0;line<5;line++) {		GetIndString( infoLine, INFO_STRS, line+1 );		MoveTo( 110-StringWidth( infoLine )/2, lineh[line] );		DrawString( infoLine );	}		/* Wait for a mouse down event inside the window */		for(;;) {		while( !GetNextEvent( mDownMask, &aboutEvent )) SystemTask();				where = FindWindow( aboutEvent.where, &whichWindow );		if ( (where==inContent) && (whichWindow==aboutWindow) ) break;				SysBeep( 30 );	}	DisposeWindow( aboutWindow );		return;}MakeHenon( henonView )	HenonViewType	*henonView;{	EventRecord		intptEvent;	double		scaleV,				scaleH,				xTemp,				top,				right,				width;	int			h, v,				inner;	long		*recallBank;	ScaleType   *currScale;		/* Make the mouse cursor a watch */		SetCursor( *watchCurs );		/* Set port to Henon window */	SetPort( henonView->window );		/* Get pointer to current scale */		currScale = henonView->scales + henonView->currScale;		/* Freeze our screen save segment */		HLock( henonView->recall );	/* Initialize recall bank pointer */		recallBank = (long *)*henonView->recall;	recallBank +=  henonView->points;		/* Draw until a mouse down event is registered */	width = henonView->windowWidth;		scaleV = 2.0*width/currScale->size;	scaleH = width/currScale->size;	top = currScale->bot + currScale->size/2.0;	right = currScale->left + currScale->size;		while ( !GetNextEvent( mDownMask, &intptEvent ) ) {		SystemTask();				/* Perform a quicker inner loop to keep overhead down */				for (inner=0;inner<50 && henonView->points<MAX_RECALL;inner++) {					/* Find next number in sequence */					xTemp = henonView->x;			henonView->x = henonView->y + henonView->parm1 			               - henonView->parm2*xTemp*xTemp;			henonView->y = henonView->parm3*xTemp;						/* Is this point in our window ? */						if (henonView->x < currScale->left) continue;			if (henonView->x > right ) continue;			if (henonView->y < currScale->bot ) continue;			if (henonView->y > top ) continue;						/* Plot it */						h = (henonView->x - currScale->left)*scaleH;			v = henonView->windowWidth 			    - (henonView->y - currScale->bot )*scaleV;			MoveTo( h, v );			Line( 0, 0 );						/* Save this plotted value */						henonView->points++;			*recallBank++ = (long)h + ((long)v<<16);		}		if (henonView->points>=MAX_RECALL) break;	}		/* Free up our screen save segment */		HUnlock( henonView->recall );			/* Flush all preceding events */		FlushEvents( everyEvent, 0 );		return;}RestoreHenon( henonView )	HenonViewType	*henonView;{	int			*recallBank,				*oneRecall;	int			depth,				h, v;	GrafPtr 	savePort;	PenState 	savePen;		/* Make the mouse cursor a watch */		SetCursor( *watchCurs );	/* Save port and pen */		GetPort( &savePort );	GetPenState( &savePen );	/* Set port to Henon window and set default pen */	SetPort( henonView->window );	PenNormal();	/* Draw each old point in turn */		recallBank=(int *)*henonView->recall;		oneRecall = recallBank + (henonView->points<<1) - 1;	while (oneRecall > recallBank) {	    h = *oneRecall--;	    v = *oneRecall--;	    	    MoveTo( h, v );	    Line( 0, 0 );	}		/* Draw select box, if it exists */		if (henonView->validNewScale) {		SelRect	*selRect = &henonView->selRect;		Rect	rect;			PenMode( patXor );		PenPat( gray );				SetRect( &rect, selRect->localWhere.h, 						selRect->localWhere.v, 						selRect->localWhere.h + selRect->size,						selRect->localWhere.v + selRect->size );		FrameRect( &rect );	}	/* Return port and pen */		SetPenState( &savePen );	SetPort( savePort );	return;}	ResetHenon( henonView )	HenonViewType	*henonView;{	EventRecord		updateEvent; 	/* Nullify any new update event */ 						 	GetNextEvent( updateMask, &updateEvent ); 					 	/* Erase the window, reset point number, and invalidate selection rectangle */ 						henonView->points = 0;	henonView->validNewScale = 0;	EraseRect( &henonView->window->portRect ); }NewSquare( port, corner, selRect, selValid )	GrafPtr		port;	Point		*corner;	SelRect		*selRect;	int			*selValid;/* | Draw selection square in window "window" starting at "where". | "newRect" is the selected square.  Modifies the value of where. | Parameters: |		port	 - (in) Graph port |		corner	 - (in) Coordinates of mouse down event, in global coordinates. |                      These coordinates define one corner of the box. |      selRect	 - (out) Selected rectangle, in local coordinates. |		selValid - (in/out) Non-zero if selection rectangle is valid*/{	Point	*where = &selRect->localWhere;	int		*size  = &selRect->size;	Point	deltaPoint;	int		size2;	Rect	newRect;	EventRecord	leftEvent;	GrafPtr		savePort;	PenState	penState;		/* Get port and pen state (to be returned at end) */		GetPort( &savePort );	GetPenState( &penState );		/* Set port */		SetPort( port );		/* Set up existing rectangle */		if (*selValid) 		SetRect( &newRect, where->h, where->v, where->h + *size, where->v + *size );		/* Convert mouse down coordinates to local coordinates */		*where = *corner;	GlobalToLocal( where );		/* Set pen mode to Xor and pattern to gray */		PenMode( patXor );	PenPat( gray );		/* Set cursor to cross */		SetCursor( *crossCurs );	do {		/* Clear last rectangle, if it exists */				if (*selValid) FrameRect( &newRect );					/* where is the mouse ? */		GetMouse( &deltaPoint );							deltaPoint.v -= where->v;		deltaPoint.h -= where->h;					/* Get larger of the vert, horz distances */		*size  = deltaPoint.h < 0 ? -deltaPoint.h : deltaPoint.h;		size2 = deltaPoint.v < 0 ? -deltaPoint.v : deltaPoint.v;		if (*size<size2) *size = size2;				/* Anything there? */				*selValid = (*size > 0);		if (!(*selValid)) continue;				/* Form a rectangle */		if (deltaPoint.h>0) {		 	newRect.left = where->h;		 	newRect.right = newRect.left + *size;		}		else {		 	newRect.right = where->h;		 	newRect.left = newRect.right - *size;		}		 		if (deltaPoint.v>0) {		 	newRect.top = where->v;		 	newRect.bottom = newRect.top + *size;		}		else {		 	newRect.bottom = where->v;		 	newRect.top = newRect.bottom - *size;		}		 		/* Draw the rectangle */				FrameRect( &newRect );	} while (StillDown());	/* Set 'where' to always point to top left of square */		if (*selValid) {		where->h = newRect.left;		where->v = newRect.top;	}		/* Get the leftover mouse up event */		GetNextEvent( mUpMask, &leftEvent );		/* Return port and pen */			SetPenState( &penState );	SetPort( savePort );	return;}