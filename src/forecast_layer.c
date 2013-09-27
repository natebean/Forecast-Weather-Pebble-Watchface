#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
#include "util.h"
#include "forecast_layer.h"
#include "weather_layer.h"


#define WIDTH 144
#define HEIGHT 55

void forecast_layer_init(ForecastLayer* forecast_layer, GPoint pos) {
	layer_init(&forecast_layer->layer, GRect(pos.x, pos.y, WIDTH, HEIGHT));
	
  // Add day layer
	text_layer_init(&forecast_layer->day_layer, GRect(0, 0, 44, 20));
	text_layer_set_background_color(&forecast_layer->day_layer, GColorClear);
	text_layer_set_text_alignment(&forecast_layer->day_layer, GTextAlignmentCenter);
	text_layer_set_text_color(&forecast_layer->day_layer, GColorWhite);
	text_layer_set_font(&forecast_layer->day_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FUTURA_18)));
  /*text_layer_set_text(&forecast_layer->day_layer, "AAA");*/
	layer_add_child(&forecast_layer->layer, &forecast_layer->day_layer.layer);

  // Add temperature layer
	text_layer_init(&forecast_layer->temp_layer, GRect(70, 2, 70, 20));
	text_layer_set_background_color(&forecast_layer->temp_layer, GColorClear);
	text_layer_set_text_alignment(&forecast_layer->temp_layer, GTextAlignmentCenter);
	text_layer_set_text_color(&forecast_layer->temp_layer, GColorWhite);
	text_layer_set_font(&forecast_layer->temp_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FUTURA_14)));
  text_layer_set_text(&forecast_layer->temp_layer, "000/000");
	layer_add_child(&forecast_layer->layer, &forecast_layer->temp_layer.layer);


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
  layer_set_frame(&forecast_layer->icon_layer.layer.layer, GRect(50,0, 25, 25));
  forecast_layer->has_weather_icon = true;
}

void forecast_layer_deinit(ForecastLayer* forecast_layer) {
	if(forecast_layer->has_weather_icon)
		bmp_deinit_container(&forecast_layer->icon_layer);
}

void forecast_layer_update(ForecastLayer* forecast_layer, DictionaryIterator* received,
    int icon_key, int min_temp_key, int max_temp_key){

	Tuple* icon_tuple = dict_find(received, icon_key);
	Tuple* min_temp_tuple = dict_find(received, min_temp_key);
  Tuple* max_temp_tuple = dict_find(received, max_temp_key);
  if(icon_tuple) {
    int icon = icon_tuple->value->int8;
    if(icon >= 0 && icon < 10) {
      forecast_layer_set_icon(forecast_layer, icon);
    } else {
      forecast_layer_set_icon(forecast_layer, WEATHER_ICON_NO_WEATHER);
    }
  }
	if(min_temp_tuple) {
    memcpy(forecast_layer->temp_min, itoa(min_temp_tuple->value->int16), 4);
	}
  if(max_temp_tuple) {
    memcpy(forecast_layer->temp_max, itoa(max_temp_tuple->value->int16), 4);
  }
  if(min_temp_tuple && max_temp_tuple){
    strcpy(forecast_layer->min_max_string,"");
    strcat(&(forecast_layer->min_max_string[0]),&(forecast_layer->temp_min[0]));
    strcat(&(forecast_layer->min_max_string[0]),"/");
    strcat(&(forecast_layer->min_max_string[0]),&(forecast_layer->temp_max[0]));
    text_layer_set_text(&forecast_layer->temp_layer, forecast_layer->min_max_string);
  }

}
