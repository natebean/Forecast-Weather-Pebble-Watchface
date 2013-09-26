#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
#include "util.h"
#include "weather_layer.h"

static uint8_t WEATHER_ICONS[] = {
	RESOURCE_ID_ICON_CLEAR_DAY,
	RESOURCE_ID_ICON_CLEAR_NIGHT,
	RESOURCE_ID_ICON_RAIN,
	RESOURCE_ID_ICON_SNOW,
	RESOURCE_ID_ICON_SLEET,
	RESOURCE_ID_ICON_WIND,
	RESOURCE_ID_ICON_FOG,
	RESOURCE_ID_ICON_CLOUDY,
	RESOURCE_ID_ICON_PARTLY_CLOUDY_DAY,
	RESOURCE_ID_ICON_PARTLY_CLOUDY_NIGHT,
	RESOURCE_ID_ICON_ERROR,
};

#define WIDTH 59
#define HEIGHT 80

void weather_layer_init(WeatherLayer* weather_layer, GPoint pos) {
	//layer_init(&weather_layer->layer, GRect(pos.x, pos.y, 144, 80));
	layer_init(&weather_layer->layer, GRect(pos.x, pos.y, WIDTH, HEIGHT));
	
	// Add background layer
	text_layer_init(&weather_layer->temp_layer_background, GRect(0, 0, WIDTH, 104));
	text_layer_set_background_color(&weather_layer->temp_layer_background, GColorWhite);
	layer_add_child(&weather_layer->layer, &weather_layer->temp_layer_background.layer);
	
    // Add temperature layer
	text_layer_init(&weather_layer->temp_layer, GRect(0, 40, WIDTH, 20));
	text_layer_set_background_color(&weather_layer->temp_layer, GColorClear);
	text_layer_set_text_alignment(&weather_layer->temp_layer, GTextAlignmentCenter);
	text_layer_set_font(&weather_layer->temp_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FUTURA_18)));
	layer_add_child(&weather_layer->layer, &weather_layer->temp_layer.layer);
	
    // Add message layer
	text_layer_init(&weather_layer->message_layer, GRect(0, 62, WIDTH, 20));
	text_layer_set_background_color(&weather_layer->message_layer, GColorWhite);
	text_layer_set_text_alignment(&weather_layer->message_layer, GTextAlignmentCenter);
	text_layer_set_text_color(&weather_layer->message_layer, GColorBlack);
	text_layer_set_font(&weather_layer->message_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FUTURA_14)));
	layer_add_child(&weather_layer->layer, &weather_layer->message_layer.layer);
	text_layer_set_text(&weather_layer->message_layer, "Loading");
	
	// Note absence of icon layer
	weather_layer->has_weather_icon = false;
}

void weather_layer_set_icon(WeatherLayer* weather_layer, WeatherIcon icon) {
	
  // Remove any possible existing weather icon
  if(weather_layer->has_weather_icon) {
    layer_remove_from_parent(&weather_layer->icon_layer.layer.layer);
    bmp_deinit_container(&weather_layer->icon_layer);
    weather_layer->has_weather_icon = false;
  }
  
  // Add weather icon
  bmp_init_container(WEATHER_ICONS[icon], &weather_layer->icon_layer);
  layer_add_child(&weather_layer->layer, &weather_layer->icon_layer.layer.layer);
  layer_set_frame(&weather_layer->icon_layer.layer.layer, GRect(0,-10, WIDTH, 50));
  weather_layer->has_weather_icon = true;
}

void weather_layer_set_temperature(WeatherLayer* weather_layer, int16_t t) {
	memcpy(weather_layer->temp_str, itoa(t), 4);
	int degree_pos = strlen(weather_layer->temp_str);
	
	if (strlen(weather_layer->temp_str) == 1 || 
		(strlen(weather_layer->temp_str) == 2 && weather_layer->temp_str[0] != '1')) {
	  // Don't move temperature if between 0-9° or 20°-99°
		/*text_layer_set_font(&weather_layer->temp_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FUTURA_40)));*/
	  text_layer_set_font(&weather_layer->temp_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FUTURA_18)));
	  text_layer_set_text_alignment(&weather_layer->temp_layer, GTextAlignmentCenter);
	  memcpy(&weather_layer->temp_str[degree_pos], "°", 3);
	} else if (strlen(weather_layer->temp_str) == 2 && weather_layer->temp_str[0] == '1') {
	  // Move temperature slightly to the left if between 10°-19°
		/*text_layer_set_font(&weather_layer->temp_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FUTURA_40)));*/
	  text_layer_set_font(&weather_layer->temp_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FUTURA_18)));
	  text_layer_set_text_alignment(&weather_layer->temp_layer, GTextAlignmentLeft);
	  memcpy(&weather_layer->temp_str[degree_pos], "°", 3); 
	} else if (strlen(weather_layer->temp_str) > 2) { 
	  // Shrink font size if above 99° or below -9°
		/*text_layer_set_font(&weather_layer->temp_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FUTURA_35)));*/
	  text_layer_set_font(&weather_layer->temp_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FUTURA_18)));
	  text_layer_set_text_alignment(&weather_layer->temp_layer, GTextAlignmentCenter);
	  memcpy(&weather_layer->temp_str[degree_pos], "°", 3);
	}
	
	text_layer_set_text(&weather_layer->temp_layer, weather_layer->temp_str);
}

void weather_layer_deinit(WeatherLayer* weather_layer) {
	if(weather_layer->has_weather_icon)
		bmp_deinit_container(&weather_layer->icon_layer);
}
