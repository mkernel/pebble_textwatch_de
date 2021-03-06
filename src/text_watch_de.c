#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
#include "resource_ids.auto.h"

#define MY_UUID { 0xF8, 0xA9, 0x5B, 0x9A, 0x9F, 0x8C, 0x4A, 0x5B, 0xA9, 0xFE, 0xF9, 0x7A, 0x3A, 0xE9, 0x4F, 0x3A }
PBL_APP_INFO(MY_UUID,
             "Text Watch DE", "Enno Welbers",
             1, 0, /* App version */
             RESOURCE_ID_ICON,
             APP_INFO_WATCH_FACE);

Window window;
TextLayer firstLayer;
TextLayer secondLayer;
TextLayer thirdLayer;
TextLayer fourthLayer;

PropertyAnimation firstLayerAnim;
PropertyAnimation secondLayerAnim;
PropertyAnimation thirdLayerAnim;

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
char fourthLine[100];

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

int lastHour=-1;
int lastMinute=-1;
int thisHour;
int thisMinute;

void finalizeTimeUpdate(Animation *animation, void*data)
{
  lastHour=thisHour;
  lastMinute=thisMinute;
}

void continueTimeUpdate(Animation *animation, void *data)
{
  fillHours(thisHour,thisMinute,firstLine);
  text_layer_set_text(&firstLayer,firstLine);
  fillMinutes(thisMinute,secondLine,thirdLine);
  text_layer_set_text(&secondLayer,secondLine);
  text_layer_set_text(&thirdLayer,thirdLine);
  if(lastHour!=thisHour)
  {
    property_animation_init_layer_frame(&firstLayerAnim,&firstLayer.layer,&GRect(144,0,144,45),&GRect(0,0,144,45));

    property_animation_init_layer_frame(&secondLayerAnim,&secondLayer.layer,&GRect(144,50,144,40),&GRect(0,50,144,40));
    animation_set_delay(&secondLayerAnim.animation,0.1);
    property_animation_init_layer_frame(&thirdLayerAnim,&thirdLayer.layer,&GRect(144,90,144,40),&GRect(0,90,144,40));
    animation_set_delay(&thirdLayerAnim.animation,0.2);

    animation_set_handlers(&thirdLayerAnim.animation,(AnimationHandlers){
      .started = NULL,
      .stopped = (AnimationStoppedHandler)finalizeTimeUpdate,
    },(void*)1);

    animation_schedule(&firstLayerAnim.animation);
    animation_schedule(&secondLayerAnim.animation);
    animation_schedule(&thirdLayerAnim.animation);
  }
  else if(lastMinute!=thisMinute)
  {
    property_animation_init_layer_frame(&secondLayerAnim,&secondLayer.layer,&GRect(144,50,144,40),&GRect(0,50,144,40));
    property_animation_init_layer_frame(&thirdLayerAnim,&thirdLayer.layer,&GRect(144,90,144,40),&GRect(0,90,144,40));
    animation_set_delay(&thirdLayerAnim.animation,0.1);

    animation_set_handlers(&thirdLayerAnim.animation,(AnimationHandlers){
      .started = NULL,
      .stopped = (AnimationStoppedHandler)finalizeTimeUpdate,
    },(void*)1);

    animation_schedule(&secondLayerAnim.animation);
    animation_schedule(&thirdLayerAnim.animation);
  }
}

