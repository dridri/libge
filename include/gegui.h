#ifndef __GE_GUI__
#define __GE_GUI__

//Flags for windows
#define GE_WINDOW_STYLE 0x01
	#define GE_WINDOW_STYLE_DEFAULT 0x00
	#define GE_WINDOW_STYLE_BLACK 0x01
	#define GE_WINDOW_STYLE_WHITE 0x02
	#define GE_WINDOW_STYLE_GRAY 0x03
#define GE_WINDOW_TRANSPARENT 0x20
#define GE_WINDOW_FONT 0x02
	#define GE_WINDOW_FONT_DEFAULT 0x00
#define GE_WINDOW_FONT_SIZE 0x03

#define GE_GUI_WIDGET 0x00
#define GE_GUI_WINDOW 0x01
#define GE_GUI_BUTTON 0x02
#define GE_GUI_PROGRESSBAR 0x03
#define GE_GUI_INPUTBOX 0x04
#define GE_GUI_COMBOBOX 0x05
#define GE_GUI_CHECKBOX 0x06
#define GE_GUI_RADIOBUTTON 0x07
#define GE_GUI_LISTWIDGET 0x20

#define GE_GUI_PROGRESS_NONE 0
#define GE_GUI_PROGRESS_PERCENT 1

#define GE_GUI_ALIGNX_CENTER 0x1
#define GE_GUI_ALIGNX_RIGHT 0x2
#define GE_GUI_ALIGNY_CENTER 0x4
#define GE_GUI_ALIGNY_BOTTOM 0x8

typedef struct ge_GuiWidget ge_GuiWidget;

typedef struct ge_GuiStyle {
	ge_Image* top_left;
	ge_Image* top_right;
	ge_Image* top;
	ge_Image* bottom_left;
	ge_Image* bottom_right;
	ge_Image* bottom;
	ge_Image* left;
	ge_Image* right;
	ge_Image* close;
	
	ge_Image* button;
	ge_Image* checkbox;
	ge_Image* box;
	ge_Image* progressbar;
	ge_Image* comboitem;
	ge_Image* listitem;

	ge_Font* font;
	u32 back_color;
} ge_GuiStyle;

typedef struct ge_GuiAreaObject ge_GuiAreaObject;
struct ge_GuiAreaObject {
	int type;
    int x, y;
    int absx, absy;
	int flags;
	void* object;
	void (*render)(ge_GuiAreaObject*, ge_GuiStyle*);
};

typedef struct ge_GuiArea {
	int x, y, width, height;
	int nObjects;
	ge_GuiAreaObject* objs;
} ge_GuiArea;

typedef struct ge_GuiKeyEvent {
	bool pressed[GE_KEYS_COUNT+32];
} ge_GuiKeyEvent;

typedef struct ge_GuiCursorEvent ge_GuiCursorEvent;
struct ge_GuiCursorEvent {
	int x;
	int y;
	int last_x;
	int last_y;
	int warp_x;
	int warp_y;
	u32 ticks;
	ge_GuiCursorEvent* last;
};

#define WIDGET_PROPS \
	int type; \
	char* name; \
	int x, y, width, height; \
	ge_Image* img; \
	u32 back_color; \
	ge_Image* fore_image; \
	short textpos[2]; \
	bool focused; \
	bool gotfocus; \
	bool lostfocus; \
	bool visible; \
	bool enabled; \
	char* text; \
	ge_GuiArea* area; \
	ge_GuiWidget* parent; \
	ge_GuiCursorEvent* cursor_event; \
	ge_GuiCursorEvent* last_cursor_event; \
	void (*KeyEventFunc)(ge_GuiWidget*, ge_GuiKeyEvent*); \
	void (*CursorEventFunc)(ge_GuiWidget*, ge_GuiCursorEvent*);

struct ge_GuiWidget {
	WIDGET_PROPS
};

typedef struct ge_GuiWindow {
	WIDGET_PROPS
	int flags;
	ge_GuiStyle* style;
	char* title;
	bool moving;
	//Sys
	int winwidth, winheight;
} ge_GuiWindow;

