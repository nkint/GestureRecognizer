/*
 * gui.h
 *
 *  Created on: Feb 20, 2012
 *      Author: alberto
 */

#ifndef GUI_H_
#define GUI_H_

#include <iostream>

#define TRAINSET_LENGTH 18
#define HMMS_LENGTH 12

hPanel * testApp::panel1(hGui * gui, hEvents * events) {
	hPanel *mainPanel = gui->addPanel("mainPanel1", NULL, HGUI_ABSOLUTE_POSITION,
			gui->margin1,					// x
			gui->margin1*3, 				// y
			ofGetWidth()-gui->margin1*2,	// width
			ofGetHeight()-gui->margin1*5,	// height
			true);							// border
	mainPanel->setVisibleBackground(true);

	int panelSize1 = 400;

	// column 1 : drag gesture board
	hLabel * label_drawer = gui->addLabel("", mainPanel, HGUI_TOP_LEFT,
			gui->margin1,
			gui->margin1, "draw gesture board");

	GestureThrower * drawer = new GestureThrower("drawer", mainPanel, HGUI_BOTTOM,
			0,
			0, panelSize1, panelSize1);
	gui->addWidget(drawer);
	drawer->setMessage("new_gesture");
	events->addListener("new_gesture", this, &testApp::_event_new_gesture);

	hLabel * label_info = gui->addLabel("", mainPanel, HGUI_BOTTOM,
			0,
			gui->margin1, "infos");
	hTextArea * textArea2 =	gui->addTextArea("sequence", mainPanel, HGUI_BOTTOM,
			0,
			0, 600+gui->margin1*2, 140);
	textArea2->setVisibleBorder(false);
	ofColor c1(85,85,85);
	textArea2->setBackgroundColor(0x333333); //c1.getHex());
	textArea2->setSelectable(true);

	// column 2 : train coulumn
	hLabel * label_listbox = gui->addLabel("", mainPanel, HGUI_ABSOLUTE_POSITION,
			drawer->getRight()+gui->margin1*2,
			label_drawer->getY(), "train set");

	hListBox * list = gui->addListBox("listBox", mainPanel, HGUI_ABSOLUTE_POSITION,
			label_listbox->getX(),
			label_listbox->getBottom()-1, 200);
	list->addItems(TRAINSET_LENGTH, "");
	events->addListener("visualize_gesture", this, &testApp::_event_visualize_gesture);
	list->setMessage("visualize_gesture");

	hButton * button1 = gui->addButton("add_new", mainPanel, HGUI_ABSOLUTE_POSITION,
			list->getX(),
			list->getBottom()-1, 200, "add new");
	events->addListener("add_gesture", this, &testApp::_event_add_gesture);
	button1->setMessage("add_gesture");
	hButton * button2 = gui->addButton("clear", mainPanel, HGUI_ABSOLUTE_POSITION,
			button1->getX(),
			button1->getBottom()-1, 200, "clear");
	events->addListener("clear_trainset", this, &testApp::_event_clear_gestures);
	button2->setMessage("clear_trainset");

	// column 3 : train
	hTextArea * help_text =	gui->addTextArea("help_text", mainPanel, HGUI_ABSOLUTE_POSITION,
			list->getRight() + gui->margin1*2,
			list->getY(), 330, 150);
	stringstream ss;
	ss << "draw the gesture in the board" << endl << endl;
	ss << "press space for add in train set" << endl << endl;
	ss << "when you've done with a reasonable amount of train gesture, press enter to create an hidden markov model and train it with baum-welch algorithm." << endl;
	help_text->setText(ss.str());
	help_text->setBackgroundColor(0x333333);
	help_text->setVisibleBorder(false);

	hButton * button = gui->addButton("train", mainPanel, HGUI_ABSOLUTE_POSITION,
			list->getRight() + gui->margin1*2,
			label_listbox->getY() + 400, 200, "train!");
	events->addListener("train", this, &testApp::_event_train);
	button->setMessage("train");

	return mainPanel;
}


