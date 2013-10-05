#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
#include "util.h"
#include "forecast_layer.h"
#include "weather_layer.h"


#define WIDTH 144
#define HEIGHT 55

void forecast_layer_init(ForecastLayer* forecast_layer, GPoint pos, bool time_bool) {
	layer_init(&forecast_layer->layer, GRect(pos.x, pos.y, WIDTH, HEIGHT));
  int temp_y;
  forecast_layer->time_bool = time_bool;
  if (time_bool)
  {
    temp_y = 0;
  }else{
    temp_y = 5;
  }
  // Add day layer
	text_layer_init(&forecast_layer->day_layer, GRect(0, temp_y, 55, 20));
	text_layer_set_background_color(&forecast_layer->day_layer, GColorClear);
	text_layer_set_text_alignment(&forecast_layer->day_layer, GTextAlignmentCenter);
	text_layer_set_text_color(&forecast_layer->day_layer, GColorWhite);
	text_layer_set_font(&forecast_layer->day_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FUTURA_14)));
	layer_add_child(&forecast_layer->layer, &forecast_layer->day_layer.layer);

  // Add temperature layer
	text_layer_init(&forecast_layer->temp_layer, GRect(75, temp_y, 70, 20));
	text_layer_set_background_color(&forecast_layer->temp_layer, GColorClear);
	text_layer_set_text_alignment(&forecast_layer->temp_layer, GTextAlignmentCenter);
	text_layer_set_text_color(&forecast_layer->temp_layer, GColorWhite);
	text_layer_set_font(&forecast_layer->temp_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FUTURA_14)));
  text_layer_set_text(&forecast_layer->temp_layer, "000/000");
	layer_add_child(&forecast_layer->layer, &forecast_layer->temp_layer.layer);

  if (time_bool)
  {

    // Add temperature time layer
    text_layer_init(&forecast_layer->temp_time_layer, GRect(75, 18, 70, 15));
    text_layer_set_background_color(&forecast_layer->temp_time_layer, GColorClear);
    text_layer_set_text_alignment(&forecast_layer->temp_time_layer, GTextAlignmentCenter);
    text_layer_set_text_color(&forecast_layer->temp_time_layer, GColorWhite);
    text_layer_set_font(&forecast_layer->temp_time_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FUTURA_10)));
    text_layer_set_text(&forecast_layer->temp_time_layer, "00:00/00:00");
    layer_add_child(&forecast_layer->layer, &forecast_layer->temp_time_layer.layer);

  }

	// Note absence of icon layer
	forecast_layer->has_weather_icon = false;
}

void forecast_layer_set_icon(ForecastLayer* forecast_layer, WeatherIcon icon) {
	
  // Remove any possible existing weather icon
  if(forecast_layer->has_weather_icon) {
    layer_remove_from_parent(&forecast_layer->icon_layer.layer.layer);
    bmp_deinit_container(&forecast_layer->icon_layer);
    forecast_layer->has_weather_icon = false;
  }

  // Add weather icon
  bmp_init_container(WEATHER_ICONS[icon], &forecast_layer->icon_layer);
  layer_add_child(&forecast_layer->layer, &forecast_layer->icon_layer.layer.layer);
  layer_set_frame(&forecast_layer->icon_layer.layer.layer, GRect(50,-2, 25, 25));
  forecast_layer->has_weather_icon = true;
}

void forecast_layer_deinit(ForecastLayer* forecast_layer) {
	if(forecast_layer->has_weather_icon)
		bmp_deinit_container(&forecast_layer->icon_layer);
}

void forecast_layer_update(ForecastLayer* forecast_layer,char data_packs[NUM_TERMS][TERM_LEN] ,
  int time_key, int icon_key, int temp_key, int precip_prob_key, int min_time_key, int max_time_key){

    int icon = util_atoi(data_packs[icon_key]);
    if(icon >= 0 && icon < 10) {
      forecast_layer_set_icon(forecast_layer, icon);
    } else {
      forecast_layer_set_icon(forecast_layer, WEATHER_ICON_NO_WEATHER);
    }

    memmove(forecast_layer->day_name, data_packs[time_key], 4);
    memmove(forecast_layer->temp, data_packs[temp_key], 4);
    memmove(forecast_layer->precip_prob, data_packs[precip_prob_key], 4);

  if(forecast_layer->temp && forecast_layer->precip_prob){
    strcpy(forecast_layer->output_string,"");
    strcat(&(forecast_layer->output_string[0]),&(forecast_layer->precip_prob[0]));
    strcat(&(forecast_layer->output_string[0]),"% ");
    strcat(&(forecast_layer->output_string[0]),&(forecast_layer->temp[0]));
    text_layer_set_text(&forecast_layer->temp_layer, forecast_layer->output_string);
  }

  if (forecast_layer->day_name){
    text_layer_set_text(&forecast_layer->day_layer, forecast_layer->day_name);
  }

  if (forecast_layer->time_bool)
  {

    memmove(forecast_layer->time_min, data_packs[min_time_key], 5);
    memmove(forecast_layer->time_max, data_packs[max_time_key], 5);

    if(forecast_layer->time_min && forecast_layer->time_max){
      strcpy(forecast_layer->min_max_time_string,"");
      strcat(&(forecast_layer->min_max_time_string[0]),&(forecast_layer->time_min[0]));
      strcat(&(forecast_layer->min_max_time_string[0]),"/");
      strcat(&(forecast_layer->min_max_time_string[0]),&(forecast_layer->time_max[0]));
      text_layer_set_text(&forecast_layer->temp_time_layer, forecast_layer->min_max_time_string);
    }

  }

}