typedef struct ge_GuiButton {
	WIDGET_PROPS
    int state;
    int pressed;
} ge_GuiButton;

typedef struct ge_GuiCheckBox {
	WIDGET_PROPS
    int state;
    bool checked;
} ge_GuiCheckBox;

typedef struct ge_GuiRadioButton {
	WIDGET_PROPS
    int state;
    bool checked;
} ge_GuiRadioButton;

typedef struct ge_GuiProgressBar {
	WIDGET_PROPS
	int textmode;
	int status, max;
} ge_GuiProgressBar;

typedef struct ge_GuiInputBox {
	WIDGET_PROPS
	int maxlen;
	int maxlines;
} ge_GuiInputBox;

typedef struct ge_GuiComboBoxItem {
	char* text;
	u32 back_color;
} ge_GuiComboBoxItem;

typedef struct ge_GuiComboBox {
	WIDGET_PROPS
	int state;
	int pressed;
	int currentIndex;
	ge_GuiComboBoxItem* currentItem;
	ge_GuiComboBoxItem* items;
	int nItems;
} ge_GuiComboBox;

typedef struct ge_GuiListWidgetItem {
	char* text;
	u32 front_color;
	u32 back_color;
	ge_Image* image;
} ge_GuiListWidgetItem;

typedef struct ge_GuiListWidget {
	WIDGET_PROPS
	int currentIndex;
	int firstVisible;
	ge_GuiListWidgetItem* currentItem;
	ge_GuiListWidgetItem* items;
	int nItems;
} ge_GuiListWidget;

// Styles
LIBGE_API void geGuiStyleFont(ge_GuiStyle* style, ge_Font* font, int size);

// Windows
LIBGE_API ge_GuiWindow* geGuiCreateWindow(const char* title, int width, int height, int flags);
LIBGE_API void geGuiWindowLinkObject(ge_GuiWindow* win, int x, int y, void* object, int flags);
LIBGE_API void* geGuiWindowFindObject(ge_GuiWindow* win, const char* name);

// Area
LIBGE_API void geGuiAreaLinkObject(ge_GuiArea* area, int x, int y, void* object, int flags);
LIBGE_API void* geGuiAreaFindObject(ge_GuiArea* area, const char* name);

// Widget
LIBGE_API ge_GuiWidget* geGuiCreateWidget(int width, int height);
LIBGE_API void geGuiWidgetSetName(void* widget, const char* name);
LIBGE_API void geGuiWidgetLinkObject(ge_GuiWidget* widget, int x, int y, void* object, int flags);
LIBGE_API void geGuiWidgetForegroundImage(ge_GuiWidget* widget, ge_Image* img);

// Button
LIBGE_API ge_GuiButton* geGuiCreateButton(const char* text, int width, int height);

// Check Box
LIBGE_API ge_GuiCheckBox* geGuiCreateCheckBox(const char* text, int width, int height);

// Radio Button
LIBGE_API ge_GuiRadioButton* geGuiCreateRadioButton(const char* text, int width, int height);

// Input Box
LIBGE_API ge_GuiInputBox* geGuiCreateInputBox(int width, int height, const char* base_text, int maxlen);

// Progress Bar
LIBGE_API ge_GuiProgressBar* geGuiCreateProgressBar(int width, int height, int max);

// Combo Box
LIBGE_API ge_GuiComboBox* geGuiCreateComboBox(int width, int height);
LIBGE_API void geGuiComboBoxAddItem(ge_GuiComboBox* box, const char* txt);

// List Widget
LIBGE_API ge_GuiListWidget* geGuiCreateListWidget(int width, int height);
LIBGE_API ge_GuiListWidgetItem* geGuiListWidgetAddItem(ge_GuiListWidget* box, const char* txt);
LIBGE_API void geGuiListWidgetClear(ge_GuiListWidget* list);

LIBGE_API void geGuiGiveFocus(void* widget);

#endif // __GE_GUI__
