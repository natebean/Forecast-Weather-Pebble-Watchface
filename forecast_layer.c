#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
#include "util.h"
#include "forecast_layer.h"
#include "weather_layer.h"

/*static uint8_t WEATHER_ICONS[] = {*/
  /*RESOURCE_ID_ICON_CLEAR_DAY,*/
  /*RESOURCE_ID_ICON_CLEAR_NIGHT,*/
  /*RESOURCE_ID_ICON_RAIN,*/
  /*RESOURCE_ID_ICON_SNOW,*/
  /*RESOURCE_ID_ICON_SLEET,*/
  /*RESOURCE_ID_ICON_WIND,*/
  /*RESOURCE_ID_ICON_FOG,*/
  /*RESOURCE_ID_ICON_CLOUDY,*/
  /*RESOURCE_ID_ICON_PARTLY_CLOUDY_DAY,*/
  /*RESOURCE_ID_ICON_PARTLY_CLOUDY_NIGHT,*/
  /*RESOURCE_ID_ICON_ERROR,*/
/*};*/

#define WIDTH 75
#define HEIGHT 55

void forecast_layer_init(ForecastLayer* forecast_layer, GPoint pos) {
	//layer_init(&forecast_layer->layer, GRect(pos.x, pos.y, 144, 80));
	layer_init(&forecast_layer->layer, GRect(pos.x, pos.y, WIDTH, HEIGHT));
	
	// Add background layer
	text_layer_init(&forecast_layer->temp_layer_background, GRect(0, 0, WIDTH, HEIGHT));
	text_layer_set_background_color(&forecast_layer->temp_layer_background, GColorBlack);
	layer_add_child(&forecast_layer->layer, &forecast_layer->temp_layer_background.layer);
	
    // Add temperature layer
	text_layer_init(&forecast_layer->temp_layer, GRect(WIDTH/2, 5, WIDTH/2, 20));
	text_layer_set_background_color(&forecast_layer->temp_layer, GColorClear);
	text_layer_set_text_alignment(&forecast_layer->temp_layer, GTextAlignmentRight);
	text_layer_set_text_color(&forecast_layer->temp_layer, GColorWhite);
	text_layer_set_font(&forecast_layer->temp_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FUTURA_18)));
  text_layer_set_text(&forecast_layer->temp_layer, "TBD");
	layer_add_child(&forecast_layer->layer, &forecast_layer->temp_layer.layer);

  /*// Add sunrise_layer*/
  text_layer_init(&forecast_layer->sunrise_layer, GRect(0, 25, WIDTH, 20));
  text_layer_set_background_color(&forecast_layer->sunrise_layer, GColorBlack);
  text_layer_set_text_alignment(&forecast_layer->sunrise_layer, GTextAlignmentCenter);
  text_layer_set_text_color(&forecast_layer->sunrise_layer, GColorWhite);
  text_layer_set_font(&forecast_layer->sunrise_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FUTURA_12)));
  layer_add_child(&forecast_layer->layer, &forecast_layer->sunrise_layer.layer);
  text_layer_set_text(&forecast_layer->sunrise_layer, "sunrise");

  /*// Add sunset_layer*/
  text_layer_init(&forecast_layer->sunset_layer, GRect(0, 40, WIDTH, 20));
  text_layer_set_background_color(&forecast_layer->sunset_layer, GColorBlack);
  text_layer_set_text_alignment(&forecast_layer->sunset_layer, GTextAlignmentCenter);
  text_layer_set_text_color(&forecast_layer->sunset_layer, GColorWhite);
  text_layer_set_font(&forecast_layer->sunset_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FUTURA_12)));
  layer_add_child(&forecast_layer->layer, &forecast_layer->sunset_layer.layer);
  text_layer_set_text(&forecast_layer->sunset_layer, "sunset");

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
  bmp_init_container(forecast_ICONS[icon], &forecast_layer->icon_layer);
  layer_add_child(&forecast_layer->layer, &forecast_layer->icon_layer.layer.layer);
  layer_set_frame(&forecast_layer->icon_layer.layer.layer, GRect(0,3, WIDTH/2, 25));
  forecast_layer->has_weather_icon = true;
}

void forecast_layer_set_temperature(ForecastLayer* forecast_layer, int16_t t) {
	memcpy(forecast_layer->temp_str, itoa(t), 4);
	int degree_pos = strlen(forecast_layer->temp_str);
	
	if (strlen(forecast_layer->temp_str) == 1 ||
		(strlen(forecast_layer->temp_str) == 2 && forecast_layer->temp_str[0] != '1')) {
	  // Don't move temperature if between 0-9° or 20°-99°
		/*text_layer_set_font(&forecast_layer->temp_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FUTURA_40)));*/
	  text_layer_set_font(&forecast_layer->temp_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FUTURA_18)));
	  text_layer_set_text_alignment(&forecast_layer->temp_layer, GTextAlignmentCenter);
	  memcpy(&forecast_layer->temp_str[degree_pos], "°", 3);
	} else if (strlen(forecast_layer->temp_str) == 2 && forecast_layer->temp_str[0] == '1') {
	  // Move temperature slightly to the left if between 10°-19°
		/*text_layer_set_font(&forecast_layer->temp_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FUTURA_40)));*/
	  text_layer_set_font(&forecast_layer->temp_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FUTURA_18)));
	  text_layer_set_text_alignment(&forecast_layer->temp_layer, GTextAlignmentLeft);
	  memcpy(&forecast_layer->temp_str[degree_pos], "°", 3); 
	} else if (strlen(forecast_layer->temp_str) > 2) { 
	  // Shrink font size if above 99° or below -9°
		/*text_layer_set_font(&forecast_layer->temp_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FUTURA_35)));*/
	  text_layer_set_font(&forecast_layer->temp_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FUTURA_18)));
	  text_layer_set_text_alignment(&forecast_layer->temp_layer, GTextAlignmentCenter);
	  memcpy(&forecast_layer->temp_str[degree_pos], "°", 3);
	}
	
	text_layer_set_text(&forecast_layer->temp_layer, forecast_layer->temp_str);
}

void forecast_layer_deinit(ForecastLayer* forecast_layer) {
	if(forecast_layer->has_weather_icon)
		bmp_deinit_container(&forecast_layer->icon_layer);
}
