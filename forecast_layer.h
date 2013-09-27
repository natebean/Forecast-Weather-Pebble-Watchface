#ifndef FORECAST_LAYER_H
#define FORECAST_LAYER_H

typedef struct {
	Layer layer;
	BmpContainer icon_layer;
	TextLayer temp_layer;
	TextLayer temp_layer_background;
  TextLayer sunrise_layer;
  TextLayer sunset_layer;
	bool has_weather_icon;
	char temp_str[5];
} ForecastLayer;

//typedef enum {
	//WEATHER_ICON_CLEAR_DAY = 0,
	//WEATHER_ICON_CLEAR_NIGHT,
	//WEATHER_ICON_RAIN,
	//WEATHER_ICON_SNOW,
	//WEATHER_ICON_SLEET,
	//WEATHER_ICON_WIND,
	//WEATHER_ICON_FOG,
	//WEATHER_ICON_CLOUDY,
	//WEATHER_ICON_PARTLY_CLOUDY_DAY,
	//WEATHER_ICON_PARTLY_CLOUDY_NIGHT,
	//WEATHER_ICON_NO_WEATHER,
	//WEATHER_ICON_COUNT
//} WeatherIcon;

void forecast_layer_init(ForecastLayer* forecast_layer, GPoint pos);
void forecast_layer_deinit(ForecastLayer* forecast_layer);
void forecast_layer_set_icon(ForecastLayer* forecast_layer, WeatherIcon icon);
void forecast_layer_set_temperature(ForecastLayer* forecast_layer, int16_t temperature);

#endif