hPanel * testApp::panel2(hGui * gui, hEvents * events) {
	hPanel *mainPanel = gui->addPanel("mainPanel2", NULL, HGUI_ABSOLUTE_POSITION,
			gui->margin1,					// x
			gui->margin1*3, 				// y
			ofGetWidth()-gui->margin1*2,	// width
			ofGetHeight()-gui->margin1*5,	//height
			true);							// border
	mainPanel->setVisibleBackground(true);

	// column 1 : learned gestures
	hLabel * label_listbox = gui->addLabel("", mainPanel,  HGUI_TOP_LEFT,
			gui->margin1,
			gui->margin1, "learned gesture");
	hListBox * list = gui->addListBox("listBox_hmm", mainPanel, HGUI_ABSOLUTE_POSITION,
			label_listbox->getX(),
			label_listbox->getBottom(), 200);
	list->addItems(HMMS_LENGTH, "");
	events->addListener("visualize_hmm", this, &testApp::_event_visualize_hmm);
	list->setMessage("visualize_hmm");

	// column 2 : train info
	hLabel * label_hmm_name = gui->addLabel("", mainPanel, HGUI_NEXT_COL,
			gui->margin1,
			gui->margin1, "hmm name");

	hTextBox * textBox_hmm_name = gui->addTextBox("hmm_name", mainPanel,  HGUI_BOTTOM,
			0,
			0, 200, "editable textbox");
	textBox_hmm_name->setBackgroundColor(0x333333);
	textBox_hmm_name->setMessage("hmm_name");
	events->addListener("hmm_name", this, &testApp::_event_hmm_name);

	hLabel * label_drawer = gui->addLabel("", mainPanel, HGUI_ABSOLUTE_POSITION,
			textBox_hmm_name->getX(),
			textBox_hmm_name->getBottom() +gui->margin1, "draw gesture board");

	MultiGestureDrawer * drawer = new MultiGestureDrawer("drawer_hmm", mainPanel, HGUI_ABSOLUTE_POSITION,
			label_drawer->getX(),
			label_drawer->getBottom(), 200, 200);
	gui->addWidget(drawer);

	// column 1-2 : save and load.

	hButton * save = gui->addButton("save", mainPanel, HGUI_ABSOLUTE_POSITION,
			list->getX()+1,
			list->getBottom()+gui->margin1*2,
			400+gui->margin1, "save into file");
	save->setMessage("save");
	events->addListener("save", this, &testApp::_event_save);

	hButton * load = gui->addButton("load", mainPanel, HGUI_ABSOLUTE_POSITION,
			save->getX(),
			save->getBottom()+gui->margin1,
			400+gui->margin1, "load");
	load->setMessage("load");
	events->addListener("load", this, &testApp::_event_load);

	// column 3 : log probabilities (and errors? and plot?)

	hLabel * label_logliks = gui->addLabel("", mainPanel, HGUI_ABSOLUTE_POSITION,
			drawer->getRight()+gui->margin1,
			label_hmm_name->getY(), "hmm info");
	hTextArea * textAreaHMMinfo = gui->addTextArea("hmm_info", mainPanel, HGUI_ABSOLUTE_POSITION,
			label_logliks->getX(),
			label_logliks->getBottom()+gui->margin1, 550, 550);
	textAreaHMMinfo->setBackgroundColor(0x333333);
	textAreaHMMinfo->setVisibleBorder(false);

	return mainPanel;
}

hPanel * testApp::panel3(hGui * gui, hEvents * events) {
	hPanel *mainPanel = gui->addPanel("mainPanel2", NULL, HGUI_ABSOLUTE_POSITION,
			gui->margin1,					// x
			gui->margin1*3, 				// y
			ofGetWidth()-gui->margin1*2,	// width
			ofGetHeight()-gui->margin1*5,	//height
			true);							// border
	mainPanel->setVisibleBackground(true);

	hLabel * label_drawer = gui->addLabel("", mainPanel, HGUI_TOP_LEFT,
			gui->margin1,
			gui->margin1, "draw gesture board");

	GestureThrower * drawer = new GestureThrower("recognizer", mainPanel, HGUI_BOTTOM,
			0,
			0, 550, 550);
	gui->addWidget(drawer);
	drawer->setMessage("classify");
	events->addListener("classify", this, &testApp::_event_classify);

	hLabel * label_classify = gui->addLabel("", mainPanel, HGUI_ABSOLUTE_POSITION,
			drawer->getRight()+gui->margin1*2,
			label_drawer->getY(), "classification info");
	hTextArea * textArea_classify = gui->addTextArea("classify_info", mainPanel, HGUI_ABSOLUTE_POSITION,
			label_classify->getX(),
			label_classify->getBottom(), 400, 550);
	textArea_classify->setBackgroundColor(0x333333);
	//textArea_classify->setVisibleBorder(false);


	return mainPanel;
}

