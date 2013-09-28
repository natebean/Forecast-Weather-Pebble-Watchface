#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#include "http.h"
#include "util.h"
#include "weather_layer.h"
#include "forecast_layer.h"
#include "link_monitor.h"
#include "config.h"
#include "receive_keys.h"

#define MY_UUID { 0x91, 0x41, 0xB6, 0x28, 0xBC, 0x89, 0x49, 0x8E, 0xB1, 0x47, 0x04, 0x9F, 0x49, 0xC0, 0x99, 0xAD }

PBL_APP_INFO(MY_UUID,
			"Bean Weather", "Nate Bean", // Modification of "Roboto Weather" by Martin Rosinski
             1, 0, /* App version */
             RESOURCE_ID_IMAGE_MENU_ICON,
             APP_INFO_WATCH_FACE);

#define WIDTH 75
static const int mod_minute_weather = 15;
static const int mod_minute_check = 6;

// POST variables
#define WEATHER_KEY_LATITUDE 1
#define WEATHER_KEY_LONGITUDE 2
#define WEATHER_KEY_UNIT_SYSTEM 3
	
	
#define WEATHER_HTTP_COOKIE 1949327671
#define TIME_HTTP_COOKIE 1131038282

Window window;          /* main window */
TextLayer date_layer;   /* layer for the date */
TextLayer time_layer;   /* layer for the time */
TextLayer min_max_temp_layer;   //layer for the time
TextLayer message_background_layer;   //layer for the time
TextLayer message_layer;   //layer for the time

GFont font_date;        /* font for date */
GFont font_hour;        /* font for hour */
GFont font_minute;      /* font for minute */
GFont font_small;      /* font for minute */
GFont font_very_small;      /* font for minute */

static int initial_minute;
const char *day_of_week[] = {"Sun", "Mon","Tues","Wed", "Thu", "Fri", "Sat"};
char today_char[2];
int today_int;
int tom_int;
char data_pack_string[124] = { "\0"};
char data_packs[NUM_TERMS][TERM_LEN] = {"\0"};

//Weather Stuff
static int our_latitude, our_longitude;
static bool located = false;
static bool initial_request = true;
static bool has_temperature = false;


//Sunrise/set info
char sunrise_string[12] = "sunrise";
char sunset_string[12] = "sunset";

WeatherLayer weather_layer;
ForecastLayer today_forecast_layer;
ForecastLayer tom_forecast_layer;
ForecastLayer after_forecast_layer;

void request_weather();
void current_time_text(char * output_string, int string_size);

void failed(int32_t cookie, int http_status, void* context) {
	if(cookie == 0 || cookie == WEATHER_HTTP_COOKIE) {
		if (http_status== 1002 || http_status == 1064){
		  static char time_string[] = "99:99";	
		  current_time_text(time_string, sizeof(time_string));
		  static char output_string[10] = "R 99:99";
		  memmove(&output_string[2], &time_string,sizeof(time_string)-1);
		  text_layer_set_text(&message_layer, output_string);

		  located = false;
		  request_weather();
		}else if (http_status==1008){
	      text_layer_set_text(&message_layer, "Disconnected");
		  located = false;
		  link_monitor_handle_failure(http_status);
		}else {
	      text_layer_set_text(&message_layer, itoa(http_status));
		  //Re-request the location and subsequently weather on next minute tick
		  located = false;
		  link_monitor_handle_failure(http_status);
		}
	}
}


void incoming_params_parser(char terms[NUM_TERMS][TERM_LEN], char* incoming)
{
  int found[NUM_TERMS]= {0};
  int i, j =0, k=0;
  for (i = 0; i < (int)strlen(incoming); i ++){
       if (incoming[i] == '|'){
         found[j] = i;
         j++;
       }
      }

  for (k=0; k< NUM_TERMS; k++){
    int temp = found[k];
    int left_adj = 0, left_side = 0, right_side=0, len_adj =0;
    if (temp > 0){
       if(k!=0){
         left_side = found[k-1];
         left_adj = 1;
         len_adj = 1;
    }
    right_side = found[k];
    int len = (right_side - left_side);
    memcpy(&terms[k],&incoming[left_side+left_adj],len-len_adj);
    }
  }
}//func

