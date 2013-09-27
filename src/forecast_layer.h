#ifndef FORECAST_LAYER_H
#define FORECAST_LAYER_H

#include "weather_layer.h"
#include "receive_keys.h"

typedef struct {
	Layer layer;
	BmpContainer icon_layer;
	TextLayer day_layer;
	TextLayer temp_layer;
	//TextLayer temp_layer_background;
	bool has_weather_icon;
	//char temp_str[8];
  char day_name[4];
  char temp_min[5];
  char temp_max[5];
  char min_max_string[12];
} ForecastLayer;


void forecast_layer_init(ForecastLayer* forecast_layer, GPoint pos);
void forecast_layer_deinit(ForecastLayer* forecast_layer);
void forecast_layer_set_icon(ForecastLayer* forecast_layer, WeatherIcon icon);
void forecast_layer_update(ForecastLayer* forecast_layer, DictionaryIterator* received, int icon_key, int min_temp_key, int max_temp_key);

#endif