hPanel * testApp::panel4(hGui * gui, hEvents * events) {
	hPanel *mainPanel = gui->addPanel("mainPanel2", NULL, HGUI_ABSOLUTE_POSITION,
			gui->margin1,					// x
			gui->margin1*3, 				// y
			ofGetWidth()-gui->margin1*2,	// width
			ofGetHeight()-gui->margin1*5,	//height
			true);							// border
	mainPanel->setVisibleBackground(true);


	hLabel * label_drawer = gui->addLabel("", mainPanel, HGUI_TOP_LEFT,
			gui->margin1,
			gui->margin1, "draw gesture board");

	OnlineGestureDrawer * drawer = new OnlineGestureDrawer("recognizer_online", mainPanel, HGUI_BOTTOM,
			0,
			0, 550, 550);
	gui->addWidget(drawer);

	hLabel * label_classify_online = gui->addLabel("recognized_online", mainPanel, HGUI_ABSOLUTE_POSITION,
			drawer->getRight()+gui->margin1*2,
			label_drawer->getY(), "");

	return mainPanel;
}

void testApp::setDarkColors(void)
// Example of changing the colors of the GUI
{

	hGui * gui = hGui::getInstance();

	gui->setBackgroundColor	(0x333333);
	gui->setBorderColor		(0xCCCCCC);
	gui->setFillColor		(0x555555);
	gui->setAltFillColor	(0x557766);
	gui->setButtonShadowColor(0x999999);

	gui->setScrollHandleColor(0xDDDDDD);
	gui->setScrollButtonColor(0xDDDDDD);
	gui->setTextColor		(0xFFFFFF);
	gui->setTextColor2		(0x000000);
	gui->setAlertTextColor	(0xCCCCCC);

	gui->setDisableColor	(0x999999);
	gui->setEditTextColor	(0xFFFFFF); //(0x113388);
	gui->setEditTextColor2	(0xFFFFFF); //(0x113388);
	gui->setEditBackColor	(0xCCDDEE);
	gui->setCaretColor		(0x000000);

	gui->setLabelSelColor	(0xBBBBFF);
	gui->setItemSelColor	(0xBBBBBB);
	gui->setItemSelTextColor(0x333333);
	gui->setTabBoxSelColor	(0x4477CC);
	gui->setButtonBoxSelColor(0x44CC77);

	gui->setCheckBoxColor	(0x44CC77);
	gui->setSliderColor		(0x999999);
	gui->setTwoDSliderColor	(0x33BB66);
	gui->setCounterColor	(0x33BB66);

	gui->setDialogColor		(0xE5E5E5);
	gui->setMessageBoxColor	(0x77FFAA);
	gui->setAlertColor		(0xFF7777);

	gui->setTabBoxSelColor(0xBBBBBB);
}

void testApp::setupGui() {
	gui = hGui::getInstance();
	gui->setup("fonts/mono.ttf", 9);
	gui->setFixedFont("fonts/mono.ttf", 9, -1);
	gui->addListeners();
	setDarkColors();

	hEvents * events = hEvents::getInstance();
	events->setup();
	events->addObject("testApp", this);

	int mainTabBoxWidth = ofGetWidth()-gui->margin1*2;

	hTabBox *  mainTabBox = gui->addTabBox("mainTabBox", NULL, HGUI_ABSOLUTE_POSITION,
			gui->margin1,
			gui->margin1, mainTabBoxWidth);
	gui->setRootWidget(mainTabBox);

	mainTabBox->addItems(4);

	mainTabBox->setItemLabel(1, "train set");
	mainTabBox->setItemLabel(2, "learned gesture");
	mainTabBox->setItemLabel(3, "recognition");
	mainTabBox->setItemLabel(4, "online recognition");

	hPanel *p1 = panel1(gui, events);
	mainTabBox->setItemPanel(1, p1);
	mainTabBox->selectItem(1);

	hPanel *p2 = panel2(gui, events);
	mainTabBox->setItemPanel(2, p2);

	hPanel *p3 = panel3(gui, events);
	mainTabBox->setItemPanel(3, p3);

	hPanel *p4 = panel4(gui, events);
	mainTabBox->setItemPanel(4, p4);

	mainTabBox->selectItem(1);
}

#endif /* GUI_H_ */