void success(int32_t cookie, int http_status, DictionaryIterator* received, void* context) {
	if(cookie != WEATHER_HTTP_COOKIE) return;
	Tuple* data_pack_tuple = dict_find(received, 1); //only one key
  if (data_pack_tuple){
      strcpy(data_pack_string, data_pack_tuple->value->cstring);
      incoming_params_parser(data_packs,data_pack_string);
  }
	/*Tuple* icon_tuple = dict_find(received, WEATHER_KEY_ICON);*/
    int icon = atoi(data_packs[WEATHER_KEY_ICON]);
    if(icon >= 0 && icon < 10) {
      weather_layer_set_icon(&weather_layer, icon);
    } else {
      weather_layer_set_icon(&weather_layer, WEATHER_ICON_NO_WEATHER);
    }
  /*}*/
	/*Tuple* temperature_tuple = dict_find(received, WEATHER_KEY_TEMPERATURE);*/
	/*if(temperature_tuple) {*/
		weather_layer_set_temperature(&weather_layer,atoi(data_packs[WEATHER_KEY_TEMPERATURE]));
		has_temperature = true;
		static char time_string[] = "99:99";	
		current_time_text(time_string,sizeof(time_string));
		text_layer_set_text(&message_layer, time_string);
	/*}*/
  /*Tuple* sunrise_tuple = dict_find(received, WEATHER_KEY_SUNRISE);*/
  /*Tuple* sunset_tuple = dict_find(received, WEATHER_KEY_SUNSET);*/
  /*if(sunrise_tuple) {*/
    strcpy(sunrise_string, "rise ");
    strcat(&sunrise_string[0],data_packs[WEATHER_KEY_SUNRISE]);
    text_layer_set_text(&weather_layer.sunrise_layer, sunrise_string);
  /*}*/
  /*if(sunset_tuple) {*/
    strcpy(sunset_string, "set ");
    strcat(&sunset_string[0],data_packs[WEATHER_KEY_SUNSET]);
    text_layer_set_text(&weather_layer.sunset_layer, sunset_string);
  /*}*/
  forecast_layer_update(&today_forecast_layer,data_packs, WEATHER_KEY_TODAY_ICON,
      WEATHER_KEY_TODAY_MIN, WEATHER_KEY_TODAY_MAX);

  forecast_layer_update(&tom_forecast_layer,data_packs, WEATHER_KEY_TOM_ICON,
      WEATHER_KEY_TOM_MIN, WEATHER_KEY_TOM_MAX);

  link_monitor_handle_success();
}

void location(float latitude, float longitude, float altitude, float accuracy, void* context) {
	// Fix the floats
	our_latitude = latitude * 10000;
	our_longitude = longitude * 10000;
	located = true;
	request_weather();
}

void reconnect(void* context) {
	located = false;
	request_weather();
}

void request_weather();

/* Called by the OS once per minute. Update the time and date.
*/
void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t)
{
    /* Need to be static because pointers to them are stored in the text
    * layers.
    */
    static char date_text[] = "XXX 00";
	  static char time_string[] = "99:99";	

    (void)ctx;  /* prevent "unused parameter" warning */

    if (t->units_changed & DAY_UNIT)
    {		
	    string_format_time(date_text,
                           sizeof(date_text),
                           "%b %d",
                           t->tick_time);

      string_format_time(today_forecast_layer.day_name,
                           sizeof(today_forecast_layer.day_name),
                           "%a",
                           t->tick_time);

      string_format_time(today_char,
                           sizeof(today_char),
                           "%w",
                           t->tick_time);

      today_int = util_atoi(today_char);
      /*today_int = 5;*/
      tom_int = (today_int > 5) ? 0 : today_int + 1;
      /*after_int = (tom_int > 5) ? 0 : tom_int + 1;*/

      text_layer_set_text(&today_forecast_layer.day_layer,today_forecast_layer.day_name);
      strcpy(tom_forecast_layer.day_name, day_of_week[tom_int]);
      text_layer_set_text(&tom_forecast_layer.day_layer,tom_forecast_layer.day_name);

		if (date_text[4] == '0') /* is day of month < 10? */
		{
		    /* This is a hack to get rid of the leading zero of the
			   day of month
            */
            memmove(&date_text[4], &date_text[5], sizeof(date_text) - 1);
		}
        text_layer_set_text(&date_layer, date_text);
    }

	current_time_text(time_string,sizeof(time_string));
	text_layer_set_text(&time_layer, time_string);
	
	if(initial_request || !has_temperature || (t->tick_time->tm_min % mod_minute_weather) == initial_minute)
	{
		// Every mod_minute_weather minutes, request updated weather
		http_location_request();
		initial_request = false;
	}
	else if (t->tick_time->tm_min % mod_minute_check == initial_minute)
	{
		// Ping the phone every mod_minute_check minutes
		link_monitor_ping();
	}
}

