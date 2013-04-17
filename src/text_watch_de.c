#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
#include "resource_ids.auto.h"

#define MY_UUID { 0xF8, 0xA9, 0x5B, 0x9A, 0x9F, 0x8C, 0x4A, 0x5B, 0xA9, 0xFE, 0xF9, 0x7A, 0x3A, 0xE9, 0x4F, 0x3A }
PBL_APP_INFO(MY_UUID,
             "Text Watch DE", "Enno Welbers",
             1, 0, /* App version */
             DEFAULT_MENU_ICON,
             APP_INFO_WATCH_FACE);

Window window;
TextLayer firstLayer;
TextLayer secondLayer;
TextLayer thirdLayer;

char *hours[]={
  "Zwölf",
  "Eins",
  "Zwei",
  "Drei",
  "Vier",
  "Fünf",
  "Sechs",
  "Sieben",
  "Acht",
  "Neun",
  "Zehn",
  "Elf"
};

char *hoursException[]={
  NULL,
  "Ein"
};

char * minutes[] = {
  "Uhr",
  "Eins",
  "Zwei",
  "Drei",
  "Vier",
  "Fünf",
  "Sechs",
  "Sieben",
  "Acht",
  "Neun",
  "Zehn",
  "Elf",
  "Zwölf"
};

char * minutesException[] = {
  NULL,//Uhr
  "Ein"//Eins
};

char * minutesBigcomps[]={
  "",//0
  "zehn",//1
  "zwanzig",//2
  "dreißig",//3
  "vierzig",//4
  "fünfzig"//5
};

char * minutesBigcompsSolo[]={
  "",//0
  "Zehn",//1
  "Zwanzig",//2
  "Dreißig",//3
  "Vierzig",//4
  "Fünfzig"//5
};


char *minutesbuilding[]={
  "",//0
  "",//zehn
  "und",//zwanzig
  "und",//dreißig
  "und",//vierzig
  "und"//fünfzig
};

char firstLine[100];
char secondLine[100];
char thirdLine[100];

void fillMinutes(unsigned int minute_time, char *firstLine, char *secondLine)
{
  if(minute_time<(sizeof(minutes)/sizeof(minutes[0])))
  {
    strcpy(firstLine,minutes[minute_time]);
    strcpy(secondLine,"");
  }
  else
  {
    unsigned int smallpart=minute_time%10;
    unsigned int bigpart=minute_time/10;
    char *firstpart;
    char *secondpart;
    char *thirdpart;
    if(smallpart==0)
    {
      firstpart=minutesBigcompsSolo[bigpart];
      secondpart="";
      thirdpart="";
    }
    else
    {
      firstpart=minutes[smallpart];
      if(smallpart<(sizeof(minutesException)/sizeof(minutesException[0])) && minutesException[smallpart]!=NULL)
      {
        firstpart=minutesException[smallpart];
      }
      secondpart=minutesbuilding[bigpart];
      thirdpart=minutesBigcomps[bigpart];

      if(smallpart==6 && bigpart==1)
      {
        firstpart="Sech";
      }
      else if(smallpart==7 && bigpart==1)
      {
        firstpart="Sieb";
      }
    }
    strcpy(firstLine,"");
    strcpy(secondLine,"");
    strcat(firstLine,firstpart);
    strcat(firstLine,secondpart);
    strcat(secondLine,thirdpart);
  }
}

void fillHours(unsigned int hour_time,unsigned int minute_time, char *line)
{
  strcpy(line,"");
  hour_time=hour_time%12;
  if(minute_time==0 && hour_time<(sizeof(hoursException)/sizeof(hoursException[0])) && hoursException[hour_time]!=NULL)
  {
    strcpy(line,hoursException[hour_time]);
  }
  else
  {
    strcpy(line,hours[hour_time]);
  }
}

void updateTime(unsigned int hour, unsigned int minute) 
{
  fillHours(hour,minute,firstLine);
  text_layer_set_text(&firstLayer,firstLine);
  fillMinutes(minute,secondLine,thirdLine);
  text_layer_set_text(&secondLayer,secondLine);
  text_layer_set_text(&thirdLayer,thirdLine);
}


void handle_init(AppContextRef ctx) {
  (void)ctx;

  resource_init_current_app(&APP_RESOURCES);

  window_init(&window, "Text Watch");
  window_set_background_color(&window,GColorBlack);

  text_layer_init(&firstLayer,GRect(0,30,144,45));
  text_layer_set_background_color(&firstLayer,GColorBlack);
  text_layer_set_text_color(&firstLayer,GColorWhite);
  text_layer_set_font(&firstLayer,fonts_get_system_font(FONT_KEY_GOTHAM_42_BOLD));

  text_layer_init(&secondLayer,GRect(0,75,144,35));
  text_layer_set_background_color(&secondLayer,GColorBlack);
  text_layer_set_text_color(&secondLayer,GColorWhite);
  GFont font=fonts_load_custom_font(resource_get_handle(RESOURCE_ID_SUBHEADINGFONT_33));
  text_layer_set_font(&secondLayer,font);

  text_layer_init(&thirdLayer,GRect(0,110,144,40));
  text_layer_set_background_color(&thirdLayer,GColorBlack);
  text_layer_set_text_color(&thirdLayer,GColorWhite);
  text_layer_set_font(&thirdLayer,font);

  layer_add_child(&window.layer,&firstLayer.layer);
  layer_add_child(&window.layer,&secondLayer.layer);
  layer_add_child(&window.layer,&thirdLayer.layer);

  PblTm tick_time;

  get_time(&tick_time);
  // updateTime(tick_time.tm_hour,tick_time.tm_min);
  updateTime(7,27);
  window_stack_push(&window, true /* Animated */);
}

void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t) {
  (void)t;
  (void)ctx;
  // updateTime(t->tick_time->tm_hour,t->tick_time->tm_min);
  updateTime(7,27);
}

void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,
    .tick_info = {
      .tick_handler = &handle_minute_tick,
      .tick_units = MINUTE_UNIT
    }
  };
  app_event_loop(params, &handlers);
}