void updateTime(unsigned int hour, unsigned int minute) 
{
  thisHour=hour;
  thisMinute=minute;
  if(lastHour==-1 && lastMinute==-1)
  {
    lastHour=hour;
    lastMinute=minute;
    continueTimeUpdate(NULL,NULL);
  }
  if((unsigned int)lastHour!=hour)
  {
    //we have to animate on hour-base
    property_animation_init_layer_frame(&firstLayerAnim,&firstLayer.layer,NULL,&GRect(-144,0,144,45));
    animation_set_delay(&firstLayerAnim.animation,0.2);

    property_animation_init_layer_frame(&secondLayerAnim,&secondLayer.layer,NULL,&GRect(-144,50,144,40));
    animation_set_delay(&secondLayerAnim.animation,0.1);
    property_animation_init_layer_frame(&thirdLayerAnim,&thirdLayer.layer,NULL,&GRect(-144,90,144,40));

    animation_set_handlers(&firstLayerAnim.animation,(AnimationHandlers){
      .started = NULL,
      .stopped = (AnimationStoppedHandler)continueTimeUpdate,
    },(void*)1);

    animation_schedule(&firstLayerAnim.animation);
    animation_schedule(&secondLayerAnim.animation);
    animation_schedule(&thirdLayerAnim.animation);
  }

  else if((unsigned int)lastMinute!=minute)
  {
    //we have to animate on minute-base
    property_animation_init_layer_frame(&secondLayerAnim,&secondLayer.layer,NULL,&GRect(-144,50,144,40));
    animation_set_delay(&secondLayerAnim.animation,0.1);
    property_animation_init_layer_frame(&thirdLayerAnim,&thirdLayer.layer,NULL,&GRect(-144,90,144,40));

    animation_set_handlers(&secondLayerAnim.animation,(AnimationHandlers){
      .started = NULL,
      .stopped = (AnimationStoppedHandler)continueTimeUpdate,
    },(void*)1);

    animation_schedule(&secondLayerAnim.animation);
    animation_schedule(&thirdLayerAnim.animation);
  }
}

void updateDate(PblTm *time)
{
  string_format_time(fourthLine,100,"%d.%m.%Y",time);
  text_layer_set_text(&fourthLayer,fourthLine);
}

void handle_init(AppContextRef ctx) {
  (void)ctx;

  resource_init_current_app(&APP_RESOURCES);

  window_init(&window, "Text Watch");
  window_set_background_color(&window,GColorBlack);

  text_layer_init(&firstLayer,GRect(0,0,144,45));
  text_layer_set_background_color(&firstLayer,GColorBlack);
  text_layer_set_text_color(&firstLayer,GColorWhite);
  text_layer_set_font(&firstLayer,fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));

  text_layer_init(&secondLayer,GRect(0,50,144,40));
  text_layer_set_background_color(&secondLayer,GColorBlack);
  text_layer_set_text_color(&secondLayer,GColorWhite);
  GFont font=fonts_load_custom_font(resource_get_handle(RESOURCE_ID_SUBHEADINGFONT_33));
  text_layer_set_font(&secondLayer,font);

  text_layer_init(&thirdLayer,GRect(0,90,144,40));
  text_layer_set_background_color(&thirdLayer,GColorBlack);
  text_layer_set_text_color(&thirdLayer,GColorWhite);
  text_layer_set_font(&thirdLayer,font);

  GFont subfont=fonts_load_custom_font(resource_get_handle(RESOURCE_ID_SUBHEADINGFONT_18));
  text_layer_init(&fourthLayer,GRect(0,150,144,20));
  text_layer_set_background_color(&fourthLayer,GColorBlack);
  text_layer_set_text_color(&fourthLayer,GColorWhite);
  text_layer_set_font(&fourthLayer,subfont);
  text_layer_set_text_alignment(&fourthLayer,GTextAlignmentRight);

  layer_add_child(&window.layer,&firstLayer.layer);
  layer_add_child(&window.layer,&secondLayer.layer);
  layer_add_child(&window.layer,&thirdLayer.layer);
  layer_add_child(&window.layer,&fourthLayer.layer);

  PblTm tick_time;

  get_time(&tick_time);
  updateTime(tick_time.tm_hour,tick_time.tm_min);
  updateDate(&tick_time);
  last_day=tick_time->tm_yday;
  // updateTime(7,52);
  window_stack_push(&window, true /* Animated */);
}

int last_day;

void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t) {
  (void)t;
  (void)ctx;
  updateTime(t->tick_time->tm_hour,t->tick_time->tm_min);
  if(last_day!=t->tick_time->tm_yday)
  {
    updateDate(t->tick_time);
    last_day=t->tick_time->tm_yday;
  }
  // updateTime(7,52);
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