/* Initialize the application.
*/
void handle_init(AppContextRef ctx)
{
    PblTm tm;
    PebbleTickEvent t;
    ResHandle res_d;
    ResHandle res_h;
    ResHandle res_small;
    ResHandle res_very_small;

    window_init(&window, "Bean Weather");
    window_stack_push(&window, true /* Animated */);
    window_set_background_color(&window, GColorBlack);

    resource_init_current_app(&APP_RESOURCES);

    res_d = resource_get_handle(RESOURCE_ID_FUTURA_18);
    res_h = resource_get_handle(RESOURCE_ID_FUTURA_24);
    res_small = resource_get_handle(RESOURCE_ID_FUTURA_14);
    res_very_small = resource_get_handle(RESOURCE_ID_FUTURA_12);

    font_date = fonts_load_custom_font(res_d);
    font_hour = fonts_load_custom_font(res_h);
    font_minute = fonts_load_custom_font(res_h);
    font_small = fonts_load_custom_font(res_small);
    font_very_small = fonts_load_custom_font(res_very_small);

	//Add simple time layer
    /*text_layer_init(&time_layer, window.layer.frame);*/
    text_layer_init(&time_layer, GRect(0,0,WIDTH,30));
    text_layer_set_text_color(&time_layer, GColorWhite);
    text_layer_set_text_alignment(&time_layer, GTextAlignmentCenter);
    text_layer_set_background_color(&time_layer, GColorClear);
    text_layer_set_font(&time_layer, font_hour);
    layer_add_child(&window.layer, &time_layer.layer);

	//Add date layer
    text_layer_init(&date_layer, GRect(0,30,WIDTH,20));
    text_layer_set_text_color(&date_layer, GColorWhite);
    text_layer_set_background_color(&date_layer, GColorClear);
    text_layer_set_font(&date_layer, font_date);
    text_layer_set_text_alignment(&date_layer, GTextAlignmentCenter);
    layer_add_child(&window.layer, &date_layer.layer);

	//Add message background layer
    text_layer_init(&message_background_layer, GRect(0,58,144,20));
    text_layer_set_background_color(&message_background_layer, GColorWhite);
    layer_add_child(&window.layer, &message_background_layer.layer);

	//Add message layer
    text_layer_init(&message_layer, GRect(0,2,144,20));
    text_layer_set_text_color(&message_layer, GColorBlack);
    text_layer_set_background_color(&message_layer, GColorClear);
    text_layer_set_font(&message_layer, font_small);
    text_layer_set_text_alignment(&message_layer, GTextAlignmentCenter);
    layer_add_child(&message_background_layer.layer, &message_layer.layer);

	// Add weather layer
    weather_layer_init(&weather_layer, GPoint(70, 0));
    layer_add_child(&window.layer, &weather_layer.layer);

	// Add forecast layer
    forecast_layer_init(&today_forecast_layer, GPoint(0, 85));
    layer_add_child(&window.layer, &today_forecast_layer.layer);
	
	// Add forecast layer
    forecast_layer_init(&tom_forecast_layer, GPoint(0, 115));
    layer_add_child(&window.layer, &tom_forecast_layer.layer);

	http_register_callbacks((HTTPCallbacks){
		.failure=failed,
		.success=success,
		.reconnect=reconnect,
		.location=location},
		(void*)ctx);
	
	// Refresh time
	get_time(&tm);
    t.tick_time = &tm;
    t.units_changed = SECOND_UNIT | MINUTE_UNIT | HOUR_UNIT | DAY_UNIT;
	
	initial_minute = (tm.tm_min % mod_minute_weather);
	
	handle_minute_tick(ctx, &t);
}

void current_time_text(char * output_string, int string_size){
    PblTm tm;
    PebbleTickEvent t;
	  get_time(&tm);
    t.tick_time = &tm;
	
    string_format_time(output_string, string_size, "%I:%M", t.tick_time);
	
	if (output_string[0] == '0')
	{
		// This is a hack to get rid of the leading zero of the hour.
		memmove(&output_string[0], &output_string[1], string_size - 1);
	}
	
}


/* Shut down the application
*/
void handle_deinit(AppContextRef ctx)
{
    fonts_unload_custom_font(font_date);
    fonts_unload_custom_font(font_hour);
    fonts_unload_custom_font(font_minute);
    fonts_unload_custom_font(font_small);
    fonts_unload_custom_font(font_very_small);
	
	  weather_layer_deinit(&weather_layer);
}


/********************* Main Program *******************/

void pbl_main(void *params)
{
    PebbleAppHandlers handlers =
    {
        .init_handler = &handle_init,
        .deinit_handler = &handle_deinit,
        .tick_info =
        {
            .tick_handler = &handle_minute_tick,
            .tick_units = MINUTE_UNIT
        },
		.messaging_info = {
			.buffer_sizes = {
				.inbound = 124,
				.outbound = 256,
			}
		}
    };

    app_event_loop(params, &handlers);
}

void request_weather() {
	if(!located) {
		http_location_request();
		return;
	}
	// Build the HTTP request
	DictionaryIterator *body;
	HTTPResult result = http_out_get("http://natebean.info/weather_str.php", WEATHER_HTTP_COOKIE, &body);
	if(result != HTTP_OK) {
		weather_layer_set_icon(&weather_layer, WEATHER_ICON_NO_WEATHER);
		return;
	}
	dict_write_int32(body, WEATHER_KEY_LATITUDE, our_latitude);
	dict_write_int32(body, WEATHER_KEY_LONGITUDE, our_longitude);
	dict_write_cstring(body, WEATHER_KEY_UNIT_SYSTEM, UNIT_SYSTEM);
	// Send it.
	if(http_out_send() != HTTP_OK) {
		weather_layer_set_icon(&weather_layer, WEATHER_ICON_NO_WEATHER);
		return;
	}
}
